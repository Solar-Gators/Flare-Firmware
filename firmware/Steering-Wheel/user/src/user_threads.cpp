#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "main.h"
#include "steering_state.h"

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
        osDelay(500);
    }
}

void startPollButtons_user(void* argument)
{
    sg::CANFrame frame{0x064,
                       sg::CANFrameIDType::STANDARD,
                       sg::CANFrameRTRMode::DATA,
                       sg::CANFrameLen::BYTES_8,
                       0,
                       {}};

    uint8_t button_states = 0;
    for (;;)
    {
        // buttons lights test
        HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON7_LED_GPIO_Port, BUTTON7_LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_SET);

        // button layout
        // 1(lt)      5(rt) (both for hazards)
        // 2(horn)    6(fwrd/rev)
        // 3(array)   7(headlights)
        // 4(cc-)     8(cc+)

        // turn signals
        GPIO_PinState left = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
        GPIO_PinState right = HAL_GPIO_ReadPin(BUTTON5_GPIO_Port, BUTTON5_Pin);
        if (left && right)
        {
            frame.data[0] = static_cast<uint8_t>(TurnSignals::HAZARDS);
        }
        else if (left)
        {
            frame.data[0] = static_cast<uint8_t>(TurnSignals::LEFT);
        }
        else if (right)
        {
            frame.data[0] = static_cast<uint8_t>(TurnSignals::RIGHT);
        }
        else
        {
            frame.data[0] = static_cast<uint8_t>(TurnSignals::OFF);
        }

        // frwrd / reverse
        if (HAL_GPIO_ReadPin(BUTTON6_GPIO_Port, BUTTON6_Pin))
        {
            frame.data[1] = static_cast<uint8_t>(Direction::FORWARD);
        }
        else
        {
            frame.data[1] = static_cast<uint8_t>(Direction::REVERSE);
        }

        // array
        if (HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin))
        {
            frame.data[2] = 1;
        }
        else
        {
            frame.data[2] = 0;
        }

        osDelay(10);
    }
}