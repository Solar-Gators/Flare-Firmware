#include "DAC8562.h"
#include <array>

namespace sg
{

inline DAC8562::DAC8562(SPI_HandleTypeDef* hspi,
                        GPIO_TypeDef* cs_port,
                        uint16_t cs_pin,
                        ActivationLevel al) : spi_(hspi, cs_port, cs_pin, al)
{
    // write to reset register
}

HAL_StatusTypeDef DAC8562::writeRegister(uint8_t cmd_byte, uint16_t data_bytes)
{
    // transmit cmd byte, then transmit data bytes
    std::array<uint8_t, 3> bytes{
        cmd_byte,
        static_cast<uint8_t>(data_bytes >> 8),
        static_cast<uint8_t>(data_bytes & 0xFF)
    };

    return spi_.transmit(bytes.data(), bytes.size());
}

} // namespace sg

