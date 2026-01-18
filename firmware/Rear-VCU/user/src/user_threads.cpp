#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.hpp"
#include "main.h"
#include "rearvcu_state.h"

// initialize devices here and such
void init_user()
{
    // can
    can_init();

    // assert atomics work
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}
void startRegenThrottleTask_user(void* argument)
{
    for (;;)
    {
        // consistently update throttle and regen here based on..
        // throttle value over can
        // regen value over can
        // cruise control stuff
        osDelay(1000);
    }
}