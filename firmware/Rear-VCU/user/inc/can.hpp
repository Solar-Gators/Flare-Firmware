#pragma once
#include "CanDriver.hpp"
#include "rearvcu_state.h"

extern FDCAN_HandleTypeDef hfdcan1;

inline sg::CANDevice can_device(
    &hfdcan1);  // inline on variable declared in header file like this allows
                // us to avoid odr when including in multiple cpp files, all references to it
                // will correctly refer to a single instance of the variable.

HAL_StatusTypeDef throttleMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef driverMessageCallback(const sg::CANFrame& msg, void* ctx);

void can_init();