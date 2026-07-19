#pragma once

#include "CanDriver.hpp"
#include "main.h"

#define MPPT1_BASE_ADDR 0x600
#define MPPT2_BASE_ADDR 0x610
#define MPPT3_BASE_ADDR 0x620

extern FDCAN_HandleTypeDef hfdcan1;

void canInit();
HAL_StatusTypeDef frontVCUThrottleMessageCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef rearVCUStatusCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef bmsFaultsMessageCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef radioTXCallback(const sg::CANFrame& frame, void* ctx);

// mppt can callbacks
HAL_StatusTypeDef MPPT1InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT2InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT3InputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);

HAL_StatusTypeDef MPPT1OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT2OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);
HAL_StatusTypeDef MPPT3OutputMeasurementsCallback(const sg::CANFrame& frame, void* ctx);

inline sg::CANDevice can_device(
    &hfdcan1,
    {
        // -- frames with dedicated handlers (also forwarded) --
        {0x080, {&frontVCUThrottleMessageCallback, &radioTXCallback}},
        {0x064, {&steeringRequestsCallback, &radioTXCallback}},
        {0x040, {&bmsFaultsMessageCallback, &radioTXCallback}},

        // -- MPPT sensor data (packed + forwarded via forwardMpptState) --
        {MPPT1_BASE_ADDR, {&MPPT1InputMeasurementsCallback}},
        {MPPT2_BASE_ADDR, {&MPPT2InputMeasurementsCallback}},
        {MPPT3_BASE_ADDR, {&MPPT3InputMeasurementsCallback}},
        {MPPT1_BASE_ADDR + 1, {&MPPT1OutputMeasurementsCallback}},
        {MPPT2_BASE_ADDR + 1, {&MPPT2OutputMeasurementsCallback}},
        {MPPT3_BASE_ADDR + 1, {&MPPT3OutputMeasurementsCallback}},
    },
    {});
