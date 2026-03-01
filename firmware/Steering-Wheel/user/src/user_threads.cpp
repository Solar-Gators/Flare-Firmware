#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "main.h"
#include "steering.h"

#include <array>
#include <string>

void init_user()
{
    // total init
    steering::init();
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
    for (;;)
    {
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
        // uint8_t speed = demo_speed;
        osDelay(500);
    }
}

void startPollButtons_user(void* argument)
{
    /*
    // turn signal blinking variables
    int blinker_ticks = 0;  // also used for killed blinking
    bool blinker_on = false;
    */

    for (;;)
    {
        steering::sendRequestsMessage();
        steering::processHornButton();
        osDelay(30);

        /*
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
        */
    }
}