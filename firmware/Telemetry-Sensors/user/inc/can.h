#ifndef FLAREFIRMWARE_CAN_H
#define FLAREFIRMWARE_CAN_H

#pragma once

#include "CanDriver.hpp"
#include "main.h"

#define MPPT1_BASE_ADDR 0x600
#define MPPT2_BASE_ADDR 0x610
#define MPPT3_BASE_ADDR 0x620

extern CAN_HandleTypeDef hcan1;

void canInit();

HAL_StatusTypeDef GenericFrameForwardingCallback(const sg::CANFrame& frame, void* ctx);

inline sg::CANDevice can_device(&hcan1,
                                {
                                    {MPPT1_BASE_ADDR, {&GenericFrameForwardingCallback}},
                                    {MPPT2_BASE_ADDR, {&GenericFrameForwardingCallback}},
                                    {MPPT3_BASE_ADDR, {&GenericFrameForwardingCallback}},
                                    {MPPT1_BASE_ADDR + 1, {&GenericFrameForwardingCallback}},
                                    {MPPT2_BASE_ADDR + 1, {&GenericFrameForwardingCallback}},
                                    {MPPT3_BASE_ADDR + 1, {&GenericFrameForwardingCallback}},
                                },
                                {});

#endif  //FLAREFIRMWARE_CAN_H
