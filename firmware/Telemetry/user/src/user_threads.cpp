#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"

void StartDefaultTask_user(void* argument)
{
    while (1)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
        HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
        HAL_GPIO_TogglePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin);
        osDelay(1000);
    }
}

void StartGPSReadBuffer_user(void* argument)
{
    while (1)
    {
        ;
    }
}

void StartGPSParseNMEA_user(void* argument)
{
    while (1)
    {
        ;
    }
}

void StartStartTXRadio_user(void* argument)
{
    while (1)
    {
        ;
    }
}