
#ifndef I2C_API_H
#define I2C_API_H

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

#endif  // I2C_API_H