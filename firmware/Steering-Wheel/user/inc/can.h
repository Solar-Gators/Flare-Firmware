#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

constexpr sg::CANFrame mitsuba_frame0_request = {
    .can_id = 0x08F89540,
    .id_type = sg::CANFrameIDType::EXTENDED,
    .rtr_mode = sg::CANFrameRTRMode::DATA,
    .len = sg::CANFrameLen::BYTES_1,
    .data = {0b00000101}  // bit 1 set = request frame 0
};

void initCan();

HAL_StatusTypeDef rearVCUInfoMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef rearVCUSuppBattMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsBatteryVoltageMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsBatteryTempMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef telemKillStatusMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef speedMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef frontVCUThrottleMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsFaultMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame2Callback(const sg::CANFrame& msg, void* ctx);

inline sg::CANDevice can_device(&hfdcan1,
                                {{0x020, {&rearVCUInfoMessageCallback}},  // std id callbacks
                                 {0x021, {&rearVCUSuppBattMessageCallback}},
                                 {0x041, {&bmsBatteryVoltageMessageCallback}},
                                 {0x042, {&bmsBatteryTempMessageCallback}},
                                 {0x010, {&telemKillStatusMessageCallback}},
                                 {0x0A0, {&speedMessageCallback}},
                                 {0x040, {&bmsFaultMessageCallback}},
                                 {0x08A50225, {&mitsubaFrame2Callback}},
                                 {0x080, {&frontVCUThrottleMessageCallback}}},
                                {{0x08850225, {&mitsubaFrame0Callback}}});  // ext id callbacks
