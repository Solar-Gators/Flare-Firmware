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

HAL_StatusTypeDef MPPT1InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT2InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT3InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);

HAL_StatusTypeDef MPPT1OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT2OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT3OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);

inline sg::CANDevice can_device(&hcan1,
                                {
                                    {MPPT1_BASE_ADDR, {&MPPT1InputMeasurementsCallback}},
                                    {MPPT2_BASE_ADDR, {&MPPT2InputMeasurementsCallback}},
                                    {MPPT3_BASE_ADDR, {&MPPT3InputMeasurementsCallback}},
                                    {MPPT1_BASE_ADDR + 1, {&MPPT1OutputMeasurementsCallback}},
                                    {MPPT2_BASE_ADDR + 1, {&MPPT2OutputMeasurementsCallback}},
                                    {MPPT3_BASE_ADDR + 1, {&MPPT3OutputMeasurementsCallback}},
                                },
                                {});

#endif  //FLAREFIRMWARE_CAN_H
