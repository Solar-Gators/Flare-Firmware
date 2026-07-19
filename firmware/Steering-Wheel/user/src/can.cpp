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

namespace
{

uint64_t packMitsubaLe(const sg::CANFrame& msg, int nbytes)
{
    uint64_t full_data = 0;
    for (int i = 0; i < nbytes; i++)
    {
        full_data |= (static_cast<uint64_t>(msg.data[i]) << (8 * i));
    }
    return full_data;
}

}  // namespace

HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx)
{
    uint64_t full_data = packMitsubaLe(msg, 8);

    // volatiles for debugger breakpoints
    volatile uint16_t batt_voltage_raw = (full_data >> MITSUBA_VOLTAGE_LSB_BIT_INDEX) &
                                         ((1u << MITSUBA_VOLTAGE_LEN) - 1);  // 0.5V/LSB
    volatile uint16_t batt_current_raw =
        (full_data >> MITSUBA_CURRENT_LSB_BIT_INDEX) & ((1u << MITSUBA_CURRENT_LEN) - 1);  // 1A/LSB
    volatile uint8_t batt_current_dir =
        (full_data >> MITSUBA_BATTERY_CURRENT_DIRECTION_BIT_INDEX) & 0x01;  // 0=discharge, 1=charge
    volatile uint16_t motor_rpm =
        (full_data >> MITSUBA_RPM_VELOCITY_LSB_BIT_INDEX) & ((1u << MITSUBA_RPM_VELOCITY_LEN) - 1);

    state.motor_rpm.store(motor_rpm, std::memory_order_relaxed);

    (void) batt_voltage_raw;
    (void) batt_current_raw;
    (void) batt_current_dir;
    (void) ctx;
    return HAL_OK;  // breakpoint here for frame 0
}

HAL_StatusTypeDef mitsubaFrame1Callback(const sg::CANFrame& msg, void* ctx)
{
    uint64_t full_data = packMitsubaLe(msg, 5);

    volatile uint8_t mode = full_data & 0x1;                  // 0=eco, 1=power
    volatile uint8_t control = (full_data >> 1) & 0x1;        // 0=CCM, 1=PWM
    volatile uint16_t accel_pos = (full_data >> 2) & 0x3FF;   // 0.5%/LSB
    volatile uint16_t regen_pos = (full_data >> 12) & 0x3FF;  // 0.5%/LSB
    volatile uint8_t motor_stat = (full_data >> 36) & 0x3;    // wait/fwd/rev
    volatile uint8_t drive = (full_data >> 38) & 0x1;         // 0=drive, 1=regen

    (void) mode;
    (void) control;
    (void) accel_pos;
    (void) regen_pos;
    (void) motor_stat;
    (void) drive;
    (void) ctx;
    return HAL_OK;  // breakpoint here for frame 1
}

HAL_StatusTypeDef mitsubaFrame2Callback(const sg::CANFrame& msg, void* ctx)
{
    uint64_t full_data = packMitsubaLe(msg, 5);

    // Sensor / system faults (1 = active)
    volatile bool ad_sensor_error = (full_data >> 0) & 1;
    volatile bool motor_curr_sensor_u_error = (full_data >> 1) & 1;
    volatile bool motor_curr_sensor_w_error = (full_data >> 2) & 1;
    volatile bool fet_therm_error = (full_data >> 3) & 1;
    volatile bool batt_volt_sensor_error = (full_data >> 5) & 1;
    volatile bool batt_curr_sensor_error = (full_data >> 6) & 1;
    volatile bool batt_curr_sensor_adj_error = (full_data >> 7) & 1;
    volatile bool motor_curr_sensor_adj_error = (full_data >> 8) & 1;
    volatile bool accel_pos_error = (full_data >> 9) & 1;  // throttle out of range (LED 6)
    volatile bool cont_volt_sensor_error = (full_data >> 11) & 1;
    volatile bool power_system_error = (full_data >> 16) & 1;
    volatile bool over_curr_error = (full_data >> 17) & 1;  // LED 1
    volatile bool over_volt_error = (full_data >> 19) & 1;  // LED 8
    volatile bool over_curr_limit = (full_data >> 21) & 1;
    volatile bool motor_system_error = (full_data >> 24) & 1;
    volatile bool motor_lock = (full_data >> 25) & 1;         // LED 4
    volatile bool hall_sensor_short = (full_data >> 26) & 1;  // LED 3
    volatile bool hall_sensor_open = (full_data >> 27) & 1;   // LED 3
    volatile uint8_t fet_oh_lvl = (full_data >> 32) & 0x3;    // LED 9: 0=ok, 1/2/3 heat stages

    (void) ad_sensor_error;
    (void) motor_curr_sensor_u_error;
    (void) motor_curr_sensor_w_error;
    (void) fet_therm_error;
    (void) batt_volt_sensor_error;
    (void) batt_curr_sensor_error;
    (void) batt_curr_sensor_adj_error;
    (void) motor_curr_sensor_adj_error;
    (void) accel_pos_error;
    (void) cont_volt_sensor_error;
    (void) power_system_error;
    (void) over_curr_error;
    (void) over_volt_error;
    (void) over_curr_limit;
    (void) motor_system_error;
    (void) motor_lock;
    (void) hall_sensor_short;
    (void) hall_sensor_open;
    (void) fet_oh_lvl;
    (void) ctx;
    return HAL_OK;  // breakpoint here for frame 2 faults
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
