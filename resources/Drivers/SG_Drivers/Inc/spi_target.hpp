#pragma once

#include "spi_target.hpp"
#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#endif

#include <memory>

namespace sg
{

// this eeprom has a start bit before it begins reading bits it receives on its mosi line, which means you can prepend as many zeros as you like so that it correctly aligns with the 8 bit at a time send protocol of the HAL_SPI functions
enum class ActivationLevel
{
    ActiveLow = GPIO_PIN_RESET,
    ActiveHigh = GPIO_PIN_SET
};

static inline ActivationLevel invert(const ActivationLevel a)
{
    return (a == ActivationLevel::ActiveLow) ? ActivationLevel::ActiveHigh
                                             : ActivationLevel::ActiveLow;
}

class SpiTarget
{
   public:
    SpiTarget(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin, ActivationLevel al);

    HAL_StatusTypeDef transmit(const uint8_t* data, uint16_t len);

    HAL_StatusTypeDef receive(uint8_t* data, uint16_t len, uint8_t fill = 0xFF);

    HAL_StatusTypeDef transmitReceive(const uint8_t* cmd,
                                      uint16_t cmd_len,
                                      uint8_t* rx,
                                      uint16_t rx_len,
                                      uint8_t fill = 0xFF);

   private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* cs_port_;
    uint16_t cs_pin_;
    ActivationLevel al_;

    struct ChipSelectGuard
    {
        GPIO_TypeDef* port;
        uint16_t pin;
        ActivationLevel al;
        explicit ChipSelectGuard(GPIO_TypeDef* p, const uint16_t n, const ActivationLevel a)
            : port(p), pin(n), al(a)
        {
            HAL_GPIO_WritePin(port, pin, static_cast<GPIO_PinState>(al));
        }
        ~ChipSelectGuard() { HAL_GPIO_WritePin(port, pin, static_cast<GPIO_PinState>(invert(al))); }
    };

    HAL_StatusTypeDef txrx_fill(uint8_t* rx, uint16_t len, uint8_t fill);
};

}  // namespace sg