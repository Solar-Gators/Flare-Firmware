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
    while (1)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}

void startGPSReadBuffer_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startGPSParseNMEA_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startTXRadio_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}