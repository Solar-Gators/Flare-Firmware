#pragma once

#include "i2c_api.h"

#include <vector>

#define ONE_BYTE 1
#define TWO_BYTES 2
#define CELL_COUNT 5

#define CC_CONFIG_MASK 0x19
#define STATUS_CC_READY_MASK 0b1000'0000
#define CC_EN_MASK 0b0100'0000
#define ADC_EN_MASK 0b0001'0000
#define STATUS_ERR_MASK 0b0010'1111
#define ACTIVE_BAL_MASK 0b0001'1111

#define ADC_GAIN_MAX_MASK 0x1F
#define ADC_GAIN_REG1_MASK 0b0001'1000
#define ADC_GAIN_REG2_MASK 0b0000'0111

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
     * @brief Retrieves current battery calculation
     *
     * @param [out] data
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef getBAT(uint16_t *data);

    /**
     * @brief Retrieves ADC Offset from read only register
     *
     * @param [out] data
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef getADCOffset(uint8_t *data);

    /**
     * @brief Sets ADC gain
     *
     * @param [in] data from 0x00 to 0x1F
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef setADCGain(uint8_t *data);

    /**
     * @brief Get the Active Balancing register
     *
     * @param [out] activeBal
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef getActiveBalancing(uint8_t *activeBal);

    /**
     * @brief Set the Active Balancing register
     *
     * @param [in] activeBal
     * @return HAL_StatusTypeDef
     */
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

    /**
     * @brief Private CC start
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef initCC();

    /**
     * @brief Private enable CC reading reg
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef enableCCReading();

    /**
     * @brief Private ADC enable
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef enableADC();

    /**
     * @brief Private read status register
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef checkStatus();

    /**
     * @brief Private read of all 10 VC registers
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef checkVC();

    /**
     * @brief Private read CC register
     *
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef checkCC();
};