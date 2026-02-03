#include "can.h"

#include "CanDriver.hpp"

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

void can_init()
{
    ASSERT_TRUE(
        can_device.addCallbackId(0x064, sg::CANFrameIDType::STANDARD, &steeringRequestsCallback));
    ASSERT_HAL_OK(can_device.StartCANDevice());
}

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    // do stuff
    volatile int x = 5;
    return HAL_OK;
}
