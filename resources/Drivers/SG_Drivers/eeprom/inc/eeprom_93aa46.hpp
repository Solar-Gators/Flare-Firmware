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

class Eeprom93AA46 final : public Eeprom
{
   public:
    Eeprom93AA46(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
        : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin)
    {
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

    inline void csLow(){};
    inline void csHigh(){};

    static constexpr uint8_t OpRead = 0b10;
    static constexpr uint8_t OpWrite = 0b01;
};