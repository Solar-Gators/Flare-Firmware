#include "eeprom_93aa46.hpp"

#include <array>

#define HAL_SPI_TRANSMIT_TIMEOUT 100

// read can only do one byte at a time on on this device
EepromStatus Eeprom93AA46::read(uint32_t addr, uint8_t* buf, size_t len)
{
    EepromStatus status;

    for (size_t i = 0; i < len; i++)
    {
        status = sendRead(addr + i, buf[i]);
        if (status != EepromStatus::kOk)
        {
            return status;
        }
    }

    return status;
}

// write can only do one byte at a time on on this device
EepromStatus Eeprom93AA46::write(uint32_t addr, const uint8_t* buf, size_t len)
{
    EepromStatus status;

    status = sendEWEN();
    if (status != EepromStatus::kOk)
    {
        return status;
    }

    for (size_t i = 0; i < len; i++)
    {
        status = sendWrite(addr + i, buf[i]);
        if (status != EepromStatus::kOk)
        {
            return status;
        }
    }

    status = sendEWDS();

    return status;
}

EepromStatus Eeprom93AA46::sendRead(uint32_t addr, uint8_t& out)
{
    uint32_t instruction = 0;

    instruction |= kRdMask;
    addr &= kAddrMask;
    instruction |= addr;

    csHigh();
    EepromStatus status = sendReadFromBitInstruction(instruction);
    if (status != EepromStatus::kOk)
    {
        csLow();
        return status;
    }

    // recieve byte
    HAL_StatusTypeDef status_hal =
        HAL_SPI_Receive(hspi_, &out, programGranularity(), HAL_SPI_TRANSMIT_TIMEOUT);
    csLow();
    return hal_to_eeprom_status(status_hal);
}

// need write enable sent before this, this does not send the write enable, so it should not be called by the user
EepromStatus Eeprom93AA46::sendWrite(uint32_t addr, const uint8_t& byte)
{
    uint32_t instruction = 0;

    instruction |= kWrMask;
    addr &= kAddrMask;
    instruction |= (addr << 8);
    instruction |= byte;

    csHigh();
    EepromStatus status = sendWriteFromBitInstruction(instruction);
    csLow();
    return status;
}

EepromStatus Eeprom93AA46::sendEWEN()
{
    csHigh();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi_, &kEwen[0], kEwLen, 100);
    csLow();
    return hal_to_eeprom_status(status);
}

EepromStatus Eeprom93AA46::sendEWDS()
{
    csHigh();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi_, &kEwds[0], kEwLen, HAL_SPI_TRANSMIT_TIMEOUT);
    csLow();
    return hal_to_eeprom_status(status);
}

EepromStatus Eeprom93AA46::sendWriteFromBitInstruction(uint32_t instr)
{
    std::array<uint8_t, kWLen> bytes_arr;

    bytes_arr[0] = static_cast<uint8_t>(instr >> 16);
    bytes_arr[1] = static_cast<uint8_t>(instr >> 8);
    bytes_arr[2] = static_cast<uint8_t>(instr >> 0);

    HAL_StatusTypeDef status =
        HAL_SPI_Transmit(hspi_, &bytes_arr[0], bytes_arr.size(), HAL_SPI_TRANSMIT_TIMEOUT);
    return hal_to_eeprom_status(status);
}

EepromStatus Eeprom93AA46::sendReadFromBitInstruction(uint32_t instr)
{
    std::array<uint8_t, kRLen> bytes_arr;

    bytes_arr[0] = static_cast<uint8_t>(instr >> 8);
    bytes_arr[1] = static_cast<uint8_t>(instr >> 0);

    HAL_StatusTypeDef status =
        HAL_SPI_Transmit(hspi_, &bytes_arr[0], bytes_arr.size(), HAL_SPI_TRANSMIT_TIMEOUT);
    return hal_to_eeprom_status(status);
}