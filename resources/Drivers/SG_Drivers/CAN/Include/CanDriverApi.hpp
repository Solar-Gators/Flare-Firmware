#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_can.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_fdcan.h"
#endif

#if defined(HAL_FDCAN_MODULE_ENABLED)
using CanHandle_t = FDCAN_HandleTypeDef;
using CanFilter_t = FDCAN_FilterTypeDef;
#elif defined(HAL_CAN_MODULE_ENABLED)
using CanHandle_t = CAN_HandleTypeDef;
using CanFilter_t = CAN_FilterTypeDef;
#endif
