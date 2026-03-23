#include "watchdog.hpp"

#include "stm32u5xx.h"   // CMSIS device header

namespace mcu
{

    // LSI frequency = 32 kHz
    static constexpr uint32_t LSI_FREQ_HZ = 32000;

    // IWDG limits from reference manual
    static constexpr uint32_t IWDG_MAX_RELOAD = 0x0FFF;

    void Watchdog::Init(uint32_t timeout_ms)
    {
        if (initialized_){return;}

        EnableLSI();

        // Enable write access to IWDG registers
        IWDG->KR = 0x5555;

        IWDG->PR = IWDG_PR_PR_3; // prescaler = 32, might be IWDG_PR_PR_2

        uint32_t reload = ComputeReload(timeout_ms);
        IWDG->RLR = reload & IWDG_MAX_RELOAD;

        // reload counter
        IWDG->KR = 0xAAAA;

        // start watchdog
        IWDG->KR = 0xCCCC;

        initialized_ = true;
    }

    void Watchdog::Kick()
    {
        // Reload counter
        IWDG->KR = 0xAAAA;
    }

    bool Watchdog::WasWatchdogReset()
    {
        // Check reset flags
        bool caused_by_iwdg = (RCC->CSR & RCC_CSR_IWDGRSTF) != 0;

        // Clear reset flags (write 1 to RMVF)
        RCC->CSR |= RCC_CSR_RMVF;

        return caused_by_iwdg;
    }

    void Watchdog::EnableLSI()
    {
        // Enable LSI clock if not already enabled
        if ((RCC->BDCR & RCC_BDCR_LSION) == 0)
        {
            RCC->BDCR |= RCC_BDCR_LSION;

            // Wait until LSI is ready
            while ((RCC->BDCR & RCC_BDCR_LSIRDY) == 0)
            {
                // spin
            }
        }
    }

    uint32_t Watchdog::ComputeReload(uint32_t timeout_ms)
    {
        // Prescaler = 32
        constexpr uint32_t prescaler = 32;

        uint64_t ticks =
            (static_cast<uint64_t>(timeout_ms) * (LSI_FREQ_HZ / prescaler)) / 1000;

        if (ticks > IWDG_MAX_RELOAD)
            ticks = IWDG_MAX_RELOAD;

        if (ticks == 0)
            ticks = 1;

        return static_cast<uint32_t>(ticks);
    }

} // namespace mcu