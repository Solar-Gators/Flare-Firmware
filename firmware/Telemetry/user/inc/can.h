#pragma once

#include "CanDriver.hpp"
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

void canInit();
HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef rearVCUStatusCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef bmsFaultsMessageCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef radioTXCallback(const sg::CANFrame& frame, void* ctx);

inline sg::CANDevice can_device(&hfdcan1,
                                {
                                    {0x064, {&steeringRequestsCallback, &radioTXCallback}},
                                    {0x020, {&rearVCUStatusCallback, &radioTXCallback}},
                                    {0x040, {&bmsFaultsMessageCallback, &radioTXCallback}},
                                    {0x041, {&radioTXCallback}},  // batt voltage
                                    {0x600, {&radioTXCallback}},  // mppts
                                    {0x601, {&radioTXCallback}},
                                    {0x610, {&radioTXCallback}},
                                    {0x611, {&radioTXCallback}},
                                    {0x620, {&radioTXCallback}},
                                    {0x621, {&radioTXCallback}},
                                },
                                {});
