#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>
#include <stm32u5xx_hal_def.h>

#include "CanDriver.hpp"
#include "main.h"

#include <CanDriverApi.hpp>

extern CanHandle_t hfdcan1;

HAL_StatusTypeDef testCallback(const sg::CANFrame& msg, void* ctx)
{
    return HAL_OK;
}

HAL_StatusTypeDef loggingCallback(const sg::CANFrame& msg, void* ctx)
{
    return HAL_OK;
}

void StartDefaultTask_user(void* argument)
{
    sg::CANDevice CAN(&hfdcan1);

    CAN.AddFilterId(0x102, SG_CAN_ID_STD, SG_CAN_RTR_DATA, SG_CAN_PRIORITY_HIGH);
    CAN.AddFilterRange(0x700, 4, SG_CAN_ID_STD, SG_CAN_RTR_DATA, SG_CAN_PRIORITY_HIGH);

    CAN.addCallbackId(0x102, SG_CAN_ID_STD, testCallback);
    CAN.addCallbackRange(0x700, 4, SG_CAN_ID_STD, loggingCallback);

    CAN.StartCANDevice();

    sg::CANFrame msg1{0x100, SG_CAN_ID_STD, SG_CAN_RTR_DATA, sg::CANFrameLen::BYTES_64, 0};
    uint8_t data[64] = {};
    msg1.LoadData(data, 64);

    // driver now holds a copy for simplicity, allows msg1 to be destructed
    CAN.Send(msg1);

    for (;;)
    {
        osDelay(500);
    }
}
