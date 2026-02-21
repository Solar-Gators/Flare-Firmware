#pragma once

#include <cstdint>

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#endif

/*
THIS SHOULD BE MOVED IN LOGGER CLASS / FILE WHEN WE DO THAT LOL
0x00–0x03 : BOOT_COUNT   (uint32_t, little-endian)
0x04      : BOOT_CSUM    (boot0 ^ boot1 ^ boot2 ^ boot3)   ← lets you detect torn writes (the ^ are xor's) (checksum)
0x05      : NEXT_SLOT    (addr to start writing to next) 

0x06–0x7D : 24 records × 5 bytes each (120 bytes total)
0x7E–0x7F : spare
*/

namespace sg
{

// abc for very basic eeprom, might get wierd if you wanna use an eeprom with pages that limit how many bytes you can write at once
class Eeprom
{
   public:
    virtual ~Eeprom() = default;

    /**
     *
     * @param addr Address inside the eeprom to read from
     * @param buf Pointer to byte buffer that data at addr will be placed in
     * @param len The number of bytes read
     * @return HAL status denoting sucess of transmit. HAL_OK on success
     */
    virtual HAL_StatusTypeDef read(uint32_t addr, uint8_t* buf, size_t len) = 0;

    /**
     *
     * @param addr Address inside the eeprom to write to
     * @param buf Pointer to const byte buffer where data resides that will be written
     * @param len The number of bytes to write
     * @return HAL status denoting sucess of transmit. HAL_OK on success
     */
    virtual HAL_StatusTypeDef write(uint32_t addr, const uint8_t* buf, size_t len) = 0;

    /**
     *
     * @return The number of pages in the eeprom
     */
    virtual uint32_t size() const = 0;

    /**
     *
     * @return The size of a single page, in bytes
     */
    virtual uint16_t pageSize() const = 0;

    /**
     *
     * @return The number of bytes that can be read/written in one command
     */
    virtual uint16_t programGranularity() const = 0;

    /**
     *
     * @param addr Addr to read byte from
     * @param out Byte passed by reference that the read byte will be place in
     * @return HAL status denoting success of transmission
     */
    HAL_StatusTypeDef readByte(uint32_t addr, uint8_t& out) { return read(addr, &out, 1); }

    /**
     *
     * @param addr Addr to write byte to
     * @param val Const byte that will be written
     * @return HAL status denoting success of transmission
     */
    HAL_StatusTypeDef writeByte(uint32_t addr, const uint8_t val) { return write(addr, &val, 1); }
};

}  // namespace sg
