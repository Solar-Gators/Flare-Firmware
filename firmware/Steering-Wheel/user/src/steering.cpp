//
// Created by justin on 2/24/26.
//

#include "steering.h"

#include <sys/signal.h>

#include "../inc/steering_state.h"
#include "buttons.h"
#include "can.h"
#include "can_protocol.h"
#include "screen.h"

#include <limits>

extern TIM_HandleTypeDef htim1;

// private variables
namespace
{

}  // namespace

// public functions
namespace steering
{

HAL_StatusTypeDef init()
{
    // check atomics
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

    // inits
    initScreen();
    initButtons();
    initCan();

    return HAL_OK;
}

void sendRequestsMessage()
{
    static sg::CANFrame steering_requests_frame{0x064,
                                                sg::CANFrameIDType::STANDARD,
                                                sg::CANFrameRTRMode::DATA,
                                                sg::CANFrameLen::BYTES_8,
                                                0,
                                                {}};
    // turn signals
    const auto turn_signals = state.turn_signals_requested.load(std::memory_order_relaxed);
    steering_requests_frame.data[0] = static_cast<uint8_t>(turn_signals);

    // frwrd / reverse
    steering_requests_frame.data[1] =
        static_cast<uint8_t>(state.direction_requested.load(std::memory_order_relaxed));

    // array
    steering_requests_frame.data[2] = static_cast<uint8_t>(
        state.array_contactors_requested_closed.load(std::memory_order_relaxed));

    // horn
    steering_requests_frame.data[3] =
        static_cast<uint8_t>(state.horn_requested_on.load(std::memory_order_relaxed));

    // turn signals phase
    const bool blink_phase = (HAL_GetTick() / 500) % 2 == 0;

    steering_requests_frame.data[4] = static_cast<uint8_t>(blink_phase);

    // regen breaking strength
    steering_requests_frame.data[5] = state.regen_percent_requested.load(std::memory_order_relaxed);

    // pwr/eco request
    steering_requests_frame.data[6] =
        static_cast<uint8_t>(state.mc_power_mode_requested.load(std::memory_order_relaxed));

    // fan request
    steering_requests_frame.data[7] = state.fan_requested_on.load(std::memory_order_relaxed);

    can_device.send(steering_requests_frame);
}

void sendMitsubaRequestMessage()
{
    can_device.send(mitsuba_frame0_request);
}

// TODO: some indicator for the button 7 light
// TODO: important info not done: regenerative breaking
void processScreen()
{
    // cc values used for both cc and speed redraws
    static bool old_cc_on = state.is_cc_on.load(std::memory_order_relaxed);
    bool cc_on = state.is_cc_on.load(std::memory_order_relaxed);

    static uint8_t old_speed = state.car_speed.load(std::memory_order_relaxed);
    if (uint8_t speed = state.car_speed.load(std::memory_order_relaxed);
        speed != old_speed || cc_on != old_cc_on)
    {
        drawSpeed(speed);
        old_speed = speed;
        old_cc_on = cc_on;
    }

    static auto timerValue = state.timer_value.load(std::memory_order::relaxed);
    if (auto newTimerValue = state.timer_value.load(std::memory_order::relaxed);
        newTimerValue != timerValue)
    {
        drawTimer(newTimerValue);
        timerValue = newTimerValue;
    }

    static auto old_power_mode = state.mc_power_mode_requested.load(std::memory_order::relaxed);
    if (auto power_mode = state.mc_power_mode_requested.load(std::memory_order::relaxed);
        power_mode != old_power_mode)
    {
        drawPowerMode(power_mode);
        old_power_mode = power_mode;
    }

    // TODO: sup batt reading like .15 volts lower than it actually is with all the boards connected, test and fix
    static uint16_t old_supp_batt_mv = state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
    if (uint16_t sup_batt_mv = state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
        sup_batt_mv != old_supp_batt_mv)
    {
        drawSuppBatt(sup_batt_mv);
        old_supp_batt_mv = sup_batt_mv;
    }

    static auto old_direction = state.actual_direction.load(std::memory_order_relaxed);
    if (auto direction = state.actual_direction.load(std::memory_order_relaxed);
        direction != old_direction)
    {
        drawDirection(direction);
        old_direction = direction;
    }

    static uint16_t old_main_batt_cv = state.main_batt_voltage_cv.load(std::memory_order_relaxed);
    if (uint16_t main_batt_cv =
            state.main_batt_voltage_cv.load(std::memory_order_relaxed);  // centivolts
        main_batt_cv != old_main_batt_cv)
    {
        drawMainBatt(main_batt_cv);
        old_main_batt_cv = main_batt_cv;
    }

    static uint16_t old_high_temp_dc = state.high_temp_dc.load(std::memory_order_relaxed);
    if (uint16_t high_temp_dc = state.high_temp_dc.load(std::memory_order_relaxed);  // decicelcius
        old_high_temp_dc != high_temp_dc)
    {
        drawHighTemp(high_temp_dc);
        old_high_temp_dc = high_temp_dc;
    }

    static auto old_array_contactors =
        state.actual_array_contactors_status.load(std::memory_order_relaxed);
    if (auto array_contactors =
            state.actual_array_contactors_status.load(std::memory_order_relaxed);
        old_array_contactors != array_contactors)
    {
        drawArrayContactors(array_contactors);
        old_array_contactors = array_contactors;
    }

    static bool old_horn_status = state.horn_requested_on.load(std::memory_order_relaxed);
    if (bool horn_status = state.horn_requested_on.load(std::memory_order_relaxed);
        old_horn_status != horn_status)
    {
        drawHornStatus(horn_status);
        old_horn_status = horn_status;
    }

    static bool old_fan_status = state.fan_requested_on.load(std::memory_order_relaxed);
    if (bool fan_status = state.fan_requested_on.load(std::memory_order_relaxed);
        old_fan_status != fan_status)
    {
        drawFanStatus(fan_status);
        old_fan_status = fan_status;
    }

    static bool old_headlights_status =
        state.headlights_requested_on.load(std::memory_order_relaxed);
    if (bool headlights_status = state.headlights_requested_on.load(std::memory_order_relaxed);
        old_headlights_status != headlights_status)
    {
        drawHeadlightsStatus(headlights_status);
        old_headlights_status = headlights_status;
    }

    flare_can::CarKilledStatus old_killed_status;
    if (auto killed_status = state.killed_status.load(std::memory_order_relaxed);
        old_killed_status != killed_status)
    {
        drawKillStatus(killed_status);
        old_killed_status = killed_status;
    }

    // draw percent for debugging
    static uint16_t old_throttle_percent =
        state.throttle_percent_debug.load(std::memory_order_relaxed);
    if (auto throttle_percent = state.throttle_percent_debug.load(std::memory_order_relaxed);
        old_throttle_percent != throttle_percent)
    {
        // top middle
        drawThrottlePercent(throttle_percent);
        old_throttle_percent = throttle_percent;
    }

    drawTurnIndicator(state.left_blink_active.load(std::memory_order_relaxed),
                      state.right_blink_active.load(std::memory_order_relaxed),
                      state.blink_state.load(std::memory_order_relaxed));
}

void flareDance()
{
    auto direction = state.actual_direction.load(std::memory_order_relaxed);
    drawCar(direction);
}

void processHornButton()
{
    // -- HORN POLLING LOGIC --
    bool horn_pressed = (HAL_GPIO_ReadPin(HORN_PORT, HORN_PIN) == GPIO_PIN_RESET);
    state.horn_requested_on.store(horn_pressed, std::memory_order_relaxed);
}

// Process the Turn Signals and the Kill Status for blinking the top LEDs
void processTurnSignals()
{
    static uint32_t blink_period_ms = 500;
    static uint32_t last_blink_tick{};
    static bool blinker_on = false;

    uint32_t current_tick = HAL_GetTick();
    if (current_tick - last_blink_tick < blink_period_ms)
    {
        return;
    }
    last_blink_tick = current_tick;
    blinker_on = !blinker_on;

    // led control
    auto current_signal = state.turn_signals_requested.load();

    bool left_active = (current_signal == flare_can::TurnSignals::LEFT ||
                        current_signal == flare_can::TurnSignals::HAZARDS);
    bool right_active = (current_signal == flare_can::TurnSignals::RIGHT ||
                         current_signal == flare_can::TurnSignals::HAZARDS);

    // update variables for screen blinking effect
    state.left_blink_active.store(left_active, std::memory_order_relaxed);
    state.right_blink_active.store(right_active, std::memory_order_relaxed);
    state.blink_state.store(blinker_on, std::memory_order_relaxed);
}

// TODO: Check if this function is still required, may be already done in rearvcu
void processKill()
{
    /*
    if (state.killed_status.load() == flare_can::CarKilledStatus::DEAD)
    {
        state.array_contactors_requested_closed.store(false, std::memory_order_relaxed);
    }
    */
}

void processCC()
{
    bool active = state.is_cc_on.load();

    // this turns on the bottom left led for some reason, probably take out and put in buttons.cpp if anything
    /*HAL_GPIO_WritePin(
        BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, active ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, active ? GPIO_PIN_SET : GPIO_PIN_RESET);*/

    if (state.killed_status.load() == flare_can::CarKilledStatus::DEAD || state.brake_state.load())
    {
        // turn off cc if car dead
        state.is_cc_on.store(false);
    }

    static sg::CANFrame steering_requests_frame_2{0x065,
                                                  sg::CANFrameIDType::STANDARD,
                                                  sg::CANFrameRTRMode::DATA,
                                                  sg::CANFrameLen::BYTES_8,
                                                  0,
                                                  {}};
    // cc on/off
    steering_requests_frame_2.data[0] = state.is_cc_on.load(std::memory_order_relaxed);

    can_device.send(steering_requests_frame_2);
}

void processRegen()
{
    const bool regen_plus_pressed =
        (HAL_GPIO_ReadPin(REGEN_PLUS_PORT, REGEN_PLUS_PIN) == GPIO_PIN_RESET);
    const bool regen_minus_pressed =
        (HAL_GPIO_ReadPin(REGEN_MINUS_PORT, REGEN_MINUS_PIN) == GPIO_PIN_RESET);

    if (!regen_plus_pressed && !regen_minus_pressed)
        return;  // nothing pressed
    if (regen_plus_pressed && regen_minus_pressed)
        return;  // both pressed, ignore

    // linear increment
    constexpr std::uint8_t regen_delta = 13;
    uint8_t curr_regen = state.regen_percent_requested.load();
    if (regen_plus_pressed)
        curr_regen += std::min(
            regen_delta,
            static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::max() - curr_regen));
    if (regen_minus_pressed)
        curr_regen -= std::min<std::uint8_t>(regen_delta, curr_regen);
    state.regen_percent_requested.store(curr_regen, std::memory_order_relaxed);

    while (HAL_GPIO_ReadPin(REGEN_PLUS_PORT, REGEN_PLUS_PIN) == GPIO_PIN_RESET ||
           HAL_GPIO_ReadPin(REGEN_MINUS_PORT, REGEN_MINUS_PIN) == GPIO_PIN_RESET)
        ;
}

void processTimer()
{
    static bool was_running = false;
    bool requested = state.timer_requested_on.load(std::memory_order_relaxed);

    if (requested && !was_running)
    {
        // start timer
        __HAL_TIM_SET_COUNTER(&htim1, 0);                       // reset hardware counter
        state.timer_value.store(0, std::memory_order_relaxed);  // reset variable
        HAL_TIM_Base_Start_IT(&htim1);
        was_running = true;
    }
    else if (!requested && was_running)
    {
        // stop timer
        HAL_TIM_Base_Stop_IT(&htim1);
        was_running = false;
    }
}

}  // namespace steering
