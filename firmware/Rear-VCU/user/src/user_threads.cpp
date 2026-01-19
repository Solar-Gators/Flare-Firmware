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

    // watchdog init
    HAL_GPIO_WritePin(WATCHDOG_SET1_GPIO_Port,
                      WATCHDOG_SET1_Pin,
                      GPIO_PIN_SET);  // set to 0b10 mode to use ratio of 8
    HAL_GPIO_WritePin(WATCHDOG_SET0_GPIO_Port,
                      WATCHDOG_SET0_Pin,
                      GPIO_PIN_RESET);  // 8 * 5 = 40ms timeout and needs to be kicked before
    HAL_GPIO_WritePin(WATCHDOG_INPUT_GPIO_Port, WATCHDOG_INPUT_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(
        WATCHDOG_ENABLE_GPIO_Port, WATCHDOG_ENABLE_Pin, GPIO_PIN_SET);  // enable watchdog

    // array contactors should start open
    HAL_GPIO_WritePin(PRE_ARRAY_CTRL_GPIO_Port, PRE_ARRAY_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MAIN_ARRAY_CTRL_GPIO_Port, MAIN_ARRAY_CTRL_Pin, GPIO_PIN_RESET);

    // maybe need to initialize ina chip here or something
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);

        // send mitsuba request for frame0 to get wheel rpm, shouldnt be sent faster than every 500ms
        can_device.Send(mitsuba_frame0_request);
        ++vcu_state.can_messages_sent;

        // could get voltage of supp batt here
        vcu_state.supp_battery_voltage_mv = 12000;  // dummy

        osDelay(550);
    }
}

void startRegenThrottleTask_user(void* argument)
{
    for (;;)
    {
        // kick watchdog by sending a falling edge
        HAL_GPIO_WritePin(WATCHDOG_INPUT_GPIO_Port, WATCHDOG_INPUT_Pin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(WATCHDOG_INPUT_GPIO_Port, WATCHDOG_INPUT_Pin, GPIO_PIN_SET);

        // consistently update throttle and regen here based on..
        // throttle value over can
        // regen value over can
        // cruise control stuff
        osDelay(25);
    }
}

void startOutputsTask_user(void* argument)
{
    ArrayContactors array_contactors = ArrayContactors::BOTH_OPEN;

    for (;;)
    {
        // should write to pins like power/eco, direction, contactors, and send CAN message to tell car that mc is enabled and such
    }
}