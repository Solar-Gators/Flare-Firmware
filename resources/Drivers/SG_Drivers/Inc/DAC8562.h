#pragma once
#include "spi_api.hpp"

namespace sg
{

// spi notes
// an entire command is 24 bits
// bring sync line / chip select low, then clock 24 bits, then bring line high
// first two bits ignored
// next 3 bits are command
// next 3 are address select (typically dac a or dac b)
// next 16 are data

class DAC8562
{
   public:
    DAC8562(SPI_HandleTypeDef* hspi,
            GPIO_TypeDef* cs_port,
            uint16_t cs_pin,
            ActivationLevel al = ActivationLevel::ActiveLow);

   private:
    SpiDevice spi_;

    // constant command bytes to pass into writeRegister()
    // write to and update dac
    static constexpr uint8_t WRITE_UPDATE_DACA_CMD = 0b00011000;
    static constexpr uint8_t WRITE_UPDATE_DACB_CMD = 0b00011001;

    // power up dac a and dac b,
    static constexpr uint8_t POWERUP_CMD =
        0b00100000;  // powerup specifically needs the 0 data byte as well
    static constexpr uint16_t POWERUP_DATA = 0x0000;

    // reset all registers and update all dacs
    static constexpr uint8_t FULL_RESET_CMD = 0b00101000;
    static constexpr uint8_t FULL_RESET_DATA = 0x0001;

    // command bytes is made of 2 bits dont care, 3 bits command, 3 bits address, some examples are above
    HAL_StatusTypeDef writeRegister(uint8_t cmd_byte, uint16_t data_bytes);

   public:
    // software reset
    HAL_StatusTypeDef init();

    // dac with 16 bit resolution, val should be 0 to 65,535
    HAL_StatusTypeDef setChannelA(uint16_t val);

    // dac with 16 bit resolution, val should be 0 to 65,535
    HAL_StatusTypeDef setChannelB(uint16_t val);
};

}  // namespace sg