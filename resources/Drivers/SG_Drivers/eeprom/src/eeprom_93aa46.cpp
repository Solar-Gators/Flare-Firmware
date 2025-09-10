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

HAL_StatusTypeDef Eeprom93AA46::sendEWEN() {}

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

HAL_StatusTypeDef Eeprom93AA46::sendBits(const uint32_t& data, size_t count)
{
    if (count > 32)
    {
        return HAL_StatusTypeDef();
    }

    std::array<uint8_t, 4> bytes_arr;

    bytes_arr[0] = static_cast<uint8_t>(data >> 24);
    bytes_arr[1] = static_cast<uint8_t>(data >> 16);
    bytes_arr[2] = static_cast<uint8_t>(data >> 8);
    bytes_arr[3] = static_cast<uint8_t>(data >> 0);

    uint8_t* start = &bytes_arr[0];
    size_t byte_count = 4;

    if (count <= 16)
    {
        start += 2;
        byte_count = 2;
    }

    HAL_SPI_Transmit(hspi_, start, byte_count, 100);
}
