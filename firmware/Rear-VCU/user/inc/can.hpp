#pragma once
#include "CanDriver.hpp"

extern FDCAN_HandleTypeDef hfdcan1;

namespace rearvcu
{

HAL_StatusTypeDef throttleMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef driverMessageCallback(const sg::CANFrame& msg, void* ctx);
HAL_StatusTypeDef killCarMessageCallback(const sg::CANFrame& msg, void* ctx);

inline sg::CANDevice can_device(&hfdcan1,
                                {
                                    {0x080, {&throttleMessageCallback}},
                                    {0x064, {&driverMessageCallback}},
                                    {0x010, {&killCarMessageCallback}},
                                },
                                {});  // inline on variable declared in header file like this allows
// us to avoid odr when including in multiple cpp files, all references to it
// will correctly refer to this single instance of the variable.

void can_init();

}  // namespace rearvcu
