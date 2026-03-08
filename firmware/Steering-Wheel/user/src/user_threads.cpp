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
    display.ClearScreen(RGB565_BLUE);

    // draw all static labels once
    display.SetTextSize(2);
    display.DrawText(140, 90, "MPH", RGB565_GRAY);
    display.DrawText(210, 90, "CC", RGB565_GRAY);
    display.DrawText(260, 90, "RB", RGB565_GRAY);

    display.DrawText(5, 130, "MAIN V:", RGB565_BLACK);
    display.DrawText(5, 155, "SUPP V:", RGB565_BLACK);
    display.DrawText(5, 180, "H TEMP:", RGB565_BLACK);
    display.DrawText(5, 205, "ARR CN:", RGB565_BLACK);

    display.SetTextSize(3);
    display.DrawText(10, 40, "R N F", RGB565_BLACK);
    display.DrawText(10, 70, "EC PW", RGB565_BLACK);

    // speed demo variables
    // uint8_t demo_speed = 0;
    // bool demo_up = true;

    for (;;)
    {
        // TODO: consolidate into functions, check justin's refactoring

        // --- TOP SECTION ---

        // direction highlight (R N F)
        display.SetTextSize(3);
        display.DrawText(10, 40, "R N F", RGB565_BLACK);

        auto current_dir = steering::state.direction_requested.load(std::memory_order_relaxed);
        if (current_dir == flare_can::Direction::REVERSE)
        {
            display.DrawText(10, 40, "R", RGB565_ORANGE);
        }
        else if (current_dir == flare_can::Direction::FORWARD)
        {
            display.DrawText(82, 40, "F", RGB565_ORANGE);
        }
        else
        {
            display.DrawText(46, 40, "N", RGB565_ORANGE);
        }

        // power mode highlight
        display.DrawText(10, 70, "EC PW", RGB565_BLACK);

        auto pwr_mode = steering::state.mc_power_mode_requested.load(std::memory_order_relaxed);
        if (pwr_mode == flare_can::MCPowerMode::ECO)
        {
            display.DrawText(10, 70, "EC", RGB565_ORANGE);
        }
        else
        {
            display.DrawText(64, 70, "PW", RGB565_ORANGE);
        }

        // speed draw
        // sample code to have the speed cycle through values for appearance
        // if (demo_up)
        // {
        //     demo_speed++;
        //     if (demo_speed >= 70) demo_up = false;
        // } else
        // {
        //     demo_speed--;
        //     if (demo_speed == 0) demo_up = true;
        // }
        // }
        // uint8_t speed = demo_speed;
        uint8_t speed = steering::state.car_speed.load(std::memory_order_relaxed);

        if (speed > 150)  // >150 means can/sensor error
        {
            snprintf(text_buffer.data(), sizeof(text_buffer), "ER");
        }
        else
        {
            snprintf(text_buffer.data(),
                     sizeof(text_buffer),
                     "%02lu",
                     static_cast<unsigned long>(speed));
            // add leading 0 with %02
        }
        display.SetTextSize(5);
        display.FillRect(130, 45, 100, 40, RGB565_BLUE);
        display.DrawText(130, 50, text_buffer.data(), RGB565_WHITE);

        // cruise draw
        uint8_t cc_val = steering::state.cc_mph_requested.load(std::memory_order_relaxed);
        snprintf(
            text_buffer.data(), sizeof(text_buffer), "%02lu", static_cast<unsigned long>(cc_val));
        display.SetTextSize(3);
        display.FillRect(210, 60, 30, 16, RGB565_BLUE);
        display.DrawText(210, 60, text_buffer.data(), RGB565_WHITE);

        // regen percent draw
        uint8_t regen = steering::state.regen_requested.load(std::memory_order_relaxed);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%02lu",
                 static_cast<unsigned long>((regen * 100) / 255));
        display.FillRect(260, 60, 30, 16, RGB565_BLUE);
        display.DrawText(260, 60, text_buffer.data(), RGB565_WHITE);

        // --- BOTTOM LEFT (BATT) SECTION ---
        display.SetTextSize(2);

        // supp batt v draw
        uint16_t sup_batt_mv = steering::state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
        uint32_t whole = sup_batt_mv / 1000;
        uint32_t frac = (sup_batt_mv % 1000);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(110, 155, 80, 16, RGB565_BLUE);
        display.DrawText(110, 155, text_buffer.data(), RGB565_BLACK);

        // main batt v draw
        uint32_t cv = steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);
        whole = cv / 100;
        frac = (cv % 100);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(110, 130, 80, 16, RGB565_BLUE);
        display.DrawText(110, 130, text_buffer.data(), RGB565_BLACK);

        // main batt high temp draw
        uint32_t dc = steering::state.high_temp_dc.load(std::memory_order_relaxed);
        whole = dc / 10;
        frac = (dc % 10);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(110, 180, 60, 16, RGB565_BLUE);
        display.DrawText(110, 180, text_buffer.data(), RGB565_BLACK);

        // array contactors draw
        auto arr_closed = steering::state.array_contactors_requested_closed.load();
        display.FillRect(110, 205, 100, 16, RGB565_BLUE);
        display.DrawText(110, 205, arr_closed ? "CLSD" : "OPEN", RGB565_BLACK);

        // -- BOTTOM RIGHT (STATUS) SECTION --

        // kill status draw
        if (steering::state.killed_status.load() == flare_can::CarKilledStatus::DEAD)
        {
            display.DrawText(240, 205, "KILLED", RGB565_RED);
        }
        else
        {
            display.FillRect(240, 205, 75, 16, RGB565_BLUE);
        }

        // horn draw
        bool horn_active = steering::state.horn_requested_on.load(std::memory_order_relaxed);
        if (horn_active)
        {
            display.DrawText(252, 130, "HORN", RGB565_BLACK);
        }
        else
        {
            display.FillRect(252, 130, 48, 16, RGB565_BLUE);
        }

        // headlights draw
        bool headlights_active =
            steering::state.headlights_requested_on.load(std::memory_order_relaxed);
        if (headlights_active)
        {
            display.DrawText(228, 155, "HDLGTS", RGB565_BLACK);
        }
        else
        {
            display.FillRect(228, 155, 72, 16, RGB565_BLUE);
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
    int blinker_ticks = 0;  // also used for killed blinking
    bool blinker_on = false;

    for (;;)
    {
        // -- TURN SINGAL BLINKING LOGIC --
        // determine blink speed bc ticks shared with car killed status
        bool killed = (steering::state.killed_status.load() == flare_can::CarKilledStatus::DEAD);
        int target_ticks = killed ? 10 : 20;  // faster blinking when car killed blinking

        blinker_ticks++;                    // shared with car killed
        if (blinker_ticks >= target_ticks)  // roughly every 500ms
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
        HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port,
                          BUTTON1_LED_Pin,
                          (left_active && blinker_on) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port,
                          BUTTON5_LED_Pin,
                          (right_active && blinker_on) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        // -- KILLED BLINKING LOGIC --
        if (killed)
        {
            // TODO: make killed car activate hazards?
            auto pin_state = blinker_on ? GPIO_PIN_SET : GPIO_PIN_RESET;

            HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, pin_state);
            HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, pin_state);
            HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin, pin_state);
            HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, pin_state);
        }

        // -- HORN POLLING LOGIC --
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
        steering_requests_frame.data[3] = static_cast<uint8_t>(horn_pressed);

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