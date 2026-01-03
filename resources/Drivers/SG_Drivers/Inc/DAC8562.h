#pragma once
#include "spi_api.hpp"

namespace sg
{

class DAC8562
{
public:
    DAC8562(SPI_HandleTypeDef* hspi,
                 GPIO_TypeDef* cs_port,
                 uint16_t cs_pin,
                 ActivationLevel al = ActivationLevel::ActiveLow)
        : spi_(hspi, cs_port, cs_pin, al) { }


private:
    SpiDevice spi_;

};

} // namespace sg