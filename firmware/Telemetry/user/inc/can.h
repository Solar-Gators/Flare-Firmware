#pragma once

#include "CanDriver.hpp"
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;

inline sg::CANDevice can_device(&hfdcan1);

void can_init();

void steeringRequestsCallback(sg::CANFrame frame);