#include "stm32_hal_spi_target.hpp"

#include <cstdint>

#include "transmit_status.hpp"

#define TRY(x)                           \
    do                                   \
    {                                    \
        if ((x) != HAL_OK)               \
            return hal_to_spi_status(x); \
    } while (0)

#define HAL_SPI_TRANSMIT_TIMEOUT 100

#include <cstdint>

#include <memory>

namespace sg
{

static inline Status hal_to_spi_status(const HAL_StatusTypeDef& status)
{
    switch (status)
    {
        case HAL_OK:
            return Status::Ok;
        case HAL_ERROR:
            return Status::Error;
        case HAL_BUSY:
            return Status::Busy;
        case HAL_TIMEOUT:
            return Status::Timeout;
        default:
            return Status::InvalidParam;
    }
}

Stm32HalSpiTarget::Stm32HalSpiTarget(SPI_HandleTypeDef* hspi,
                                     GPIO_TypeDef* cs_port,
                                     uint16_t cs_pin,
                                     bool active_low_cs)
    : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin), active_low_cs_(active_low_cs){};

Status Stm32HalSpiTarget::transmit(const uint8_t* data, uint16_t len)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
    TRY(HAL_SPI_Transmit(hspi_, data, len, HAL_SPI_TRANSMIT_TIMEOUT));

    return Status::Ok;
}

Status Stm32HalSpiTarget::receive(uint8_t* data, uint16_t len, uint8_t fill)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
    return txrx_fill(data, len, fill);
}

Status Stm32HalSpiTarget::transmitReceive(const uint8_t* cmd,
                                          uint16_t cmd_len,
                                          uint8_t* rx,
                                          uint16_t rx_len,
                                          uint8_t fill)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
    TRY(HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(cmd), cmd_len, HAL_SPI_TRANSMIT_TIMEOUT));
    return txrx_fill(rx, rx_len, fill);
}

Status Stm32HalSpiTarget::txrx_fill(uint8_t* rx, uint16_t len, uint8_t fill)
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

}  // namespace sg