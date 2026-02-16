#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "buttons.h"
#include "can.h"
#include "main.h"
#include "steering_state.h"

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
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}

void startScreenTask_user(void* argument)
{
    ILI9341 display(320, 240);
    display.Init();

    for (;;)
    {
        display.ClearScreen(RGB565_WHITE);

        uint32_t cv = steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);
        uint32_t whole = cv / 100;
        uint32_t frac = (cv % 100);
        std::array<char, 16> main_batt_v{};
        snprintf(main_batt_v.data(),
                 sizeof(main_batt_v),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.DrawText(30, 30, main_batt_v.data(), RGB565_BLACK);

        uint32_t dc = steering::state.high_temp_dc.load(std::memory_order_relaxed);
        whole = dc / 10;
        frac = (dc % 10);
        std::array<char, 16> high_temp_c{};
        snprintf(high_temp_c.data(),
                 sizeof(high_temp_c),
                 "%lu.%lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.DrawText(30, 60, high_temp_c.data(), RGB565_BLACK);

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