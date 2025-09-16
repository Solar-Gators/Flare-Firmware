#pragma once
#include <cstdint>

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#endif

namespace sg
{

enum class Status : uint8_t
{
    Ok,
    Error,
    Busy,
    Timeout,
    InvalidParam
};

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

}  // namespace sg