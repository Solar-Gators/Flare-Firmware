#include "spi_target.hpp"

#include <cstdint>

#define TRY(x)             \
    do                     \
    {                      \
        if ((x) != HAL_OK) \
            return x;      \
    } while (0)

#define HAL_SPI_TRANSMIT_TIMEOUT 100

#include <cstdint>

#include <memory>

namespace sg
{

SpiTarget::SpiTarget(SPI_HandleTypeDef* hspi,
                     GPIO_TypeDef* cs_port,
                     uint16_t cs_pin,
                     bool active_low_cs)
    : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin), active_low_cs_(active_low_cs){};

HAL_StatusTypeDef SpiTarget::transmit(const uint8_t* data, uint16_t len)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
    TRY(HAL_SPI_Transmit(hspi_, data, len, HAL_SPI_TRANSMIT_TIMEOUT));

    return HAL_OK;
}

HAL_StatusTypeDef SpiTarget::receive(uint8_t* data, uint16_t len, uint8_t fill)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
    return txrx_fill(data, len, fill);
}

HAL_StatusTypeDef SpiTarget::transmitReceive(const uint8_t* cmd,
                                             uint16_t cmd_len,
                                             uint8_t* rx,
                                             uint16_t rx_len,
                                             uint8_t fill)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, active_low_cs_};
    TRY(HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(cmd), cmd_len, HAL_SPI_TRANSMIT_TIMEOUT));
    return txrx_fill(rx, rx_len, fill);
}

HAL_StatusTypeDef SpiTarget::txrx_fill(uint8_t* rx, uint16_t len, uint8_t fill)
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
    return HAL_OK;
}

}  // namespace sg