#pragma once

#include "CanDriver.hpp"
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

inline sg::CANDevice can_device(&hfdcan1);

void canInit();
HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef rearVCUStatusCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef bmsFaultsMessageCallback(const sg::CANFrame& frame, void* ctx);
