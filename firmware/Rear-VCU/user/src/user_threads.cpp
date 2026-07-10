#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"
#include "rearvcu.h"
#include "watchdog.hpp"

#include <atomic>

// initialize devices here and such
void init_user()
{
    rearvcu::init();
}

[[noreturn]] void startHeartbeatTask_user(void* argument)
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

// should continuously write to pins like power/eco, direction, contactors, and send CAN message to tell car that mc is enabled and such
[[noreturn]] void startOutputsTask_user(void* argument)
{
    uint32_t next_wake = osKernelGetTickCount();

    sg::Watchdog wdog2;
    wdog2.IWDG_Init();

    for (;;)
    {
        next_wake += rearvcu::throttle_output_loop_rate_ms;
        osDelayUntil(next_wake);

        rearvcu::processArrayContactors();
        rearvcu::processMCOutputs();
        rearvcu::processRegenThrottleOutputs();
        wdog2.Kick();
    }
}

void startSendStatusTask_user(void* argument)
{
    sg::Watchdog wdog3;
    wdog3.IWDG_Init();

    for (;;)
    {
        rearvcu::sendStatusMessage();
        wdog3.Kick();
        osDelay(50);
    }
}

void startReadSupBat_user(void* argument)
{
    sg::Watchdog wdog4;
    wdog4.IWDG_Init();

    for (;;)
    {
        rearvcu::readSuppBatt();
        rearvcu::sendSuppBattFrame();
        wdog4.Kick();
        osDelay(30);
    }
}