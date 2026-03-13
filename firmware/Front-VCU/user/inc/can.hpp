#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

namespace frontvcu
{

inline sg::CANDevice can_device(
    &hfdcan1);  // inline on variable declared in header file like this allows
// us to avoid odr when including in multiple cpp files, all references to it
// will correctly refer to this single instance of the variable.

HAL_StatusTypeDef steering_wheel_msg_cb(const sg::CANFrame& msg, void* ctx);

void can_init();
}  // namespace frontvcu
