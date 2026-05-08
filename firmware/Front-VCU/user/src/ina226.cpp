/*
 * ina226.cpp
 *
 *  Created on: Nov 23, 2025
 *      Author: samrb
 */

#include "ina226.hpp"

#include <cmath>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
INA226::INA226(I2C_HandleTypeDef *hi2c, uint8_t addr_7bit)
    : hi2c_(hi2c), addr_(static_cast<uint8_t>(addr_7bit << 1))  // HAL expects 8-bit addr
{
}

// -----------------------------------------------------------------------------
// Low-level I2C 16-bit register R/W (INA226 uses big-endian MSB:LSB)
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::writeReg16(uint8_t reg, uint16_t value)
{
    uint8_t buf[2];
    buf[0] = static_cast<uint8_t>((value >> 8) & 0xFF);  // MSB
    buf[1] = static_cast<uint8_t>(value & 0xFF);         // LSB

    return HAL_I2C_Mem_Write(hi2c_, addr_, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef INA226::readReg16(uint8_t reg, uint16_t &value)
{
    uint8_t buf[2] = {0, 0};

    HAL_StatusTypeDef st =
        HAL_I2C_Mem_Read(hi2c_, addr_, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
    if (st != HAL_OK)
    {
        return st;
    }

    value = (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
    return HAL_OK;
}

// -----------------------------------------------------------------------------
// Calibration computation
// Equation (datasheet):
//   CAL = 0.00512 / (Current_LSB * RSHUNT)
//   Current_LSB ~ I_max / 32768 (2^15)  :contentReference[oaicite:5]{index=5}
// -----------------------------------------------------------------------------
uint16_t INA226::computeCalibration(float shunt_res_ohm, float max_current_A)
{
    if (shunt_res_ohm <= 0.0f || max_current_A <= 0.0f)
    {
        current_lsb_A_ = 0.0f;
        power_lsb_W_ = 0.0f;
        return 0;
    }

    // Pick Current_LSB as Imax / 2^15
    current_lsb_A_ = max_current_A / 32768.0f;  // A per LSB

    // CAL register
    const float cal_f = 0.00512f / (current_lsb_A_ * shunt_res_ohm);
    uint16_t cal_reg = static_cast<uint16_t>(std::round(cal_f));

    // Power LSB is 25 * Current_LSB (datasheet). :contentReference[oaicite:6]{index=6}
    power_lsb_W_ = 25.0f * current_lsb_A_;

    return cal_reg;
}

// -----------------------------------------------------------------------------
// Public: init
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::init(float shunt_res_ohm, float max_current_A, uint16_t config)
{
    shunt_res_ohm_ = shunt_res_ohm;

    // 1) Write configuration register (operating mode, averages, conv times)
    // Default 0x4127 = continuous shunt + bus, 1.1ms conv, 1 sample avg.
    HAL_StatusTypeDef st = writeConfig(config);
    if (st != HAL_OK)
    {
        return st;
    }

    // 2) Compute and write calibration based on shunt and max current
    uint16_t cal = computeCalibration(shunt_res_ohm, max_current_A);
    st = writeCalibration(cal);
    if (st != HAL_OK)
    {
        return st;
    }

    // Short delay to let things settle
    osDelay(2);

    return HAL_OK;
}

// -----------------------------------------------------------------------------
// Config / Calibration setters
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::writeConfig(uint16_t config)
{
    return writeReg16(REG_CONFIG, config);
}

HAL_StatusTypeDef INA226::writeCalibration(uint16_t cal)
{
    return writeReg16(REG_CALIB, cal);
}

// -----------------------------------------------------------------------------
// Read shunt voltage [V]
// Shunt Voltage register: signed 16-bit, LSB = 2.5 µV. :contentReference[oaicite:8]{index=8}
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::readShuntVoltage(float &volts)
{
    uint16_t raw_u16 = 0;
    HAL_StatusTypeDef st = readReg16(REG_SHUNT_V, raw_u16);
    if (st != HAL_OK)
        return st;

    int16_t raw = static_cast<int16_t>(raw_u16);
    volts = static_cast<float>(raw) * 2.5e-6f;  // 2.5 µV per LSB
    return HAL_OK;
}

// -----------------------------------------------------------------------------
// Read bus voltage [V]
// Bus Voltage register: unsigned, bits [15:3] used, LSB = 1.25 mV. :contentReference[oaicite:9]{index=9}
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::readBusVoltage(float &volts)
{
    uint16_t raw = 0;
    HAL_StatusTypeDef st = readReg16(REG_BUS_V, raw);
    if (st != HAL_OK)
        return st;

    // Shift right 3; LSB = 1.25 mV
    uint16_t v_raw = raw >> 3;
    volts = static_cast<float>(v_raw) * 1.25e-3f;
    return HAL_OK;
}

// -----------------------------------------------------------------------------
// Read current [A]
// Current register: signed 16-bit in units of Current_LSB. :contentReference[oaicite:10]{index=10}
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::readCurrent(float &amps)
{
    if (current_lsb_A_ <= 0.0f)
    {
        amps = 0.0f;
        return HAL_ERROR;
    }

    uint16_t raw_u16 = 0;
    HAL_StatusTypeDef st = readReg16(REG_CURRENT, raw_u16);
    if (st != HAL_OK)
        return st;

    int16_t raw = static_cast<int16_t>(raw_u16);
    amps = static_cast<float>(raw) * current_lsb_A_;
    return HAL_OK;
}

// -----------------------------------------------------------------------------
// Read power [W]
// Power register: unsigned 16-bit in units of 25 * Current_LSB. :contentReference[oaicite:11]{index=11}
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::readPower(float &watts)
{
    if (power_lsb_W_ <= 0.0f)
    {
        watts = 0.0f;
        return HAL_ERROR;
    }

    uint16_t raw = 0;
    HAL_StatusTypeDef st = readReg16(REG_POWER, raw);
    if (st != HAL_OK)
        return st;

    watts = static_cast<float>(raw) * power_lsb_W_;
    return HAL_OK;
}

// -----------------------------------------------------------------------------
// Convenience: read all measurements
// -----------------------------------------------------------------------------
HAL_StatusTypeDef INA226::readMeasurement(Measurement &m)
{
    HAL_StatusTypeDef st;

    st = readShuntVoltage(m.shunt_V);
    if (st != HAL_OK)
        return st;

    st = readBusVoltage(m.bus_V);
    if (st != HAL_OK)
        return st;

    st = readCurrent(m.current_A);
    if (st != HAL_OK)
        return st;

    st = readPower(m.power_W);
    if (st != HAL_OK)
        return st;

    return HAL_OK;
}