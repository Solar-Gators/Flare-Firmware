#pragma once

#include <cstdint>

#include <memory>

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#endif

#include "spi_target.hpp"

namespace sg
{

class SpiTarget
{
   public:
    SpiTarget(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin, bool active_low_cs);

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
    bool active_low_cs_;

    struct ChipSelectGuard
    {
        GPIO_TypeDef* port;
        uint16_t pin;
        bool active_low;
        explicit ChipSelectGuard(GPIO_TypeDef* p, uint16_t n, bool a)
            : port(p), pin(n), active_low(a)
        {
            if (active_low)
            {
                HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
            }
        }
        ~ChipSelectGuard()
        {
            if (active_low)
            {
                HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
            }
            else
            {
                HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
            }
        }
    };

    HAL_StatusTypeDef txrx_fill(uint8_t* rx, uint16_t len, uint8_t fill);
};

}  // namespace sg