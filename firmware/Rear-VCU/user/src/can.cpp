#include "can.hpp"

HAL_StatusTypeDef throttleMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    uint8_t throttle_low = msg.data[0];
    uint8_t throttle_high = msg.data[1];
    uint16_t throttle_value =
        (static_cast<uint16_t>(throttle_high) << 8) | static_cast<uint16_t>(throttle_low);
    vcu_state.throttle.store(throttle_value);

    ++vcu_state.can_messages_received;

    return HAL_OK;
}
HAL_StatusTypeDef driverMessageCallback(const sg::CANFrame& msg, void* ctx)
{
    vcu_state.direction_requested.store(static_cast<Direction>(msg.data[0] & 0b00000001));
    vcu_state.array_contactors_requested_closed.store(static_cast<bool>(msg.data[2] & 0b00000010));
    vcu_state.regen.store(msg.data[5]);

    ++vcu_state.can_messages_received;

    return HAL_OK;
}
HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx)
{
    // need to get rpm here and calculate miles per hour

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
}
