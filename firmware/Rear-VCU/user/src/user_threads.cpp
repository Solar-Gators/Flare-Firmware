#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.hpp"
#include "main.h"
#include "rearvcu_state.h"

#define ARRAY_PRECHARGE_HOLD_TIME_MS 500

// initialize devices here and such
void init_user()
{
    // assert atomics work
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

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

    // turn on mc
    HAL_GPIO_WritePin(MC_MAIN_CTRL_GPIO_Port, MC_MAIN_CTRL_Pin, GPIO_PIN_SET);

    // select analog throttle
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

    // TODO: initialize ina chip here

    // can
    rearvcu::can_init();
}

[[noreturn]] void startHeartbeatTask_user(void* argument)
{
    sg::CANFrame supp_batt_frame{0x021,
                                 sg::CANFrameIDType::STANDARD,
                                 sg::CANFrameRTRMode::DATA,
                                 sg::CANFrameLen::BYTES_4,
                                 0,
                                 {}};

    for (;;)
    {
        // blink ok led
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);

        // send mitsuba request for frame0 to get wheel rpm, shouldnt be sent faster than every 500ms
        rearvcu::can_device.send(rearvcu::mitsuba_frame0_request);

        // supp batt voltage can be read and sent in this thread as its not as urgent/important
        uint16_t supp_batt_voltage_mv = 0xFFFF;  // TODO: could get voltage of supp batt here
        uint16_t supp_batt_current = 0xFFFF;     // TODO: could get current draw of supp batt here
        supp_batt_frame.data[0] = static_cast<uint8_t>(supp_batt_voltage_mv);       // lsb
        supp_batt_frame.data[1] = static_cast<uint8_t>(supp_batt_voltage_mv >> 8);  // msb
        supp_batt_frame.data[2] = static_cast<uint8_t>(supp_batt_current);          // lsb
        supp_batt_frame.data[3] = static_cast<uint8_t>(supp_batt_current >> 8);     // msb
        rearvcu::can_device.send(supp_batt_frame);

        osDelay(500);
    }
}

[[noreturn]] void startRegenThrottleTask_user(void* argument)
{
    for (;;)
    {
        // kick watchdog by sending a falling edge
        HAL_GPIO_WritePin(WATCHDOG_INPUT_GPIO_Port, WATCHDOG_INPUT_Pin, GPIO_PIN_RESET);
        osDelay(1);
        HAL_GPIO_WritePin(WATCHDOG_INPUT_GPIO_Port, WATCHDOG_INPUT_Pin, GPIO_PIN_SET);

        // consistently update throttle and regen here based on..
        // throttle value over can
        // regen value over can
        // cruise control stuff
        osDelay(25);
    }
}

// should continuously write to pins like power/eco, direction, contactors, and send CAN message to tell car that mc is enabled and such
[[noreturn]] void startOutputsTask_user(void* argument)
{
    sg::CANFrame rearvcu_statuses_frame{0x020,
                                        sg::CANFrameIDType::STANDARD,
                                        sg::CANFrameRTRMode::DATA,
                                        sg::CANFrameLen::BYTES_8,
                                        0,
                                        {}};

    flare_can::ArrayContactors array_contactors = flare_can::ArrayContactors::BOTH_OPEN;
    uint32_t precharge_closed_timestamp = 0;

    for (;;)
    {
        // TODO: can make these writes less frequent using flag, only call writepin on change yk
        // power eco pin
        flare_can::MCPowerMode mc_power_mode_requested =
            rearvcu::state.mc_power_mode_requested.load();
        HAL_GPIO_WritePin(MC_PWR_ECO_CTRL_GPIO_Port,
                          MC_PWR_ECO_CTRL_Pin,
                          static_cast<GPIO_PinState>(mc_power_mode_requested));

        // direction pin
        flare_can::Direction direction_requested = rearvcu::state.direction_requested.load();
        HAL_GPIO_WritePin(MC_FWD_REV_CTRL_GPIO_Port,
                          MC_FWD_REV_CTRL_Pin,
                          static_cast<GPIO_PinState>(direction_requested));

        // array contactors logic
        // TODO: user timer peripheral for consistent timer logic
        if (rearvcu::state.array_contactors_requested_closed.load())
        {
            if (array_contactors == flare_can::ArrayContactors::BOTH_OPEN)
            {
                // close pre
                HAL_GPIO_WritePin(PRE_ARRAY_CTRL_GPIO_Port, PRE_ARRAY_CTRL_Pin, GPIO_PIN_SET);
                precharge_closed_timestamp = HAL_GetTick();
                array_contactors = flare_can::ArrayContactors::PRECHARGE_CLOSED;
            }
            else if (array_contactors == flare_can::ArrayContactors::PRECHARGE_CLOSED &&
                     HAL_GetTick() - precharge_closed_timestamp > ARRAY_PRECHARGE_HOLD_TIME_MS)
            {
                // close main
                HAL_GPIO_WritePin(MAIN_ARRAY_CTRL_GPIO_Port, MAIN_ARRAY_CTRL_Pin, GPIO_PIN_SET);
                array_contactors = flare_can::ArrayContactors::MAIN_CLOSED;
            }
        }
        else
        {
            // open both contactors
            HAL_GPIO_WritePin(PRE_ARRAY_CTRL_GPIO_Port, PRE_ARRAY_CTRL_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MAIN_ARRAY_CTRL_GPIO_Port, MAIN_ARRAY_CTRL_Pin, GPIO_PIN_RESET);
            array_contactors = flare_can::ArrayContactors::BOTH_OPEN;
        }

        // setup and send diagnostic can message
        rearvcu_statuses_frame.data[0] =
            1;  // mc enable should always be enabled if this board is alive, written to at startup
        rearvcu_statuses_frame.data[1] = static_cast<uint8_t>(direction_requested);
        rearvcu_statuses_frame.data[2] = static_cast<uint8_t>(mc_power_mode_requested);
        rearvcu_statuses_frame.data[3] = static_cast<uint8_t>(array_contactors);
        rearvcu_statuses_frame.data[4] = rearvcu::state.car_speed.load();
        rearvcu::can_device.send(rearvcu_statuses_frame);

        osDelay(200);
    }
}