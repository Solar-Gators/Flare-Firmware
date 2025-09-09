#include "eeprom_93aa46.hpp"

Eeprom93AA46::Eeprom93AA46(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
{
    hspi_ = hspi;
    cs_port_ = cs_port;
    cs_pin_ = cs_pin;
}

int Eeprom93AA46::read(uint32_t addr, void* buf, size_t len)
{
    return 0;
}

int Eeprom93AA46::write(uint32_t addr, const void* buf, size_t len)
{
    return 0;
}
