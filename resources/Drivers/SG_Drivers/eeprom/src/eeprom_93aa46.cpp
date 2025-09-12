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
    EepromStatus status;
    const uint8_t* ptr = static_cast<const uint8_t*>(buf);

    status = sendEWEN();
    if (status != EepromStatus::kOk)
    {
        return status;
    }

    for (int i = 0; i < len; i++)
    {
        sendWrite(addr + i, ptr[i]);
    }

    status = sendEWDS();
    if (status != EepromStatus::kOk)
    {
        return status;
    }

    return status;
}

EepromStatus Eeprom93AA46::sendRead(uint32_t addr, uint8_t& out)
{
    return EepromStatus::kOk;
}

EepromStatus Eeprom93AA46::sendWrite(uint32_t addr, const uint8_t& byte)
{
    uint32_t instruction = 0;

    instruction |= kWrMask;
    addr &= kAddrMask;
    instruction |= (addr << 8);
    instruction |= byte;

    return sendRWInstruction(instruction);
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

EepromStatus Eeprom93AA46::sendRWInstruction(uint32_t instr)
{
    std::array<uint8_t, kRWLen> bytes_arr;

    bytes_arr[0] = static_cast<uint8_t>(instr >> 16);
    bytes_arr[1] = static_cast<uint8_t>(instr >> 8);
    bytes_arr[2] = static_cast<uint8_t>(instr >> 0);

    HAL_StatusTypeDef status =
        HAL_SPI_Transmit(hspi_, &bytes_arr[0], bytes_arr.size(), HAL_SPI_TRANSMIT_TIMEOUT);
    return hal_to_eeprom_status(status);
}
