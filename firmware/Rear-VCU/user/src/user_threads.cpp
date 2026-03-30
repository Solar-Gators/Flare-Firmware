#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"
#include "rearvcu.h"

#include <atomic>

// initialize devices here and such
void init_user()
{
    rearvcu::init();
}

[[noreturn]] void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        rearvcu::sendSuppBattFrame();
        osDelay(500);
    }
}

// should continuously write to pins like power/eco, direction, contactors, and send CAN message to tell car that mc is enabled and such
[[noreturn]] void startOutputsTask_user(void* argument)
{
    uint32_t next_wake = osKernelGetTickCount();
    for (;;)
    {
        next_wake += rearvcu::throttle_output_loop_rate_ms;
        osDelayUntil(next_wake);

        rearvcu::processArrayContactors();
        rearvcu::processMCOutputs();
        rearvcu::processRegenThrottleOutputs();
    }
}

void startSendStatusTask_user(void* argument)
{
    for (;;)
    {
        rearvcu::sendStatusMessage();
        osDelay(50);
    }
}