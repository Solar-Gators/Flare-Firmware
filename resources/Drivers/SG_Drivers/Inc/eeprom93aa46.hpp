#pragma once

#include "eeprom.hpp"
#include "spi_target.hpp"

#include <array>
#include <memory>

namespace sg
{

// this eeprom has a start bit before it begins reading bits it receives on its mosi line, which means you can prepend as many zeros as you like so that it correctly aligns with the 8 bit at a time send protocol of the HAL_SPI functions

class Eeprom93AA46 final : public Eeprom
{
   public:
    Eeprom93AA46(SPI_HandleTypeDef* hspi,
                 GPIO_TypeDef* cs_port,
                 uint16_t cs_pin,
                 bool active_low_cs)
        : spi_(hspi, cs_port, cs_pin, active_low_cs)
    {
    }

    HAL_StatusTypeDef read(uint32_t addr, uint8_t* buf, size_t len);
    HAL_StatusTypeDef write(uint32_t addr, const uint8_t* buf, size_t len);

    uint32_t size() const override { return 128; }
    uint16_t programGranularity() const override { return 1; }
    uint16_t pageSize() const override { return 1; }

   private:
    SpiTarget spi_;

    // EWEN  -> 1 00 1 1 X X X X X
    static inline constexpr std::array<uint8_t, 2> kEwen = {0b00000010, 0b01100000};
    // EWDS  -> 1 00 0 0 XXXXX
    static inline constexpr std::array<uint8_t, 2> kEwds = {0b00000010, 0b00000000};

    // start bit and opcode for read, length is byte length of instruction
    static inline constexpr uint32_t kRdMask = (0b110 << 7);
    static inline constexpr size_t kRLen = 2;

    // start bit and opcode for write, length in byte length of instruction
    static inline constexpr uint32_t kWrMask = (0b101 << 15);
    static inline constexpr size_t kWLen = 3;

    // address is 7 bits
    static inline constexpr uint8_t kAddrMask = 0b01111111;

    // helper functions that send one of the commands to the eeprom
    HAL_StatusTypeDef sendRead(uint32_t addr, uint8_t& out);
    HAL_StatusTypeDef sendWrite(uint32_t addr, const uint8_t& byte);
    HAL_StatusTypeDef sendEWEN();
    HAL_StatusTypeDef sendEWDS();

    // internal helpers
    HAL_StatusTypeDef sendWriteFromBitInstruction(uint32_t instr);
    HAL_StatusTypeDef sendReadFromBitInstruction(uint32_t instr, uint8_t& out);
};

}  // namespace sg