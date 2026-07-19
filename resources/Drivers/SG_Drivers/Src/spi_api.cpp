#include "spi_api.hpp"

#include <cstdint>

#include <memory>

#define TRY(x)             \
    do                     \
    {                      \
        if ((x) != HAL_OK) \
            return x;      \
    } while (0)

#define HAL_SPI_TRANSMIT_TIMEOUT 100

namespace sg
{

SpiDevice::SpiDevice(SPI_HandleTypeDef* hspi,
                     GPIO_TypeDef* cs_port,
                     uint16_t cs_pin,
                     ActivationLevel al)
    : hspi_(hspi), cs_port_(cs_port), cs_pin_(cs_pin), al_(al)
{
}

HAL_StatusTypeDef SpiDevice::transmit(const uint8_t* data, uint16_t len)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, al_};
    TRY(HAL_SPI_Transmit(hspi_, data, len, HAL_SPI_TRANSMIT_TIMEOUT));

    return HAL_OK;
}

HAL_StatusTypeDef SpiDevice::receive(uint8_t* data, uint16_t len, uint8_t fill)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, al_};
    return txrx_fill(data, len, fill);
}

HAL_StatusTypeDef SpiDevice::transmitReceive(const uint8_t* cmd,
                                             uint16_t cmd_len,
                                             uint8_t* rx,
                                             uint16_t rx_len,
                                             uint8_t fill)
{
    ChipSelectGuard guard{cs_port_, cs_pin_, al_};
    TRY(HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(cmd), cmd_len, HAL_SPI_TRANSMIT_TIMEOUT));
    return txrx_fill(rx, rx_len, fill);
}

HAL_StatusTypeDef SpiDevice::txrx_fill(uint8_t* rx, uint16_t len, uint8_t fill)
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