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

#define ONE_BYTE 1
#define TWO_BYTES 2
#define CELL_COUNT 5

#define CC_CONFIG_MASK 0x19
#define STATUS_CC_READY_MASK 0b1000'0000
#define CC_EN_MASK 0b0100'0000
#define ADC_EN_MASK 0b0001'0000
#define STATUS_ERR_MASK 0b0010'1111
#define ACTIVE_BAL_MASK 0b0001'1111

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

    /**
     * @brief Enables Coulomb Counting and ADC reading on VC pins
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef init();
    // TODO: Possibly make switch into SHIP mode for power conservation ?

    /**
     * @brief Reads the 5 Cell voltages of the BQ7692x
     *
     * @param vc_values list of cell voltages
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef getVC(std::vector<uint16_t> &vc_values);

    /**
     * @brief Reads Coulomb count register after ensuring CC_Ready is true
     *
     * @param [out] data Atomic value of CC
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef getCC(uint16_t *data);

    /**
     * @brief
     *
     * @param data
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef getBAT(uint16_t *data);

    HAL_StatusTypeDef getADCOffset(uint8_t *data);
    HAL_StatusTypeDef setADCGain(uint8_t *data);

    HAL_StatusTypeDef getActiveBalancing(uint8_t *activeBal);
    HAL_StatusTypeDef setActiveBalancing(uint8_t *activeBal);

   private:
    /* VARIABLES */

    // TODO: Consider cleaning up this section
    static constexpr uint8_t i2c_addr = (0x08 << 1);

    enum class registers : uint8_t
    {
        SYS_STAT = 0x00,
        CELL_BAL1 = 0x01,
        SYS_CTRL1 = 0x04,
        SYS_CTRL2 = 0x05,
        CC_CFG = 0x0B,
        VC1_HI = 0x0C,
        BAT_HI = 0x2A,
        CC_HI = 0x32,
        ADCGAIN1 = 0x50,
        ADCOFFSET = 0x51,
        ADCGAIN2 = 0x59
    };

    uint8_t status_ = 0;
    uint8_t dataCC_[TWO_BYTES] = {};
    uint8_t dataBAT_[TWO_BYTES] = {};
    std::vector<uint8_t> dataVC_;

    /* FUNCTIONS */

    HAL_StatusTypeDef initCC();

    HAL_StatusTypeDef enableCCReading();
    HAL_StatusTypeDef enableADC();

    HAL_StatusTypeDef checkStatus();
    HAL_StatusTypeDef checkVC();
    HAL_StatusTypeDef checkCC();
};