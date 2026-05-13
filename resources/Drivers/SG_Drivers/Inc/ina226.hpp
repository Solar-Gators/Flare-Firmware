/*
 * ina226.hpp
 *
 *  Created on: Nov 23, 2025
 *      Author: samrb
 */

#ifndef INC_INA226_HPP_
#define INC_INA226_HPP_

#pragma once

// Adjust include to your STM32 family if needed
extern "C"
{
#include "stm32u5xx_hal.h"

#include "FreeRTOSConfig.h"
#include "app_freertos.h"
}

#include <cstdint>

class INA226
{
   public:
    // 7-bit base addresses (before HAL shift)
    static constexpr uint8_t I2C_ADDR_BASE = 0x40;  // A0/A1 = GND, see datasheet

    // Simple struct for a combined measurement
    struct Measurement
    {
        float shunt_V;    // V across shunt
        float bus_V;      // bus voltage
        float current_A;  // calculated current
        float power_W;    // calculated power
    };

    // Constructor: addr is 7-bit I2C address (0x40..0x4F), we shift <<1 internally
    INA226(I2C_HandleTypeDef *hi2c, uint8_t addr_7bit = I2C_ADDR_BASE);

    // Initialize:
    //  - shunt_res_ohm: value of shunt resistor in ohms
    //  - max_current_A: expected max current (used to pick Current_LSB and calibration)
    //  - config: configuration register value (default 0x4127 = cont. shunt+bus, 1.1ms conv, avg=1)
    HAL_StatusTypeDef init(float shunt_res_ohm, float max_current_A, uint16_t config = 0x4127);

    // Read individual quantities
    HAL_StatusTypeDef readShuntVoltage(float &volts);  // V
    HAL_StatusTypeDef readBusVoltage(float &volts);    // V
    HAL_StatusTypeDef readCurrent(float &amps);        // A
    HAL_StatusTypeDef readPower(float &watts);         // W

    // Convenience: read everything at once
    HAL_StatusTypeDef readMeasurement(Measurement &m);

    // Optionally re-write config or calibration
    HAL_StatusTypeDef writeConfig(uint16_t config);
    HAL_StatusTypeDef writeCalibration(uint16_t cal);

    I2C_HandleTypeDef *hi2c_;
    uint8_t addr_;  // 8-bit address for HAL (7-bit << 1)

    float shunt_res_ohm_ = 0.0f;
    float current_lsb_A_ = 0.0f;  // A/LSB
    float power_lsb_W_ = 0.0f;    // W/LSB (25 * current_lsb)

    // Registers (pointer addresses) – refer to datasheet table.
    enum : uint8_t
    {
        REG_CONFIG = 0x00,
        REG_SHUNT_V = 0x01,
        REG_BUS_V = 0x02,
        REG_POWER = 0x03,
        REG_CURRENT = 0x04,
        REG_CALIB = 0x05,
        REG_MASK_ENABLE = 0x06,
        REG_ALERT_LIMIT = 0x07,
        REG_MANUF_ID = 0xFE,
        REG_DIE_ID = 0xFF
    };

    // Low-level helpers
    HAL_StatusTypeDef writeReg16(uint8_t reg, uint16_t value);
    HAL_StatusTypeDef readReg16(uint8_t reg, uint16_t &value);

    // Calibration calculation
    uint16_t computeCalibration(float shunt_res_ohm, float max_current_A);
};

#endif /* INC_INA226_HPP_ */