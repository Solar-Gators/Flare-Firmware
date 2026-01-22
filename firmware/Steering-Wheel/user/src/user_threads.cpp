#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

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
    uint8_t button_states = 0xFF;
    for (;;)
    {
        // poll buttons

        // send can continuously regarding button states to tell car about status like
        // cc, lights, regen strength, direction, array contactors, etc,

        GPIO_PinState left = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
        GPIO_PinState right = HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
        if (left && right)
        {
        }

        osDelay(10);
    }
}