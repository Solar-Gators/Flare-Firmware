#include "can.h"

#include "CanDriver.hpp"
#include "queue.h"
#include "telem_state.h"
#include "user_threads.hpp"

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

    // batt voltage
    ASSERT_TRUE(can_device.addCallbackId(0x041, sg::CANFrameIDType::STANDARD, &radioTXCallback));
    // MPPT 1 Input Measurements frame
    ASSERT_TRUE(can_device.addCallbackId(0x600, sg::CANFrameIDType::STANDARD, &radioTXCallback));
    // MPPT 1 Output Measurements frame
    ASSERT_TRUE(
        can_device.addCallbackId(0x600 + 1, sg::CANFrameIDType::STANDARD, &radioTXCallback));
    // MPPT 2 Input Measurements frame
    ASSERT_TRUE(can_device.addCallbackId(0x610, sg::CANFrameIDType::STANDARD, &radioTXCallback));
    // MPPT 2 Output Measurements frame
    ASSERT_TRUE(
        can_device.addCallbackId(0x610 + 1, sg::CANFrameIDType::STANDARD, &radioTXCallback));
    // MPPT 3 Input Measurements frame
    ASSERT_TRUE(can_device.addCallbackId(0x620, sg::CANFrameIDType::STANDARD, &radioTXCallback));
    // MPPT 3 Output Measurements frame
    ASSERT_TRUE(
        can_device.addCallbackId(0x620 + 1, sg::CANFrameIDType::STANDARD, &radioTXCallback));

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

HAL_StatusTypeDef radioTXCallback(const sg::CANFrame& frame, void* ctx)
{
    uint8_t packed_frame[10];
    packed_frame[0] = frame.can_id & 0xFF;
    packed_frame[1] = frame.can_id >> 8;
    memcpy(packed_frame + 2, frame.data.data(), 8);

    xQueueSend(radioTXQueue, &packed_frame, pdMS_TO_TICKS(100));
    return HAL_OK;
}