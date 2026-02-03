#include "can.h"

#include "CanDriver.hpp"

#define ASSERT(statement)    \
    if (statement != HAL_OK) \
        Error_Handler();

void can_init()
{
    ASSERT(
        can_device.addCallbackId(0x064, sg::CANFrameIDType::STANDARD, &steeringRequestsCallback));
    ASSERT(can_device.StartCANDevice());
}

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    // do stuff
    volatile int x = 5;
    return HAL_OK;
}
