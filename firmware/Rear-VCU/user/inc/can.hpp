#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

inline sg::CANDevice can_device(
    &hfdcan1);  // inline on variable declared in header file like this allows
                // us to avoid odr when including in multiple cpp files, all references to it
                // will correctly refer to a single instance of the variable.

constexpr sg::CANFrame mitsuba_frame0_request = {
    .can_id = 0x08F89540,
    .id_type = sg::CANFrameIDType::EXTENDED,
    .rtr_mode = sg::CANFrameRTRMode::DATA,
    .len = sg::CANFrameLen::BYTES_1,
    .data = {0b00000001}  // bit 1 set = request frame 0
};

HAL_StatusTypeDef throttleMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef driverMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx);

void can_init();