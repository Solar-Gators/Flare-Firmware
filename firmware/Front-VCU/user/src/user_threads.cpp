#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "frontvcu.h"
#include "main.h"

#include <atomic>

void init_user()
{
    frontvcu::init();
}

void StartHeartbeat_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}

void StartCANMessagesTX_user(void* argument)
{
    for (;;)
    {
        frontvcu::sendCANMessagesTX();
        osDelay(20);
    }
}

void StartLoadsControl_user(void* argument)
{
    for (;;)
    {
        frontvcu::writeLoadsControl();
        osDelay(30);
    }
}

void StartCurrentSense_user(void* argument)
{
    for (;;)
    {
        frontvcu::readCurrentSense();
        osDelay(40);
    }
}

void StartBreakSense_user(void* argument)
{
    for (;;)
    {
        frontvcu::readBrakeSense();
        osDelay(15);
    }
}

void StartCalculateCC_user(void* argument)
{
    for (;;)
    {
        frontvcu::updateThrottleCommand();
        osDelay(19);
    }
}