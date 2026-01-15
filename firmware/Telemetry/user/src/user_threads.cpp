#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

void StartDefaultTask_user(void* argument)
{
    while (1)
    {
        osDelay(500);
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