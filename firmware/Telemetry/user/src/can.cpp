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
    memcpy(packed_frame + 2, frame.data, 8);

    xQueueSend(radioTXQueue, &packed_frame, pdMS_TO_TICKS(100));
    return HAL_OK;
}