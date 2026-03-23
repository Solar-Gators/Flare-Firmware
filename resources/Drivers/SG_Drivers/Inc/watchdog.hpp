#pragma once

#include <cstdint>

namespace mcu
{

    // IWDG driver, after started, cannot be stopped until reset
    class Watchdog
    {
    public:
        // Initialize the watchdog
        // called once during board init
        static void Init(uint32_t timeout_ms);

        // kicking the wd
        static void Kick();

        // check if last reset watch by wd, if so, return true
        static bool WasWatchdogReset();

    private:
        // computing ticks
        static uint32_t ComputeReload(uint32_t timeout_ms);
        // turn on timer
        static void EnableLSI();

        static inline bool initialized_ = false;
    };

} // namespace mcu