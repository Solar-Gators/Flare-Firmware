#include "can.h"

#include "CanDriver.hpp"
#include "queue.h"
#include "radio.h"
#include "telem_state.h"
#include "user_threads.hpp"

using namespace telem;

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
HAL_StatusTypeDef frontVCUThrottleMessageCallback(const sg::CANFrame& frame, void* ctx)
{
    // byte 7 is brakes
    brake_state.store(frame.data[7], std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    // byte 0 is turn signals
    turn_signals_status.store(static_cast<flare_can::TurnSignals>(frame.data[0]),
                              std::memory_order_relaxed);

    turn_signals_phase.store(static_cast<bool>(frame.data[4]), std::memory_order_relaxed);

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
    memcpy(packed_frame + 2, frame.data.data(), 8);

    if (addCanMessageToRadioQueue(frame.can_id, frame.data.data(), static_cast<uint8_t>(frame.len)))
    {
        return HAL_OK;  // like stuff like this too atp we are mixing hal return with os return
    }

    return HAL_ERROR;
}

// mppt sensor data
HAL_StatusTypeDef MPPT1InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx)
{
    float voltage = 0.0f;
    float current = 0.0f;

    std::memcpy(&voltage, &frame.data[0], sizeof(float));
    std::memcpy(&current, &frame.data[4], sizeof(float));

    mppt1.input.voltage.store(voltage, std::memory_order_relaxed);
    mppt1.input.current.store(current, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef MPPT2InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx)
{
    float voltage = 0.0f;
    float current = 0.0f;

    std::memcpy(&voltage, &frame.data[0], sizeof(float));
    std::memcpy(&current, &frame.data[4], sizeof(float));

    mppt2.input.voltage.store(voltage, std::memory_order_relaxed);
    mppt2.input.current.store(current, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef MPPT3InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx)
{
    float voltage = 0.0f;
    float current = 0.0f;

    std::memcpy(&voltage, &frame.data[0], sizeof(float));
    std::memcpy(&current, &frame.data[4], sizeof(float));

    mppt3.input.voltage.store(voltage, std::memory_order_relaxed);
    mppt3.input.current.store(current, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef MPPT1OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx)
{
    float voltage = 0.0f;
    float current = 0.0f;

    std::memcpy(&voltage, &frame.data[0], sizeof(float));
    std::memcpy(&current, &frame.data[4], sizeof(float));

    mppt1.output.voltage.store(voltage, std::memory_order_relaxed);
    mppt1.output.current.store(current, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef MPPT2OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx)
{
    float voltage = 0.0f;
    float current = 0.0f;

    std::memcpy(&voltage, &frame.data[0], sizeof(float));
    std::memcpy(&current, &frame.data[4], sizeof(float));

    mppt2.output.voltage.store(voltage, std::memory_order_relaxed);
    mppt2.output.current.store(current, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef MPPT3OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx)
{
    float voltage = 0.0f;
    float current = 0.0f;

    std::memcpy(&voltage, &frame.data[0], sizeof(float));
    std::memcpy(&current, &frame.data[4], sizeof(float));

    mppt3.output.voltage.store(voltage, std::memory_order_relaxed);
    mppt3.output.current.store(current, std::memory_order_relaxed);

    return HAL_OK;
}