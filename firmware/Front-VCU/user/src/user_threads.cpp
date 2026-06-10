#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "frontvcu.h"
#include "main.h"
#include "watchdog.hpp"

#include <atomic>

void init_user()
{
    frontvcu::init();
}

void StartHeartbeat_user(void* argument)
{
    sg::Watchdog wdog1;
    wdog1.IWDG_Init();

    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        wdog1.Kick();
        osDelay(500);
    }
}

void StartCANMessagesTX_user(void* argument)
{
    sg::Watchdog wdog2;
    wdog2.IWDG_Init();

    for (;;)
    {
        frontvcu::sendCANMessagesTX();
        wdog2.Kick();
        osDelay(20);
    }
}

void StartLoadsControl_user(void* argument)
{
    sg::Watchdog wdog3;
    wdog3.IWDG_Init();

    for (;;)
    {
        frontvcu::writeLoadsControl();
        wdog3.Kick();
        osDelay(30);
    }
}

void StartCurrentSense_user(void* argument)
{
    sg::Watchdog wdog4;
    wdog4.IWDG_Init();

    for (;;)
    {
        frontvcu::readCurrentSense();
        frontvcu::processHornAndFan();
        wdog4.Kick();
        osDelay(40);
    }
}

void StartBreakSense_user(void* argument)
{
    sg::Watchdog wdog5;
    wdog5.IWDG_Init();

    for (;;)
    {
        frontvcu::readBrakeSense();
        wdog5.Kick();
        osDelay(15);
    }
}

void StartCalculateCC_user(void* argument)
{
    sg::Watchdog wdog6;
    wdog6.IWDG_Init();

    for (;;)
    {
        frontvcu::updateThrottleCommand();
        wdog6.Kick();
        osDelay(19);
    }
}