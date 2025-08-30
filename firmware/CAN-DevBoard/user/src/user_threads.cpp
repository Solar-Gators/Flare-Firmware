#include "user_threads.hpp"

#include "CanDriver.hpp"
#include "cmsis_os2.h"  // this works cause cmsis_os includes cmsis_os2 so u could write either
#include "main.h"
#include "stm32u5xx_hal.h"

extern CanHandle_t hfdcan1;

void StartDefaultTask_user(void* argument)
{
    CANDevice& device = CANDevice::getInstance();
    device.StartCANDevice(&hfdcan1);

    for (;;)
    {
        osDelay(500);
    }
}
