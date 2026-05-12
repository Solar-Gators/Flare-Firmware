#pragma once

#include "stm32u5xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

// How to use
// watchdog instance name can be repeats because it is technically a local var
// but for debug, use different names (ex: wdog1, wdog_2) for different threads
// supports up to 16 threads, can be increased with id count

/*
Put this inside the thread outside of the for(;;) loop
sg::Watchdog wdog;
wdog.MX_IWDG_Init();

Put this inside the for loop
wdog.Kick();
*/

extern IWDG_HandleTypeDef hiwdg;

namespace sg
{
// IWDG driver
class Watchdog
{
   public:
    // error handling
    static void Error_Handler(void);

    // Initialize the watchdog
    HAL_StatusTypeDef IWDG_Init(void);

    // Kick the watchdog
    void Kick(void);

   private:
    static bool threadReady[16];  // Supports up to 16 threads
    static uint8_t totalThreads;  // How many threads have registered
    uint8_t id = 0;               // This specific instance's index
};
}  // namespace sg
