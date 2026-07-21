#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

#define MPPT1_BASE_ADDR 0x600
#define MPPT2_BASE_ADDR 0x610
#define MPPT3_BASE_ADDR 0x620

constexpr sg::CANFrame mitsuba_status_request = {.can_id = 0x08F89540,
                                                 .id_type = sg::CANFrameIDType::EXTENDED,
                                                 .rtr_mode = sg::CANFrameRTRMode::DATA,
                                                 .len = sg::CANFrameLen::BYTES_1,
                                                 .data = {0b00000111}};

void initCan();

HAL_StatusTypeDef rearVCUInfoMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef rearVCUSuppBattMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsBatteryVoltageMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef bmsBatteryTempMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef telemKillStatusMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef speedMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame0Callback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame1Callback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef mitsubaFrame2Callback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef frontVCUThrottleMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef MPPT1OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT2OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT3OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef bmsFaultMessageCallback(const sg::CANFrame& msg, void* ctx);

inline sg::CANDevice can_device(&hfdcan1,
                                {{0x020, {&rearVCUInfoMessageCallback}},  // std id callbacks
                                 {0x021, {&rearVCUSuppBattMessageCallback}},
                                 {0x041, {&bmsBatteryVoltageMessageCallback}},
                                 {0x042, {&bmsBatteryTempMessageCallback}},
                                 {0x010, {&telemKillStatusMessageCallback}},
                                 {0x0A0, {&speedMessageCallback}},
                                 {0x040, {&bmsFaultMessageCallback}},
                                 {MPPT1_BASE_ADDR + 1, {&MPPT1OutputMeasurementsCallback}},
                                 {MPPT2_BASE_ADDR + 1, {&MPPT2OutputMeasurementsCallback}},
                                 {MPPT3_BASE_ADDR + 1, {&MPPT3OutputMeasurementsCallback}},
                                 {0x080, {&frontVCUThrottleMessageCallback}}},
                                {{0x08850225, {&mitsubaFrame0Callback}},
                                 {0x08950225, {&mitsubaFrame1Callback}},
                                 {0x08A50225, {&mitsubaFrame2Callback}}});  // ext id callbacks
