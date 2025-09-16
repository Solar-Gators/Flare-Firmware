#pragma once

#include "spi_api.h"

class NRF24L01P : public SpiDevice
{
   public:
    NRF24L01P(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
        : SpiDevice(hspi, cs_port, cs_pin)
    {
    }
};