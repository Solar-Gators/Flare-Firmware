#include "../Include/BQ7692000.h"

#include <cstdint>

#define TRY(x)                \
    do                        \
    {                         \
        if ((x) != HAL_OK)    \
            return HAL_ERROR; \
    } while (0)

HAL_StatusTypeDef BQ7692000PW::init()
{
    TRY(initCC());
    TRY(enableCCReading());
    TRY(enableADC());

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getCC(uint16_t *data)
{
    TRY(checkCC());

    *data = (static_cast<uint16_t>(dataCC_[0]) << 8) | dataCC_[1];
    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getVC(std::array<uint16_t, CELL_COUNT> &vc_values)
{
    TRY(checkVC());

    for (size_t cell = 0; cell < CELL_COUNT; ++cell)
    {
        const size_t byte_idx = cell * TWO_BYTES;
        vc_values.at(cell) =
            (static_cast<uint16_t>(dataVC_[byte_idx]) << 8) | dataVC_[byte_idx + 1];
    }

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getBAT(uint16_t *data)
{
    TRY(readN(static_cast<uint8_t>(registers::BAT_HI), dataBAT_, TWO_BYTES));

    *data = (dataBAT_[0] << 8) | dataBAT_[1];
    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getDieTemp(uint16_t *data)
{
    TRY(readN(static_cast<uint8_t>(registers::TS1_HI), dataTemp_, TWO_BYTES));

    *data = (static_cast<uint16_t>((dataTemp_[0] << 8) | dataTemp_[1]));
    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getActiveBalancing(uint8_t *activeBal)
{
    TRY(readN(static_cast<uint8_t>(registers::CELL_BAL1), activeBal, ONE_BYTE));

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::setActiveBalancing(uint8_t *activeBal)
{
    // Ensure upper three bits are not written
    uint8_t write = *activeBal & ACTIVE_BAL_MASK;
    TRY(writeN(static_cast<uint8_t>(registers::CELL_BAL1), &write, ONE_BYTE));

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::setADCGain(uint8_t *data)
{
    uint8_t formatted_data = *data & ADC_GAIN_MAX_MASK;
    uint8_t reg1 = (formatted_data & ADC_GAIN_REG1_MASK) >> 1;
    uint8_t reg2 = (formatted_data & ADC_GAIN_REG2_MASK) << 5;

    TRY(writeN(static_cast<uint8_t>(registers::ADCGAIN1), &reg1, ONE_BYTE));
    TRY(writeN(static_cast<uint8_t>(registers::ADCGAIN2), &reg2, ONE_BYTE));

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getADCOffset(uint8_t *data)
{
    TRY(readN(static_cast<uint8_t>(registers::ADCOFFSET), data, ONE_BYTE));

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::checkStatus()
{
    TRY(readN(static_cast<uint8_t>(registers::SYS_STAT), &status_, ONE_BYTE));

    if ((status_ & STATUS_ERR_MASK) != 0)
    {
        // TODO: log/handle error bits
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::initCC()
{
    uint8_t data = CC_CONFIG_MASK;
    TRY(writeN(static_cast<uint8_t>(registers::CC_CFG), &data, ONE_BYTE));

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::enableCCReading()
{
    uint8_t reg{};
    TRY(readN(static_cast<uint8_t>(registers::SYS_CTRL2), &reg, ONE_BYTE));
    reg |= CC_EN_MASK;
    TRY(writeN(static_cast<uint8_t>(registers::SYS_CTRL2), &reg, ONE_BYTE));
    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::enableADC()
{
    uint8_t reg{};
    TRY(readN(static_cast<uint8_t>(registers::SYS_CTRL1), &reg, ONE_BYTE));
    reg |= ADC_EN_MASK;
    TRY(writeN(static_cast<uint8_t>(registers::SYS_CTRL1), &reg, ONE_BYTE));
    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::checkVC()
{
    for (int i = 0; i < CELL_COUNT; i++)
    {
        TRY(readN(static_cast<uint8_t>(registers::VC1_HI) + static_cast<uint8_t>(i * TWO_BYTES),
                  dataVC_.data() + (i * TWO_BYTES),
                  TWO_BYTES));
    }

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::checkCC()
{
    TRY(checkStatus());

    if (!(status_ & STATUS_CC_READY_MASK))
    {
        return HAL_BUSY;  // Maybe HAL_ERROR?
    }

    TRY(readN(static_cast<uint8_t>(registers::CC_HI), dataCC_, TWO_BYTES));

    // Clear CC Ready latch
    uint8_t clearCC = STATUS_CC_READY_MASK;
    return writeN(static_cast<uint8_t>(registers::SYS_STAT), &clearCC, ONE_BYTE);
}