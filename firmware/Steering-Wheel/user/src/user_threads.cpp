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
    display.ClearScreen(RGB565_BLACK);
    display.FillRect(0, 0, 320, 110, RGB565_BLUE);

    // draw all static labels once
    display.SetTextSize(2);
    display.DrawText(136, 100, "MPH", RGB565_GRAY);
    display.DrawText(50, 80, "CC", RGB565_GRAY);
    display.DrawText(10, 80, "RB", RGB565_GRAY);

    display.DrawText(5, 130, "MAIN V:", RGB565_BLUE);
    display.DrawText(5, 155, "SUPP V:", RGB565_BLUE);
    display.DrawText(5, 180, "H TEMP:", RGB565_BLUE);
    display.DrawText(5, 205, "ARR CN:", RGB565_BLUE);

    display.SetTextSize(4);
    display.DrawText(20, 40, "R N F", RGB565_BLACK);
    display.DrawText(20, 80, "EC PW", RGB565_BLACK);

    for (;;)
    {
        // TODO: consolidate into functions, check justin's refactoring

        // --- TOP SECTION ---

        // direction highlight (R N F)
        display.SetTextSize(4);
        display.DrawText(20, 40, "R N F", RGB565_BLACK);

        auto current_dir = steering::state.actual_direction.load(std::memory_order_relaxed);
        if (current_dir == flare_can::Direction::REVERSE)
        {
            display.DrawText(20, 40, "R", RGB565_ORANGE);
        }
        else if (current_dir == flare_can::Direction::FORWARD)
        {
            display.DrawText(92, 40, "F", RGB565_ORANGE);
        }
        else
        {
            display.DrawText(56, 40, "N", RGB565_ORANGE);
        }

        // power mode highlight
        display.DrawText(20, 80, "EC PW", RGB565_BLACK);

        auto pwr_mode = steering::state.mc_power_mode_requested.load(std::memory_order_relaxed);
        if (pwr_mode == flare_can::MCPowerMode::ECO)
        {
            display.DrawText(20, 80, "EC", RGB565_ORANGE);
        }
        else
        {
            display.DrawText(92, 80, "PW", RGB565_ORANGE);
        }


        // speed draw
        uint8_t speed = steering::state.car_speed.load(std::memory_order_relaxed);
        snprintf(text_buffer.data(), sizeof(text_buffer), "%lu", static_cast<unsigned long>(speed));
        display.SetTextSize(8);
        display.FillRect(136, 20, 100, 64, RGB565_BLUE);
        display.DrawText(136, 20, text_buffer.data(), RGB565_WHITE);

        // cruise draw
        uint8_t cc_val = steering::state.cc_mph_requested.load(std::memory_order_relaxed);
        snprintf(text_buffer.data(), sizeof(text_buffer), "%lu", static_cast<unsigned long>(cc_val));
        display.SetTextSize(4);
        display.FillRect(50, 40, 60, 32, RGB565_BLUE);
        display.DrawText(50, 40, text_buffer.data(), RGB565_WHITE);

        // regen percent draw
        uint8_t regen = steering::state.regen_requested.load(std::memory_order_relaxed);
        snprintf(text_buffer.data(), sizeof(text_buffer), "%lu", static_cast<unsigned long>((regen*100)/255));
        display.FillRect(10, 40, 60, 32, RGB565_BLUE);
        display.DrawText(10, 40, text_buffer.data(), RGB565_WHITE);

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
        display.FillRect(110, 155, 80, 16, RGB565_BLACK);
        display.DrawText(110, 155, text_buffer.data(), RGB565_BLUE);

        // main batt v draw
        uint32_t cv = steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);
        whole = cv / 100;
        frac = (cv % 100);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%02lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(110, 130, 80, 16, RGB565_ORANGE);
        display.DrawText(110, 130, text_buffer.data(), RGB565_BLUE);

        // main batt high temp draw
        uint32_t dc = steering::state.high_temp_dc.load(std::memory_order_relaxed);
        whole = dc / 10;
        frac = (dc % 10);
        snprintf(text_buffer.data(),
                 sizeof(text_buffer),
                 "%lu.%lu",
                 static_cast<unsigned long>(whole),
                 static_cast<unsigned long>(frac));
        display.FillRect(110, 180, 60, 16, RGB565_BLACK);
        display.DrawText(110, 180, text_buffer.data(), RGB565_BLUE);

        // array contactors draw
        auto arr_status = steering::state.actual_array_contactors_status.load();
        display.FillRect(110, 205, 100, 16, RGB565_BLACK);
        if (arr_status == flare_can::ArrayContactors::BOTH_OPEN)
        {
            display.DrawText(110, 205, "OPEN", RGB565_BLUE);
        }
        else if (arr_status == flare_can::ArrayContactors::MAIN_CLOSED)
        {
            display.DrawText(110, 205, "MN C", RGB565_BLUE);
        }
        else if (arr_status == flare_can::ArrayContactors::PRECHARGE_CLOSED)
        {
            display.DrawText(110, 205, "PC C", RGB565_BLUE);
        }

        // -- BOTTOM RIGHT (STATUS) SECTION --

        // kill status draw
        if (steering::state.killed_status.load() == flare_can::CarKilledStatus::DEAD)
        {
            display.DrawText(240, 205, "KILLED", RGB565_RED);
        }
        else
        {
            display.FillRect(240, 205, 75, 16, RGB565_BLACK);
        }

        // horn draw
        bool horn_active = steering::state.horn_requested_on.load(std::memory_order_relaxed);
        if (horn_active)
        {
            display.DrawText(240, 130, "HORN", RGB565_BLUE);
        }
        else
        {
            display.FillRect(240, 130, 60, 16, RGB565_BLACK);
        }

        // headlights draw
        bool headlights_active = steering::state.headlights_requested_on.load(std::memory_order_relaxed);
        if (headlights_active)
        {
            display.DrawText(180, 155, "HEADLIGHTS", RGB565_BLUE);
        }
        else
        {
            display.FillRect(180, 155, 120, 16, RGB565_BLACK);
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
        int target_ticks = killed ? 10 : 25; // faster blinking when car killed blinking

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