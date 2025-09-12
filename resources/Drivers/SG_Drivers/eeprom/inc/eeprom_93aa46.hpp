#pragma once

#include "eeprom.hpp"

#include <array>

// this eeprom has a start bit before it begins reading bits it recieves on its mosi line, which means you can prepend as many zeros as you like so that it correctly aligns with the 8 bit at a time send protocol of the HAL_SPI functions
class Eeprom93AA46 final : public Eeprom
{
   public:
    Eeprom93AA46(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
        : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin)
    {
        csLow();
    }

    EepromStatus read(uint32_t addr, void* buf, size_t len) override;
    EepromStatus write(uint32_t addr, const void* buf, size_t len) override;

    uint32_t size() const override { return 128; }
    uint16_t programGranularity() const override { return 1; }
    uint16_t pageSize() const override { return 1; }

   private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* cs_port_;
    uint16_t cs_pin_;

    // constant opcodes
    struct kOpcode
    {
        static constexpr uint8_t Read = 0b10;
        static constexpr uint8_t Write = 0b01;
    };

    // EWEN  -> 1 00 1 1 X X X X X
    static inline constexpr std::array<uint8_t, 2> kEwen = {0b00000010, 0b01100000};
    // EWDS  -> 1 00 0 0 XXXXX
    static inline constexpr std::array<uint8_t, 2> kEwds = {0b00000010, 0b00000000};
    static inline constexpr size_t kEwLen = 2;

    // instruction types, only used because we want to send data as fast as possible and we have to send leading 0's
    // so it helps us send an optimal amount of bytes
    enum class InstrLen : size_t
    {
        kEW = 10,
        kRW = 18
    };

    // cs control helpers
    inline void csLow() { HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET); };
    inline void csHigh() { HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET); };

    // helper functions that send one of the commands to the eeprom
    EepromStatus sendRead(uint32_t addr, uint8_t& out);
    EepromStatus sendWrite(uint32_t addr, const uint8_t& byte);
    EepromStatus sendEWEN();
    EepromStatus sendEWDS();

    // function should be used to send a single command
    // count is in bits. Prepends zeros so that it aligns
    // cannot send more than 32 bits
    // basically expecting for the count variable to be either 10 or 18 from the constants above
    //

    // TODO: Change count variable to instruction type, and function to be one that sends a specific instruction to the eeprom
    EepromStatus sendInstruction(uint8_t* instr, size_t len);
};