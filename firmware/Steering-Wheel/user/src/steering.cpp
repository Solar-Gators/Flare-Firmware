//
// Created by justin on 2/24/26.
//

#include "steering.h"

#include <sys/signal.h>

#include "buttons.h"
#include "can.h"
#include "can_protocol.h"
#include "screen.h"
#include "steering_state.h"

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
    steering_requests_frame.data[0] =
        static_cast<uint8_t>(state.turn_signals_requested.load(std::memory_order_relaxed));

    // frwrd / reverse
    steering_requests_frame.data[1] =
        static_cast<uint8_t>(state.direction_requested.load(std::memory_order_relaxed));

    // array
    steering_requests_frame.data[2] = static_cast<uint8_t>(
        state.array_contactors_requested_closed.load(std::memory_order_relaxed));

    // horn
    steering_requests_frame.data[3] =
        static_cast<uint8_t>(state.horn_requested_on.load(std::memory_order_relaxed));

    // headlights
    steering_requests_frame.data[4] =
        static_cast<uint8_t>(state.headlights_requested_on.load(std::memory_order_relaxed));

    // regen breaking strength
    steering_requests_frame.data[5] = 0;

    // pwr/eco request
    steering_requests_frame.data[6] =
        static_cast<uint8_t>(state.mc_power_mode_requested.load(std::memory_order_relaxed));

    // cc mph
    steering_requests_frame.data[7] = 0;

    can_device.send(steering_requests_frame);
}

// TODO: some indicator for the button 7 light
// TODO: important info not done: cruise control, regenerative breaking
void processScreen()
{
    static uint8_t old_speed = state.car_speed.load(std::memory_order_relaxed);
    if (uint8_t speed = state.car_speed.load(std::memory_order_relaxed); speed != old_speed)
    {
        drawSpeed(speed);
        old_speed = speed;
    }

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

    static bool old_headlights_status =
        state.headlights_requested_on.load(std::memory_order_relaxed);
    if (bool headlights_status = state.headlights_requested_on.load(std::memory_order_relaxed);
        old_headlights_status != headlights_status)
    {
        drawHeadlightsStatus(headlights_status);
        old_headlights_status = headlights_status;
    }

    static auto old_killed_status = state.killed_status.load(std::memory_order_relaxed);
    if (auto killed_status = state.killed_status.load(std::memory_order_relaxed);
        old_killed_status != killed_status)
    {
        drawKillStatus(killed_status);
        old_killed_status = killed_status;
    }
}
void processHornButton()
{
    // -- HORN POLLING LOGIC --
    bool horn_pressed = (HAL_GPIO_ReadPin(HORN_PORT, HORN_PIN) == GPIO_PIN_RESET);
    state.horn_requested_on.store(horn_pressed);
}

}  // namespace steering
