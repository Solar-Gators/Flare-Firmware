#pragma once

#if defined(STM32L476xx)
#include "cmsis_os.h"

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_can.h"
#elif defined(STM32U575xx)
#include "cmsis_os2.h"

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
    namespace sg
    {
    // forces user to pick one of the available ones
    enum class CANFrameLen : uint8_t
    {
        BYTES_0 = 0,
        BYTES_1 = 1,
        BYTES_2 = 2,
        BYTES_3 = 3,
        BYTES_4 = 4,
        BYTES_5 = 5,
        BYTES_6 = 6,
        BYTES_7 = 7,
        BYTES_8 = 8,
        BYTES_12 = 12,
        BYTES_16 = 16,
        BYTES_20 = 20,
        BYTES_24 = 24,
        BYTES_32 = 32,
        BYTES_48 = 48,
        BYTES_64 = 64
    };
    }  // namespace sg
#elif defined(HAL_CAN_MODULE_ENABLED)
typedef CAN_HandleTypeDef CanHandle_t;
typedef CAN_FilterTypeDef CanFilter_t;
namespace sg
{
// forces user to pick one of the available ones
enum class CANFrameLen
{
    BYTES_0 = 0,
    BYTES_1 = 1,
    BYTES_2 = 2,
    BYTES_3 = 3,
    BYTES_4 = 4,
    BYTES_5 = 5,
    BYTES_6 = 6,
    BYTES_7 = 7,
    BYTES_8 = 8,
};
}  // namespace sg
#else
// #error "Enable HAL_FDCAN_MODULE_ENABLED or HAL_CAN_MODULE_ENABLED in stm32*_hal_conf.h"
typedef int CanHandle_t;
typedef int CanFilter_t;
#endif

    namespace sg
    {
    enum class CANFrameIDType : uint8_t
    {
        STANDARD = 0,
        EXTENDED = 1
    };
    enum class CANFramePriority : uint8_t
    {
        HIGH = 0,
        LOW = 1
    };
    enum class CANFrameRTRMode : uint8_t
    {
        DATA = 0,
        REMOTE = 1
    };
    }  // namespace sg

#ifdef __cplusplus
}  // extern "C"
#endif