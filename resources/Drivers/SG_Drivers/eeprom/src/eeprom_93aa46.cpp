#include "eeprom_93aa46.hpp"

#include <array>

#define HAL_SPI_TRANSMIT_TIMEOUT 100

// read can only do one byte at a time on on this device
EepromStatus Eeprom93AA46::read(uint32_t addr, void* buf, size_t len)
{
    // write one byte at a time
    auto* byte_arr = static_cast<const uint8_t*>(buf);
    for (int i = 0; i < len; i++)
    {
        // begin write sequence for bytes
    }
    return EepromStatus::kOk;
}

// write can only do one byte at a time on on this device
EepromStatus Eeprom93AA46::write(uint32_t addr, const void* buf, size_t len)
{
    return EepromStatus::kOk;
}

EepromStatus Eeprom93AA46::sendRead(uint32_t addr, uint8_t& out)
{
    return EepromStatus::kOk;
}

EepromStatus Eeprom93AA46::sendWrite(uint32_t addr, const uint8_t& byte)
{
    return EepromStatus::kOk;
}

EepromStatus Eeprom93AA46::sendEWEN()
{
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi_, &kEwen[0], kEwLen, 100);
    return hal_to_eeprom_status(status);
}

EepromStatus Eeprom93AA46::sendEWDS()
{
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi_, &kEwds[0], kEwLen, HAL_SPI_TRANSMIT_TIMEOUT);
    return hal_to_eeprom_status(status);
}

EepromStatus Eeprom93AA46::sendInstruction(uint8_t* instr, size_t len)
{
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi_, instr, len, HAL_SPI_TRANSMIT_TIMEOUT);
    return hal_to_eeprom_status(status);
}
