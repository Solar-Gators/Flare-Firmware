#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.h"
#include "main.h"

void init_user()
{
    can_init();
}

void startDefaultTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
        HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
        HAL_GPIO_TogglePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin);
        osDelay(500);
    }
}

void startGPSReadBuffer_user(void* argument)
{
    for (;;)
    {
        osDelay(100);
    }
}

void startGPSParseNMEA_user(void* argument)
{
    for (;;)
    {
        osDelay(100);
    }
}

void startTXRadio_user(void* argument)
{
    for (;;)
    {
        osDelay(100);
    }
}