#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

inline sg::CANDevice can_device(&hfdcan1);

void can_init();

HAL_StatusTypeDef rearVCUInfoMessageCallback(const sg::CANFrame& msg, void* ctx);
