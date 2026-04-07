#pragma once

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

// How to use
/*

Put this in the thread OUTSIDE of the for(;;) loop
sg::Watchdog wdog;
wdog.MX_IWDG_Init();

Put this INSIDE the for loop

wdog.Kick();

*/


namespace sg
{
    // IWDG driver
    class Watchdog
    {
    public:
        // error handling
        static void Error_Handler(void);

        // Initialize the watchdog
        // called once during thread init
        HAL_StatusTypeDef MX_IWDG_Init(void);
        void Kick(void);

        // TODO: other options for expansion, would need to write
        // bool IsReset(); // check if last reset was caused by IWDG
        // void ClearResetFlag(); // clear the reset flag

    private:
        IWDG_HandleTypeDef hiwdg;
    };

} // namespace sg