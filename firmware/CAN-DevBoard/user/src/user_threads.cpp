#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "main.h"

extern CanHandle_t hfdcan1;
using namespace CANDriver;

HAL_StatusTypeDef testCallback(const CANFrame& msg, void* ctx)
{
    return HAL_OK;
}

void StartDefaultTask_user(void* argument)
{
    CANDevice CAN(&hfdcan1);

    CAN.AddFilterId(0x102, SG_CAN_ID_STD, SG_CAN_RTR_DATA, SG_CAN_PRIORITY_HIGH);
    CAN.addCallbackId(0x102, SG_CAN_ID_STD, testCallback);

    CAN.StartCANDevice();

    for (;;)
    {
        osDelay(500);
    }
}
