#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "buttons.h"
#include "can.h"
#include "main.h"
#include "steering_state.h"
#include "watchdog.hpp"

#include <array>
#include <string>

void init_user()
{
    // check atomics
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

    // start can
    steering::can_init();
}

void startHeartbeatTask_user(void* argument)
{
    // start watchdog
    sg::Watchdog wdog;
    wdog.MX_IWDG_Init();
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        wdog.Kick();
        osDelay(500);
    }
}

void startScreenTask_user(void* argument)
{
    // Top Right (0,0)
    // Bottom Right (240, 0)
    // Top Left (0, 320)
    // Bottom Left (240, 320)
    // Items build from top right to bottom left
    ILI9341 display(240, 320);
    display.Init();
    display.SetRotation(3);
    display.SetTextSize(2);
    std::array<char, 16> text_buffer{};

    for (;;)
    {
        display.ClearScreen(RGB565_ORANGE);

        // TODO: create indicators for the right buttons to turn on as the lights on the actual buttons arent working
        // TODO: important info: cruise control, regenerative breaking, car speed, array connectors, sup batt voltage

        // speed draw
        uint8_t speed = steering::state.car_speed.load(std::memory_order_relaxed);
        snprintf(text_buffer.data(), sizeof(text_buffer), "%lu", static_cast<unsigned long>(speed));
        display.DrawText(5, 20, "Speed: ", RGB565_BLUE);
        display.DrawText(75, 20, text_buffer.data(), RGB565_BLUE);

        // supp batt v draw
        uint16_t sup_batt_mv = steering::state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
        uint32_t whole = sup_batt_mv / 1000;
        uint32_t frac = (sup_batt_mv % 1000);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.DrawText(5, 40, "Sup volt: ", RGB565_BLUE);
        display.DrawText(115, 40, text_buffer.data(), RGB565_BLUE);

        // direction draw
        flare_can::Direction direction =
            steering::state.actual_direction.load(std::memory_order_relaxed);
        display.DrawText(5, 60, "Direction: ", RGB565_BLUE);
        if (static_cast<int>(direction) == 1)
            display.DrawText(130, 60, "Forward", RGB565_BLUE);
        else if (static_cast<int>(direction) == 0)
            display.DrawText(130, 60, "Backward", RGB565_BLUE);
        else
            display.DrawText(130, 60, "ERROR", RGB565_BLUE);

        // main batt v draw
        uint32_t cv = steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);
        whole = cv / 100;
        frac = (cv % 100);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.DrawText(5, 80, "Main volt: ", RGB565_BLUE);
        display.DrawText(130, 80, text_buffer.data(), RGB565_BLUE);

        // main batt high temp draw
        uint32_t dc = steering::state.high_temp_dc.load(std::memory_order_relaxed);
        whole = dc / 10;
        frac = (dc % 10);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.DrawText(5, 100, "High temp: ", RGB565_BLUE);
        display.DrawText(145, 100, text_buffer.data(), RGB565_BLUE);

        osDelay(500);
    }
}

void startPollButtons_user(void* argument)
{
    sg::CANFrame steering_requests_frame{0x064,
                                         sg::CANFrameIDType::STANDARD,
                                         sg::CANFrameRTRMode::DATA,
                                         sg::CANFrameLen::BYTES_8,
                                         0,
                                         {}};

    steering::initButtons();

    for (;;)
    {
        // turn signals
        steering_requests_frame.data[0] =
            static_cast<uint8_t>(steering::state.turn_signals_requested.load());

        // frwrd / reverse
        steering_requests_frame.data[1] =
            static_cast<uint8_t>(steering::state.direction_requested.load());

        // array
        steering_requests_frame.data[2] =
            static_cast<uint8_t>(steering::state.array_contactors_requested_closed.load());

        // horn
        steering_requests_frame.data[3] =
            static_cast<uint8_t>(!HAL_GPIO_ReadPin(HORN_PORT, HORN_PIN));

        // headlights
        steering_requests_frame.data[4] =
            static_cast<uint8_t>(steering::state.headlights_requested_on.load());

        // regen breaking strength
        steering_requests_frame.data[5] = 0;

        // pwr/eco request
        steering_requests_frame.data[6] =
            static_cast<uint8_t>(steering::state.mc_power_mode_requested.load());

        // cc mph
        steering_requests_frame.data[7] = 0;

        steering::can_device.send(steering_requests_frame);

        osDelay(20);
    }
}