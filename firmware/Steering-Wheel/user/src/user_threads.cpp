#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"

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
    while (1)
    {
        // poll buttons

        osDelay(10);
    }
}