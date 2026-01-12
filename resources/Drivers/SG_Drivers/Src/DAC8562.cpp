#include "DAC8562.h"
#include <array>

namespace sg
{

DAC8562::DAC8562(SPI_HandleTypeDef* hspi,
                        GPIO_TypeDef* cs_port,
                        uint16_t cs_pin,
                        ActivationLevel al) : spi_(hspi, cs_port, cs_pin, al) {}

HAL_StatusTypeDef DAC8562::writeRegister(uint8_t cmd_byte, uint16_t data_bytes)
{
    // transmit cmd byte, then transmit data bytes
    const std::array<uint8_t, 3> bytes {
        cmd_byte,
        static_cast<uint8_t>(data_bytes >> 8),
        static_cast<uint8_t>(data_bytes & 0xFF)
    };

    return spi_.transmit(bytes.data(), bytes.size());
}
HAL_StatusTypeDef DAC8562::init()
{
    if (HAL_StatusTypeDef status = writeRegister(FULL_RESET_CMD, FULL_RESET_DATA);
        status != HAL_OK)
    {
        return status;
    }

    return writeRegister(POWERUP_CMD, POWERUP_DATA);
}

HAL_StatusTypeDef DAC8562::setChannelA(uint16_t val)
{
   return writeRegister(WRITE_UPDATE_DACA_CMD, val);
}

HAL_StatusTypeDef DAC8562::setChannelB(uint16_t val)
{
   return writeRegister(WRITE_UPDATE_DACB_CMD, val);
}

} // namespace sg

