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
    CANDevice CAN(&hfdcan1);

    CAN.AddFilterId(0x102, SG_CAN_ID_STD, SG_CAN_RTR_DATA, SG_CAN_PRIORITY_HIGH);
    CAN.AddFilterRange(0x700, 4, SG_CAN_ID_STD, SG_CAN_RTR_DATA, SG_CAN_PRIORITY_HIGH);

    CAN.addCallbackId(0x102, SG_CAN_ID_STD, testCallback);
    CAN.addCallbackRange(0x700, 4, SG_CAN_ID_STD, loggingCallback);

    CAN.StartCANDevice();

    CANFrame msg1(0x100, SG_CAN_ID_STD, SG_CAN_RTR_DATA, FDCAN_DLC_BYTES_64);
    uint8_t data[64] = {};
    msg1.LoadData(data, 64);

    CAN.Send(&msg1);

    for (;;)
    {
        osDelay(500);
    }
}
