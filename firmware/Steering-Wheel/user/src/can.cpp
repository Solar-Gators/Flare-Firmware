#include "can.h"

#include "../inc/steering_state.h"
#include "CanDriver.hpp"
#include "can_protocol.h"
#include "main.h"

// from old dashboard defines
#define MITSUBA_RPM_VELOCITY_LSB_BIT_INDEX 35  // 1rpm/lsb
#define MITSUBA_RPM_VELOCITY_LEN 12

#define MITSUBA_VOLTAGE_LSB_BIT_INDEX 0  // 0.5V/lsb
#define MITSUBA_VOLTAGE_LEN 10

#define MITSUBA_CURRENT_LSB_BIT_INDEX 10  // 1A/lsb
#define MITSUBA_CURRENT_LEN 9

#define MITSUBA_BATTERY_CURRENT_DIRECTION_BIT_INDEX \
    19  // 0 = plus current (discharge), 1 = minus current (charge)

#define WHEEL_CIRCUMFERENCE_INCHES 69.12

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

void initCan()
{
    ASSERT_HAL_OK(can_device.startCANDevice());
}

HAL_StatusTypeDef rearVCUInfoMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    state.actual_direction.store(static_cast<flare_can::Direction>(msg.data[1]),
                                 std::memory_order_relaxed);

    state.actual_array_contactors_status.store(static_cast<flare_can::ArrayContactors>(msg.data[3]),
                                               std::memory_order_relaxed);

    return HAL_OK;
}
HAL_StatusTypeDef rearVCUSuppBattMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint16_t supp_batt_voltage = (msg.data[0]) | (static_cast<uint16_t>(msg.data[1]) << 8);
    state.supp_batt_voltage_mv.store(supp_batt_voltage, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef bmsBatteryVoltageMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint16_t main_batt_voltage_mv = static_cast<uint16_t>(msg.data[0] << 8) | (msg.data[1]);
    state.main_batt_voltage_cv.store(main_batt_voltage_mv, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef bmsBatteryTempMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    // dc here is like decicelcius
    uint16_t highest_temp_cell_dc = static_cast<uint16_t>(msg.data[0] << 8) | (msg.data[1]);
    state.high_temp_dc.store(highest_temp_cell_dc, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef telemKillStatusMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    auto status = static_cast<flare_can::CarKilledStatus>(msg.data[0]);
    state.killed_status.store(status, std::memory_order_relaxed);
    return HAL_OK;
}

HAL_StatusTypeDef speedMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint8_t knots = msg.data[0];
    float mph = static_cast<float>(knots) * 1.15078f;

    state.car_speed.store(static_cast<uint8_t>(std::round(mph)), std::memory_order_relaxed);
    return HAL_OK;
}

HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx)
{
    // need to get rpm here
    uint64_t full_data = 0;
    for (int i = 0; i < 8; i++)
    {
        full_data = (full_data << 8) | msg.data[7 - i];  // lsb first
    }

    uint16_t motor_rpm =
        (full_data >> MITSUBA_RPM_VELOCITY_LSB_BIT_INDEX) & ((1 << MITSUBA_RPM_VELOCITY_LEN) - 1);
    /*
    uint16_t motor_voltage = (full_data >> MITSUBA_VOLTAGE_LSB_BIT_INDEX) & ((1 << MITSUBA_VOLTAGE_LEN) - 1);
    uint16_t motor_current = (full_data >> MITSUBA_CURRENT_LSB_BIT_INDEX) & ((1 << MITSUBA_CURRENT_LEN) - 1);
    uint8_t motor_current_direction = (full_data >> MITSUBA_BATTERY_CURRENT_DIRECTION_BIT_INDEX) & 0x01;
    */

    state.motor_rpm.store(motor_rpm, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef frontVCUThrottleMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint8_t throttle_low = msg.data[0];
    uint8_t throttle_high = msg.data[1];
    volatile uint16_t throttle_value =
        (static_cast<uint16_t>(throttle_high) << 8) | static_cast<uint16_t>(throttle_low);

    auto percent =
        static_cast<uint16_t>((static_cast<uint32_t>(throttle_value) * 100 + 2047) / 4095);

    state.throttle_percent_debug.store(percent,
                                       std::memory_order_relaxed);  // store as percent 0 - 100

    state.brake_state.store(msg.data[7], std::memory_order_relaxed);

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

HAL_StatusTypeDef bmsFaultMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint16_t bms_faults = (static_cast<uint16_t>(msg.data[0]) << 8) | msg.data[1];
    state.bms_state.store(bms_faults, std::memory_order_relaxed);
    return HAL_OK;
}