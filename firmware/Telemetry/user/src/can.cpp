#include "can.h"

#include "CanDriver.hpp"

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

namespace telem
{

void can_init()
{
    ASSERT_TRUE(telem::can_device.addCallbackId(
        0x064, sg::CANFrameIDType::STANDARD, &telem::steeringRequestsCallback));
    ASSERT_TRUE(telem::can_device.addCallbackId(
        0x020, sg::CANFrameIDType::STANDARD, &telem::rearVCUStatusCallback));
    ASSERT_HAL_OK(telem::can_device.startCANDevice());
}

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    // do stuff
    volatile int x = 5;
    return HAL_OK;
}

HAL_StatusTypeDef rearVCUStatusCallback(const sg::CANFrame& frame, void* ctx)
{
    // do stuff
    volatile int x = 5;
    return HAL_OK;
}

}  // namespace telem
