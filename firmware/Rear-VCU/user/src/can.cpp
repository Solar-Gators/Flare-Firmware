#include "can.hpp"

#include <cmath>

#include "main.h"
#include "rearvcu_state.h"

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

namespace rearvcu
{

HAL_StatusTypeDef throttleMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint8_t throttle_low = msg.data[0];
    uint8_t throttle_high = msg.data[1];
    uint16_t throttle_value =
        (static_cast<uint16_t>(throttle_high) << 8) | static_cast<uint16_t>(throttle_low);
    state.throttle_requested.store(throttle_value);

    return HAL_OK;
}

HAL_StatusTypeDef driverMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    state.direction_requested.store(static_cast<flare_can::Direction>(msg.data[1]));
    state.array_contactors_requested_closed.store(static_cast<bool>(msg.data[2]));
    state.regen_requested.store(msg.data[5]);
    state.mc_power_mode_requested.store(static_cast<flare_can::MCPowerMode>(msg.data[6]));

    return HAL_OK;
}

void can_init()
{
    // throttle
    ASSERT_TRUE(can_device.addCallbackId(
        0x080, sg::CANFrameIDType::STANDARD, &throttleMessageCallback, nullptr));

    // all the user inputs from steering wheel
    ASSERT_TRUE(can_device.addCallbackId(
        0x064, sg::CANFrameIDType::STANDARD, &driverMessageCallback, nullptr));

    // start
    ASSERT_HAL_OK(can_device.startCANDevice());
}

}  // namespace rearvcu
