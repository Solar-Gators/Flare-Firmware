#include "eeprom_93aa46.hpp"

#include <array>

// read can only do one byte at a time on on this device
int Eeprom93AA46::read(uint32_t addr, void* buf, size_t len)
{
    // write one byte at a time
    auto* byte_arr = static_cast<const uint8_t*>(buf);
    for (int i = 0; i < len; i++)
    {
        // begin write sequence for bytes
    }
    return 0;
}

// write can only do one byte at a time on on this device
int Eeprom93AA46::write(uint32_t addr, const void* buf, size_t len)
{
    return 0;
}

HAL_StatusTypeDef Eeprom93AA46::sendEWEN()
{
    return HAL_StatusTypeDef();
}

HAL_StatusTypeDef Eeprom93AA46::sendEWDS()
{
    return HAL_StatusTypeDef();
}

HAL_StatusTypeDef Eeprom93AA46::sendRead(uint32_t addr, uint8_t& out)
{
    return HAL_StatusTypeDef();
}

HAL_StatusTypeDef Eeprom93AA46::sendWrite(uint32_t addr, const uint8_t& byte)
{
    return HAL_StatusTypeDef();
}

HAL_StatusTypeDef Eeprom93AA46::sendInstruction(const uint32_t& data,
                                                const InstrLen& instruction_length)
{
    std::array<uint8_t, 3> bytes_arr;
    bytes_arr[0] = static_cast<uint8_t>(data >> 16);
    bytes_arr[1] = static_cast<uint8_t>(data >> 8);
    bytes_arr[2] = static_cast<uint8_t>(data >> 0);

    const uint8_t* start = nullptr;
    size_t byte_count;

    if (instruction_length == InstrLen::kEW)
    {
        start = &bytes_arr[1];
        byte_count = 2;
    }
    else if (instruction_length == InstrLen::kRW)
    {
        start = &bytes_arr[0];
        byte_count = 3;
    }
    else
    {
        //return error ?
    }

    HAL_SPI_Transmit(hspi_, start, byte_count, 100);
}
