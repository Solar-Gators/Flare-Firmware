#pragma once
#include <cstdint>

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#endif

#include "transmit_status.hpp"

namespace sg
{

class SpiTarget
{
   public:
    virtual sg::Status transmit(const uint8_t* data, uint16_t len) = 0;

    virtual sg::Status receive(uint8_t* data, uint16_t len, uint8_t fill = 0xFF) = 0;

    virtual sg::Status transmitReceive(const uint8_t* cmd,
                                       uint16_t cmd_len,
                                       uint8_t* rx,
                                       uint16_t rx_len,
                                       uint8_t fill = 0xFF) = 0;
};

}  // namespace sg