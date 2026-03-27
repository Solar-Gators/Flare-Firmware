#pragma once

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

namespace sg
{
    // IWDG driver
    class Watchdog
    {
    public:
        // error handling
        static void Error_Handler(void);

        // Initialize the watchdog
        // called once during board init
        HAL_StatusTypeDef MX_IWDG_Init(void);
        void Kick(void);

        // may need to call this is not automatically enabled
        // static void EnableLSI();

        // other options for expansion, would need to write
        // bool IsReset(); // check if last reset was caused by IWDG
        // void ClearResetFlag(); // clear the reset flag

    private:
        IWDG_HandleTypeDef hiwdg;
    };

} // namespace sg