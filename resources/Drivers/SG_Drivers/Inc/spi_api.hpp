#pragma once

#include <cstdint>

#include "transmit_status.hpp"

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#endif

#define TRY(x)                               \
    do                                       \
    {                                        \
        if ((x) != HAL_OK)                   \
            return sg::hal_to_spi_status(x); \
    } while (0)

#define HAL_SPI_TRANSMIT_TIMEOUT 100

namespace sg
{

class Stm32HalSpi
{
   public:
    Stm32HalSpi(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin, bool active_low_cs)
        : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin), active_low_cs_(active_low_cs){};

    Status transmit(const uint8_t* data, uint16_t len)
    {
        ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
        TRY(HAL_SPI_Transmit(hspi_, data, len, HAL_SPI_TRANSMIT_TIMEOUT));

        return Status::Ok;
    }

    sg::Status receive(uint8_t* data, uint16_t len, uint8_t fill = 0xFF)
    {
        ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
        return txrx_fill(data, len, fill);
    }

    sg::Status transmitReceive(const uint8_t* cmd,
                               uint16_t cmd_len,
                               uint8_t* rx,
                               uint16_t rx_len,
                               uint8_t fill = 0xFF)
    {
        ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
        TRY(HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(cmd), cmd_len, HAL_SPI_TRANSMIT_TIMEOUT));
        return txrx_fill(rx, rx_len, fill);
    }

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

    Status txrx_fill(uint8_t* rx, uint16_t len, uint8_t fill)
    {
        // Chunk to avoid large temp allocation if len is big
        uint8_t dummy_tx[32];
        for (size_t i = 0; i < sizeof(dummy_tx); ++i)
            dummy_tx[i] = fill;

        while (len > 0)
        {
            uint16_t chunk = (len > sizeof(dummy_tx)) ? sizeof(dummy_tx) : len;
            TRY(HAL_SPI_TransmitReceive(hspi_, dummy_tx, rx, chunk, HAL_SPI_TRANSMIT_TIMEOUT));
            rx += chunk;
            len -= chunk;
        }
        return Status::Ok;
    }
};

}  // namespace sg