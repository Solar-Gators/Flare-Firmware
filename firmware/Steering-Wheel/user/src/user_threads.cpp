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

    // one time static draw
    // clears the entire screen once
    display.ClearScreen(RGB565_ORANGE);

    // draw all static labels once
    display.DrawText(5, 20, "Speed: ", RGB565_BLUE);
    display.DrawText(5, 40, "Sup volt: ", RGB565_BLUE);
    display.DrawText(5, 60, "Direction: ", RGB565_BLUE);
    display.DrawText(5, 80, "Main volt: ", RGB565_BLUE);
    display.DrawText(5, 100, "High temp: ", RGB565_BLUE);

    for (;;)
    {
        // TODO: create indicators for the right buttons to turn on as the lights on the actual buttons arent working
        // TODO: important info: cruise control, regenerative breaking, car speed, array connectors, sup batt voltage

        // speed draw (bounding box 75,20)
        uint8_t speed = steering::state.car_speed.load(std::memory_order_relaxed);
        snprintf(text_buffer.data(), sizeof(text_buffer), "%lu", static_cast<unsigned long>(speed));
        display.FillRect(75, 20, 60, 16, RGB565_ORANGE);
        display.DrawText(75, 20, text_buffer.data(), RGB565_BLUE);

        // supp batt v draw (bounding box 130,60)
        uint16_t sup_batt_mv = steering::state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
        uint32_t whole = sup_batt_mv / 1000;
        uint32_t frac = (sup_batt_mv % 1000);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(115, 40, 80, 16, RGB565_ORANGE);
        display.DrawText(115, 40, text_buffer.data(), RGB565_BLUE);

        // direction draw (bounding box 130,60)
        flare_can::Direction direction =
            steering::state.actual_direction.load(std::memory_order_relaxed);
        display.FillRect(130, 60, 110, 16, RGB565_ORANGE);
        if (static_cast<int>(direction) == 1)
            display.DrawText(130, 60, "Forward", RGB565_BLUE);
        else if (static_cast<int>(direction) == 0)
            display.DrawText(130, 60, "Backward", RGB565_BLUE);
        else
            display.DrawText(130, 60, "ERROR", RGB565_BLUE);

        // main batt v draw (bounding box 130,80)
        uint32_t cv = steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);
        whole = cv / 100;
        frac = (cv % 100);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(130, 80, 80, 16, RGB565_ORANGE);
        display.DrawText(130, 80, text_buffer.data(), RGB565_BLUE);

        // main batt high temp draw (bounding box 145,100)
        uint32_t dc = steering::state.high_temp_dc.load(std::memory_order_relaxed);
        whole = dc / 10;
        frac = (dc % 10);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(145, 100, 60, 16, RGB565_ORANGE);
        display.DrawText(145, 100, text_buffer.data(), RGB565_BLUE);

        // kill status draw
        if (steering::state.killed_status.load() == flare_can::CarKilledStatus::DEAD)
        {
            display.DrawText(240, 210, "KILLED", RGB565_RED);
        }
        else
        {
            display.FillRect(240, 210, 75, 16, RGB565_ORANGE);
        }

        // horn draw
        bool horn_active = steering::state.horn_requested_on.load(std::memory_order_relaxed);
        if (horn_active)
        {
            display.DrawText(280, 20, "H", RGB565_BLUE);
        }
        else
        {
            display.FillRect(280, 20, 20, 20, RGB565_ORANGE);
        }

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

    // turn signal blinking variables
    int blinker_ticks = 0; // also used for killed blinking
    bool blinker_on = false;

    for (;;)
    {
        // -- TURN SINGAL BLINKING LOGIC --
        // determine blink speed bc ticks shared with car killed status
        bool killed = (steering::state.killed_status.load() == flare_can::CarKilledStatus::DEAD);
        int target_ticks = killed ? 10 : 25;

        blinker_ticks++; // shared with car killed
        if(blinker_ticks >= target_ticks) // roughly every 500ms
        {
            blinker_on = !blinker_on;
            blinker_ticks = 0;
        }

        // turn signal blinking LED control
        auto current_signal = steering::state.turn_signals_requested.load();

        bool left_active = (current_signal == flare_can::TurnSignals::LEFT ||
            current_signal == flare_can::TurnSignals::HAZARDS);
        bool right_active = (current_signal == flare_can::TurnSignals::RIGHT ||
            current_signal == flare_can::TurnSignals::HAZARDS);

        // update the relative LEDs
        HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin,
            (left_active && blinker_on) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin,
            (right_active && blinker_on) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        // -- KILLED BLINKING LOGIC --
        if (killed)
        {
            auto pin_state = blinker_on ? GPIO_PIN_SET : GPIO_PIN_RESET;

            HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, pin_state);
            HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, pin_state);
            HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin, pin_state);
            HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, pin_state);
        }

        // -- HORN POLLING LOGIC --
        // this assumes horn is an active low pin
        bool horn_pressed = (HAL_GPIO_ReadPin(BUTTON7_GPIO_Port, BUTTON7_Pin) == GPIO_PIN_RESET);
        steering::state.horn_requested_on.store(horn_pressed);

        // -- UPDATE CAN BUS --
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
            static_cast<uint8_t>(horn_pressed);

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