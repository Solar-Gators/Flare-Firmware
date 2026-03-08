#include "can.h"

#include "CanDriver.hpp"
#include "telem_state.h"

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

void canInit()
{
    ASSERT_TRUE(
        can_device.addCallbackId(0x064, sg::CANFrameIDType::STANDARD, &steeringRequestsCallback));
    ASSERT_TRUE(
        can_device.addCallbackId(0x020, sg::CANFrameIDType::STANDARD, &rearVCUStatusCallback));
    ASSERT_TRUE(
        can_device.addCallbackId(0x040, sg::CANFrameIDType::STANDARD, &bmsFaultsMessageCallback));

    // MPPT 1
    ASSERT_TRUE(
        can_device.addCallbackId(0x040, sg::CANFrameIDType::STANDARD, &bmsFaultsMessageCallback));

    ASSERT_TRUE(
        can_device.addCallbackId(0x08950225, sg::CANFrameIDType::EXTENDED, &MPPT1_callback));

    ASSERT_HAL_OK(can_device.startCANDevice());
}

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    // byte 0 is turn signals
    turn_signals_status.store(static_cast<flare_can::TurnSignals>(frame.data[0]),
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
        killed_status.store(flare_can::CarKilledStatus::DEAD, std::memory_order_relaxed);
    }
    return HAL_OK;
}

HAL_StatusTypeDef MPPT1_callback(const sg::CANFrame& frame, void* ctx)
{
    volatile uint8_t var = 0;
    return HAL_OK;
}