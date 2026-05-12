#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

namespace frontvcu
{

HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx);

inline sg::CANDevice can_device(&hfdcan1, {{0x064, {steeringRequestsCallback}}}, {});
// inline on variable declared in header file like this allows
// us to avoid odr when including in multiple cpp files, all references to it
// will correctly refer to this single instance of the variable.

void can_init();
}  // namespace frontvcu
