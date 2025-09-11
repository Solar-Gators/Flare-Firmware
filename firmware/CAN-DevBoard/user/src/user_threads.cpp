#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>
#include <stm32u5xx_hal_def.h>

#include "CanDriver.hpp"
#include "main.h"

#include <CanDriverApi.hpp>

extern CanHandle_t hfdcan1;
using namespace CANDriver;

HAL_StatusTypeDef testCallback(const CANFrame& msg, void* ctx)
{
    return HAL_OK;
}

HAL_StatusTypeDef loggingCallback(const CANFrame& msg, void* ctx)
{
    return HAL_OK;
}

void StartDefaultTask_user(void* argument)
{
    CANDevice& device = CANDevice::getInstance();
    device.StartCANDevice(&hfdcan1);

    for (;;)
    {
        osDelay(500);
    }
}
