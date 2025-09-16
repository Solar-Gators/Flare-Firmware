#include "eeprom_93aa46.hpp"

#include <array>

namespace sg
{

// read can only do one byte at a time on on this device
Status Eeprom93AA46::read(uint32_t addr, uint8_t* buf, size_t len)
{
    sg::Status status;

    for (size_t i = 0; i < len; i++)
    {
        status = sendRead(addr + i, buf[i]);
        if (status != sg::Status::Ok)
            return status;
    }

    return status;
}

// write can only do one byte at a time on this device
Status Eeprom93AA46::write(uint32_t addr, const uint8_t* buf, size_t len)
{
    sg::Status status;

    status = sendEWEN();
    if (status != sg::Status::Ok)
    {
        return status;
    }

    for (size_t i = 0; i < len; i++)
    {
        status = sendWrite(addr + i, buf[i]);
        if (status != sg::Status::Ok)
        {
            return status;
        }
    }

    status = sendEWDS();

    return status;
}

Status Eeprom93AA46::sendRead(uint32_t addr, uint8_t& out)
{
    uint32_t instruction = 0;

    instruction |= kRdMask;
    addr &= kAddrMask;
    instruction |= addr;

    return sendReadFromBitInstruction(instruction, out);
}

// need write enable sent before this, this does not send the write enable, so it should not be called by the user
Status Eeprom93AA46::sendWrite(uint32_t addr, const uint8_t& byte)
{
    uint32_t instruction = 0;

    instruction |= kWrMask;
    addr &= kAddrMask;
    instruction |= (addr << 8);
    instruction |= byte;

    return sendWriteFromBitInstruction(instruction);
}

Status Eeprom93AA46::sendEWEN()
{
    return spi_.transmit(&kEwen[0], kEwen.size());
}

Status Eeprom93AA46::sendEWDS()
{
    return spi_.transmit(&kEwds[0], kEwds.size());
}

Status Eeprom93AA46::sendWriteFromBitInstruction(uint32_t instr)
{
    std::array<uint8_t, kWLen> bytes_arr;

    bytes_arr[0] = static_cast<uint8_t>(instr >> 16);
    bytes_arr[1] = static_cast<uint8_t>(instr >> 8);
    bytes_arr[2] = static_cast<uint8_t>(instr >> 0);

    return spi_.transmit(&bytes_arr[0], bytes_arr.size());
}

Status Eeprom93AA46::sendReadFromBitInstruction(uint32_t instr, uint8_t& out)
{
    std::array<uint8_t, kRLen> bytes_arr;

    bytes_arr[0] = static_cast<uint8_t>(instr >> 8);
    bytes_arr[1] = static_cast<uint8_t>(instr >> 0);

    return spi_.transmitReceive(&bytes_arr[0], bytes_arr.size(), &out, programGranularity());
}

#ifdef HAL_SPI_MODULE_ENABLED
Eeprom93AA46 makeEeprom(SPI_HandleTypeDef* h, GPIO_TypeDef* p, uint16_t pin)
{
    return Eeprom93AA46(h, p, pin);
}
#endif

}  // namespace sg