#pragma once

#include <cstddef>
#include <cstdint>

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_def.h"
#include "stm32u5xx_hal_spi.h"
#else
#error "Define your STM32 part macro (e.g., STM32U575xx or STM32L476xx)."
#endif

/* 
0x00–0x03 : BOOT_COUNT   (uint32_t, little-endian)
0x04      : BOOT_CSUM    (boot0 ^ boot1 ^ boot2 ^ boot3)   ← lets you detect torn writes (the ^ are xor's) (checksum)
0x05      : NEXT_SLOT    (addr to start writing to next) 

0x06–0x7D : 24 records × 5 bytes each (120 bytes total)
0x7E–0x7F : spare
*/

enum class EepromStatus : uint8_t
{
    kOk,
    kError,
    kBusy,
    kTimeout,
    kInvalidParam
};

// abc for very basic eeprom, might get wierd if you wanna use an eeprom with pages that limit how many bytes you can write at once
class Eeprom
{
   public:
    virtual ~Eeprom() = default;

    virtual EepromStatus read(uint32_t addr, void* buf, size_t len) = 0;
    virtual EepromStatus write(uint32_t addr, const void* buf, size_t len) = 0;

    // max amount of bytes that can be written at once
    virtual uint16_t programGranularity() const = 0;
    // page size, for some eeprom's you can't write to multiple pages in one write
    virtual uint16_t pageSize() const = 0;
    // total size of eeprom in bytes
    virtual uint32_t size() const = 0;

    EepromStatus readByte(uint32_t addr, uint8_t& out) { return read(addr, &out, 1); }

    EepromStatus writeByte(uint32_t addr, uint8_t val) { return write(addr, &val, 1); }

   protected:
#if defined(HAL_SPI_MODULE_ENABLED)
    static inline EepromStatus hal_to_eeprom_status(const HAL_StatusTypeDef& status)
    {
        switch (status)
        {
            case HAL_OK:
                return EepromStatus::kOk;
            case HAL_ERROR:
                return EepromStatus::kError;
            case HAL_BUSY:
                return EepromStatus::kBusy;
            case HAL_TIMEOUT:
                return EepromStatus::kTimeout;
            default:
                return EepromStatus::kInvalidParam;
        }
    }
#endif
};
