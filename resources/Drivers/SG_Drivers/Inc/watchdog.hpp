#pragma once

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

// How to use
/*
Put this inside the thread outside of the for(;;) loop
sg::Watchdog wdog;
wdog.MX_IWDG_Init();

Put this inside the for loop
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
        HAL_StatusTypeDef MX_IWDG_Init(void);

        // Kick the watchdog
        void Kick(void);

    private:
        // watchdog instance
        static IWDG_HandleTypeDef hiwdg;

        static bool threadReady[16]; // Supports up to 16 threads
        static uint8_t totalThreads; // How many threads have registered
        uint8_t id = 0; // This specific instance's index
    };

} // namespace sg