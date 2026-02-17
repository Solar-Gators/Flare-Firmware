#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.h"
#include "main.h"
#include "telem_state.h"

void init_user()
{
    // TODO: turn kill light off, no hardware for it yet lol
    HAL_GPIO_WritePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin, GPIO_PIN_RESET);
    telem::killSwitchButtonInit();
    telem::canInit();
}

void startHeartbeatTask_user(void* argument)
{
    flare_can::TurnSignals turn_signals{};

    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        HAL_GPIO_TogglePin(GPS_LED_GPIO_Port, GPS_LED_Pin);
        HAL_GPIO_TogglePin(RADIO_LED_GPIO_Port, RADIO_LED_Pin);

        // kill switch led logic
        if (telem::killed_status.load(std::memory_order_relaxed) ==
            flare_can::CarKilledStatus::DEAD)
        {
            HAL_GPIO_TogglePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin);
        }

        // turn signal led logic
        // if status changed, turn the lights off
        if (flare_can::TurnSignals new_turn_signals =
                telem::turn_signals_status.load(std::memory_order_relaxed);
            new_turn_signals != turn_signals)
        {
            HAL_GPIO_WritePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin, GPIO_PIN_RESET);
            turn_signals = new_turn_signals;
        }
        // toggle correct led's
        switch (turn_signals)
        {
            case flare_can::TurnSignals::LEFT:
                HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
                break;
            case flare_can::TurnSignals::RIGHT:
                HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
                break;
            case flare_can::TurnSignals::
                HAZARDS:  // works becuase we turn lights off when state changes
                HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
                HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
            default:
                Error_Handler();
        }

        osDelay(telem::led_toggle_period_ms);
    }
}

void startGPSReadBufferTask_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startGPSParseNMEATask_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startTXRadioTask_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startKillSwitchTask_user(void* argument)
{
    sg::CANFrame kill_frame{0x010,
                            sg::CANFrameIDType::STANDARD,
                            sg::CANFrameRTRMode::DATA,
                            sg::CANFrameLen::BYTES_1,
                            0,
                            {}};

    for (;;)
    {
        // send current kill switch message
        kill_frame.data[0] =
            static_cast<uint8_t>(telem::killed_status.load(std::memory_order_relaxed));
        telem::can_device.send(kill_frame);

        osDelay(50);
    }
}