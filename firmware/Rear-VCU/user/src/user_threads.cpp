#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.hpp"
#include "main.h"
#include "rearvcu_state.h"

// initialize devices here and such
void init_user()
{
    // assert atomics work
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

    // can
    can_init();

    // maybe need to initialize ina chip here or something
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);

        // send mitsuba request for frame0 to get wheel rpm, shouldnt be sent faster than every 500ms
        can_device.Send(mitsuba_frame0_request);

        // could get voltage of supp batt here
        vcu_state.supp_battery_voltage_mv = 12000;  // dummy

        osDelay(550);
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
        osDelay(100);
    }
}