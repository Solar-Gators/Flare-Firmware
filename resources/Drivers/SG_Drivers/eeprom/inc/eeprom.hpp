#pragma once

#include <cstddef>
#include <cstdint>

/* 
0x00–0x03 : BOOT_COUNT   (uint32_t, little-endian)
0x04      : BOOT_CSUM    (boot0 ^ boot1 ^ boot2 ^ boot3)   ← lets you detect torn writes (the ^ are xor's) (checksum)
0x05      : NEXT_SLOT    (addr to start writing to next) 

0x06–0x7D : 24 records × 5 bytes each (120 bytes total)
0x7E–0x7F : spare
*/

enum class EepromErr : uint8_t
{
    Ok = 0xFF,
    Brownout = 0x01,
    ErrorHandler = 0x02,
    HardFaultHandler = 0x03
};

// abc for very basic eeprom, might get wierd if you wanna use an eeprom with pages that limit how many bytes you can write at once
class Eeprom
{
   public:
    virtual ~Eeprom() = default;

    virtual int read(uint32_t addr, void* buf, size_t len) = 0;
    virtual int write(uint32_t addr, const void* buf, size_t len) = 0;

    // max amount of bytes that can be written at once
    virtual uint16_t programGranularity() const = 0;
    // page size, for some eeprom's you can't write to multiple pages in one write
    virtual uint16_t pageSize() const = 0;
    // total size of eeprom in bytes
    virtual uint32_t size() const = 0;

    int readByte(uint32_t addr, uint8_t& out) { return read(addr, &out, 1); }

    int writeByte(uint32_t addr, uint8_t val) { return write(addr, &val, 1); }
};
