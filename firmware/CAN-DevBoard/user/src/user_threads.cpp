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

    CAN.AddFilterId(
        0x102, sg::CANFrameIDType::STANDARD, sg::CANFrameRTRMode::DATA, sg::CANFramePriority::HIGH);
    CAN.AddFilterRange(0x700,
                       4,
                       sg::CANFrameIDType::STANDARD,
                       sg::CANFrameRTRMode::DATA,
                       sg::CANFramePriority::HIGH);

    CAN.addCallbackId(0x102, sg::CANFrameIDType::STANDARD, testCallback);
    CAN.addCallbackRange(0x700, 4, sg::CANFrameIDType::STANDARD, loggingCallback);

    CAN.StartCANDevice();

    sg::CANFrame msg1{0x100,
                      sg::CANFrameIDType::STANDARD,
                      sg::CANFrameRTRMode::DATA,
                      sg::CANFrameLen::BYTES_64,
                      0};
    uint8_t data[64] = {};
    msg1.LoadData(data, 64);

    // driver now holds a copy for simplicity, allows msg1 to be destructed
    CAN.Send(msg1);

    for (;;)
    {
        osDelay(500);
    }
}
