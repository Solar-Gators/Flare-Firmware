#pragma once

#include "eeprom.hpp"

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#else
#error "Define your STM32 part macro (e.g., STM32U575xx or STM32L476xx)."
#endif

// this eeprom has a start bit before it begins reading bits it recieves on its mosi line, which means you can prepend as many zeros as you like so that it correctly aligns with the 8 bit at a time send protocol of the HAL_SPI functions
class Eeprom93AA46 final : public Eeprom
{
   public:
    Eeprom93AA46(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
        : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin)
    {
        csLow();
    }

    int read(uint32_t addr, void* buf, size_t len) override;
    int write(uint32_t addr, const void* buf, size_t len) override;

    uint32_t size() const override { return 128; }
    uint16_t programGranularity() const override { return 1; }
    uint16_t pageSize() const override { return 1; }

   private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* cs_port_;
    uint16_t cs_pin_;

    // constant opcodes
    static constexpr uint8_t kOpcodeRead = 0b10;
    static constexpr uint8_t kOpcodeWrite = 0b01;
    static constexpr uint8_t kOpcodeGeneral = 0b00;

    // constand address's, specifically for use with the general opcode
    static constexpr uint8_t kAddrEWDS = 0b0000000;
    static constexpr uint8_t kAddrEWEN = 0b1100000;

    // Lengths of the two instruction types for the eeprom
    // either erase/write enable/disable or read/write
    enum class InstrLen : size_t
    {
        kEW = 10,
        kRW = 18
    };

    // cs control helpers
    inline void csLow() { HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET); };
    inline void csHigh() { HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET); };

    // helper functions that send one of the commands to the eeprom
    HAL_StatusTypeDef sendEWEN();
    HAL_StatusTypeDef sendEWDS();
    HAL_StatusTypeDef sendRead(uint32_t addr, uint8_t& out);
    HAL_StatusTypeDef sendWrite(uint32_t addr, const uint8_t& byte);

    // function should be used to send a single command
    // count is in bits. Prepends zeros so that it aligns
    // cannot send more than 32 bits
    // basically expecting for the count variable to be either 10 or 18 from the constants above
    //

    // TODO: Change count variable to instruction type, and function to be one that sends a specific instruction to the eeprom
    HAL_StatusTypeDef sendInstruction(const uint32_t& data, const InstrLen& instruction_length);
};