#pragma once

#if defined(STM32L476xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_can.h"
#elif defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_fdcan.h"
#else
#error "Define your STM32 part macro (e.g., STM32U575xx or STM32L476xx)."
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(HAL_FDCAN_MODULE_ENABLED)
    typedef FDCAN_HandleTypeDef CanHandle_t;
    typedef FDCAN_FilterTypeDef CanFilter_t;
#elif defined(HAL_CAN_MODULE_ENABLED)
typedef CAN_HandleTypeDef CanHandle_t;
typedef CAN_FilterTypeDef CanFilter_t;
#else
#error "Enable HAL_FDCAN_MODULE_ENABLED or HAL_CAN_MODULE_ENABLED in stm32*_hal_conf.h"
#endif

#define SG_CAN_ID_STD 0u
#define SG_CAN_ID_EXT 1u

#define SG_CAN_PRIORITY_HIGH 0u
#define SG_CAN_PRIORITY_LOW 1u

#define SG_CAN_RTR_DATA 0u
#define SG_CAN_RTR_REMOTE 1u

#ifdef __cplusplus
}  // extern "C"
#endif