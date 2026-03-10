#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

constexpr sg::CANFrame mitsuba_frame0_request = {
    .can_id = 0x08F89540,
    .id_type = sg::CANFrameIDType::EXTENDED,
    .rtr_mode = sg::CANFrameRTRMode::DATA,
    .len = sg::CANFrameLen::BYTES_1,
    .data = {0b00000001}  // bit 1 set = request frame 0
};

inline sg::CANDevice can_device(&hfdcan1);

void initCan();

HAL_StatusTypeDef rearVCUInfoMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef rearVCUSuppBattMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsBatteryVoltageMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsBatteryTempMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef telemKillStatusMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef speedMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx);
