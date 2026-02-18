#include "can.h"

#include "CanDriver.hpp"
#include "main.h"
#include "steering_state.h"

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

namespace steering
{

void can_init()
{
    // recieve message from rear vcu
    ASSERT_TRUE(can_device.addCallbackId(
        0x020, sg::CANFrameIDType::STANDARD, &rearVCUInfoMessageCallback, nullptr));
    ASSERT_TRUE(can_device.addCallbackId(
        0x041, sg::CANFrameIDType::STANDARD, &bmsBatteryVoltageMessageCallback, nullptr));
    ASSERT_TRUE(can_device.addCallbackId(
        0x042, sg::CANFrameIDType::STANDARD, &bmsBatteryTempMessageCallback, nullptr));
    ASSERT_HAL_OK(can_device.startCANDevice());
}

HAL_StatusTypeDef rearVCUInfoMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    steering::state.actual_direction.store(static_cast<flare_can::Direction>(msg.data[1]),
                                           std::memory_order_relaxed);

    steering::state.actual_array_contactors_status.store(
        static_cast<flare_can::ArrayContactors>(msg.data[3]), std::memory_order_relaxed);

    uint16_t supp_batt_voltage = (msg.data[4]) | (static_cast<uint16_t>(msg.data[5]) << 8);
    steering::state.supp_batt_voltage_mv.store(supp_batt_voltage, std::memory_order_relaxed);

    steering::state.car_speed.store(msg.data[8], std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef bmsBatteryVoltageMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint16_t main_batt_voltage_mv = static_cast<uint16_t>(msg.data[0] << 8) | (msg.data[1]);
    steering::state.main_batt_voltage_cv.store(main_batt_voltage_mv, std::memory_order_relaxed);

    return HAL_OK;
}

HAL_StatusTypeDef bmsBatteryTempMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    // dc here is like decicelcius
    uint16_t highest_temp_cell_dc = static_cast<uint16_t>(msg.data[0] << 8) | (msg.data[1]);
    steering::state.high_temp_dc.store(highest_temp_cell_dc, std::memory_order_relaxed);

    return HAL_OK;
}

}  // namespace steering
