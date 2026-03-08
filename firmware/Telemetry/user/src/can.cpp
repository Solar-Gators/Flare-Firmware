#include "can.h"

#include "CanDriver.hpp"
#include "telem_state.h"

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

namespace telem
{

void canInit()
{
    ASSERT_TRUE(telem::can_device.addCallbackId(
        0x064, sg::CANFrameIDType::STANDARD, &telem::steeringRequestsCallback));
    ASSERT_TRUE(telem::can_device.addCallbackId(
        0x020, sg::CANFrameIDType::STANDARD, &telem::rearVCUStatusCallback));
    ASSERT_TRUE(telem::can_device.addCallbackId(
        0x040, sg::CANFrameIDType::STANDARD, &telem::bmsFaultsMessageCallback));
    ASSERT_HAL_OK(telem::can_device.startCANDevice());
}

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    // byte 0 is turn signals
    telem::turn_signals_status.store(static_cast<flare_can::TurnSignals>(frame.data[0]),
                                     std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef rearVCUStatusCallback(const sg::CANFrame& frame, void* ctx)
{
    // do stuff
    volatile int x = 5;
    return HAL_OK;
}

HAL_StatusTypeDef bmsFaultsMessageCallback(const sg::CANFrame& frame, void* ctx)
{
    if (frame.data[0] != 0 || frame.data[1] != 0)
    {
        telem::killed_status.store(flare_can::CarKilledStatus::DEAD, std::memory_order_relaxed);
    }
    return HAL_OK;
}

}  // namespace telem
