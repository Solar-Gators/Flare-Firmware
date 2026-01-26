#include "can.hpp"

#include <cmath>

#include "rearvcu_state.h"

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

HAL_StatusTypeDef throttleMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint8_t throttle_low = msg.data[0];
    uint8_t throttle_high = msg.data[1];
    uint16_t throttle_value =
        (static_cast<uint16_t>(throttle_high) << 8) | static_cast<uint16_t>(throttle_low);
    vcu_state.throttle_requested.store(throttle_value);

    ++vcu_state.can_messages_received;

    return HAL_OK;
}

HAL_StatusTypeDef driverMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    vcu_state.direction_requested.store(static_cast<Direction>(msg.data[0] & 0b00000001));
    vcu_state.array_contactors_requested_closed.store(static_cast<bool>(msg.data[2] & 0b00000010));
    vcu_state.regen_requested.store(msg.data[5]);
    vcu_state.mc_power_mode_requested.store(static_cast<MCPowerMode>(msg.data[6]));

    ++vcu_state.can_messages_received;

    return HAL_OK;
}

HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx)
{
    // need to get rpm here and calculate miles per hour
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

    // convert to m/s from rpm TODO: verify this is correct in real life lol maybe with speed gun or something idk
    double inches_per_sec = (motor_rpm * WHEEL_CIRCUMFERENCE_INCHES) / 60;
    double miles_per_sec = inches_per_sec / 63360;   // 1 mile = 63360 inches
    double miles_per_hour = (miles_per_sec * 3600);  // 1 hour = 3600 seconds

    vcu_state.car_speed.store(static_cast<uint8_t>(std::round(miles_per_hour)));

    ++vcu_state.can_messages_received;

    return HAL_OK;
}

void can_init()
{
    // throttle
    can_device.addCallbackId(
        0x040, sg::CANFrameIDType::STANDARD, &throttleMessageCallback, nullptr);

    // all the user inputs from steering wheel
    can_device.addCallbackId(0x064, sg::CANFrameIDType::STANDARD, &driverMessageCallback, nullptr);

    // mitsuba frame 0 comes from mc
    can_device.addCallbackId(
        0x08850225, sg::CANFrameIDType::EXTENDED, &mitsubaFrame0Callback, nullptr);

    // start
    can_device.StartCANDevice();
}
