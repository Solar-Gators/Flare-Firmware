//
// Created by justin on 2/24/26.
//

#include "steering.h"

#include "buttons.h"
#include "can.h"
#include "can_protocol.h"
#include "screen.h"
#include "steering_state.h"

// private variables
namespace
{
sg::CANFrame steering_requests_frame{0x064,
                                     sg::CANFrameIDType::STANDARD,
                                     sg::CANFrameRTRMode::DATA,
                                     sg::CANFrameLen::BYTES_8,
                                     0,
                                     {}};

// for screen
uint8_t old_speed = steering::state.car_speed.load(std::memory_order_relaxed);
uint16_t old_supp_batt_mv = steering::state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
auto old_direction = steering::state.actual_direction.load(std::memory_order_relaxed);
uint16_t old_main_batt_cv = steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);
uint16_t old_high_temp_dc = steering::state.high_temp_dc.load(std::memory_order_relaxed);
auto old_array_contactors =
    steering::state.actual_array_contactors_status.load(std::memory_order_relaxed);
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
    // turn signals
    steering_requests_frame.data[0] = static_cast<uint8_t>(state.turn_signals_requested.load());

    // frwrd / reverse
    steering_requests_frame.data[1] = static_cast<uint8_t>(state.direction_requested.load());

    // array
    steering_requests_frame.data[2] =
        static_cast<uint8_t>(state.array_contactors_requested_closed.load());

    // horn
    steering_requests_frame.data[3] = static_cast<uint8_t>(!HAL_GPIO_ReadPin(HORN_PORT, HORN_PIN));

    // headlights
    steering_requests_frame.data[4] = static_cast<uint8_t>(state.headlights_requested_on.load());

    // regen breaking strength
    steering_requests_frame.data[5] = 0;

    // pwr/eco request
    steering_requests_frame.data[6] = static_cast<uint8_t>(state.mc_power_mode_requested.load());

    // cc mph
    steering_requests_frame.data[7] = 0;

    can_device.send(steering_requests_frame);
}

// TODO: some indicator for the button 7 light
// TODO: important info not done: cruise control, regenerative breaking
void processScreen()
{
    if (uint8_t speed = steering::state.car_speed.load(std::memory_order_relaxed);
        speed != old_speed)
    {
        drawSpeed(speed);
        old_speed = speed;
    }

    if (uint16_t sup_batt_mv = steering::state.supp_batt_voltage_mv.load(std::memory_order_relaxed);
        sup_batt_mv != old_supp_batt_mv)
    {
        drawSuppBatt(sup_batt_mv);
        old_supp_batt_mv = sup_batt_mv;
    }

    if (auto direction = steering::state.actual_direction.load(std::memory_order_relaxed);
        direction != old_direction)
    {
        drawDirection(direction);
        old_direction = direction;
    }

    if (uint16_t main_batt_cv =
            steering::state.main_batt_voltage_cv.load(std::memory_order_relaxed);  // centivolts
        main_batt_cv != old_main_batt_cv)
    {
        drawMainBatt(main_batt_cv);
        old_main_batt_cv = main_batt_cv;
    }

    if (uint16_t high_temp_dc =
            steering::state.high_temp_dc.load(std::memory_order_relaxed);  // decicelcius
        old_high_temp_dc != high_temp_dc)
    {
        drawHighTemp(high_temp_dc);
        old_high_temp_dc = high_temp_dc;
    }

    if (auto array_contactors =
            steering::state.actual_array_contactors_status.load(std::memory_order_relaxed);
        old_array_contactors != array_contactors)
    {
        drawArrayContactors(array_contactors);
        old_array_contactors = array_contactors;
    }
}

}  // namespace steering
