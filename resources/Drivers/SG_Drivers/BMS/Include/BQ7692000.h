#pragma once

#include <cstdint>

#include <vector>

extern "C"
{
#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_i2c.h"
#endif
}  // Extern "C"

#define TWO_BYTES 2
#define CELL_COUNT 5

// TODO: Split I2C Device into own folders and expand default operations
class I2CDevice
{
   public:
    I2CDevice(I2C_HandleTypeDef *h, uint8_t addr) : i2cHandle(h), deviceAddress(addr){};

    HAL_StatusTypeDef readN(uint8_t reg, uint8_t *buf, size_t len)
    {
        return HAL_I2C_Mem_Read(
            i2cHandle, deviceAddress, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY);
    }
    HAL_StatusTypeDef writeN(uint8_t reg, uint8_t *buf, size_t len)
    {
        return (HAL_I2C_Mem_Write(
            i2cHandle, deviceAddress, reg, I2C_MEMADD_SIZE_8BIT, buf, len, HAL_MAX_DELAY));
    }

   private:
    I2C_HandleTypeDef *i2cHandle;
    const uint8_t deviceAddress;
};

class BQ7692000PW : public I2CDevice
{
   public:
    BQ7692000PW(I2C_HandleTypeDef *h) : I2CDevice(h, i2c_addr)
    {
        dataVC_.reserve(CELL_COUNT * TWO_BYTES);  // 5 cells, 2 bytes per
    };

    HAL_StatusTypeDef getVC(std::vector<uint16_t> &vc_values);
    HAL_StatusTypeDef getCC(uint16_t *data);

   private:
    static constexpr uint8_t i2c_addr = (0x08 << 1);
    enum class registers : uint8_t
    {
        status = 0x00,
        vc1_hi = 0x0C,
        cc_hi = 0x32
    };

    static constexpr uint8_t STATUS_CC_READY_MASK = 0b1000'0000;
    static constexpr uint8_t STATUS_ERR_MASK = 0b0010'1111;

    uint8_t status_ = 0;
    uint8_t dataCC_[TWO_BYTES] = {};
    std::vector<uint8_t> dataVC_;

    HAL_StatusTypeDef checkStatus();
    HAL_StatusTypeDef checkVC();
    HAL_StatusTypeDef checkCC();
};