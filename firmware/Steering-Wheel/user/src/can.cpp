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
    steering::state.array_contactors_status.store(
        static_cast<flare_can::ArrayContactors>(msg.data[3]));

    uint16_t supp_batt_voltage = (msg.data[4]) | (static_cast<uint16_t>(msg.data[5]) << 8);
    steering::state.supp_batt_voltage.store(supp_batt_voltage);

    steering::state.car_speed.store(msg.data[8]);

    return HAL_OK;
}

HAL_StatusTypeDef bmsBatteryVoltageMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    return HAL_OK;
}

HAL_StatusTypeDef bmsBatteryTempMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    return HAL_OK;
}