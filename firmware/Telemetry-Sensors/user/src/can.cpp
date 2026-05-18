#include "can.h"

#include "sensor_data.h"

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