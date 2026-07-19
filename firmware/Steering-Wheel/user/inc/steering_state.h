//
// Created by justin on 2/24/26.
//

#ifndef FLAREFIRMWARE_STEERING_INTERNAL_H
#define FLAREFIRMWARE_STEERING_INTERNAL_H

#include "CanDriver.hpp"
#include "can_protocol.h"

#include <atomic>

struct SteeringState
{
    // sending
    std::atomic<uint8_t>
        regen_percent_requested{};  // 0 - 255 as percent so 255 = 100% regen strength// 0 - 100 as percent so 100 = 100% regen strength
    std::atomic<bool> is_cc_on{};  // determines if cc is active
    std::atomic<bool> array_contactors_requested_closed{};
    std::atomic<bool> horn_requested_on{};
    std::atomic<bool> fan_requested_on{};
    std::atomic<bool> headlights_requested_on{};
    std::atomic<flare_can::Direction> direction_requested{flare_can::Direction::FORWARD};
    std::atomic<flare_can::MCPowerMode> mc_power_mode_requested{};
    std::atomic<flare_can::TurnSignals> turn_signals_requested{};

    // recieving
    std::atomic<uint8_t> car_speed{};
    std::atomic<uint16_t> motor_rpm{};
    std::atomic<flare_can::CarKilledStatus> killed_status{};
    std::atomic<flare_can::ArrayContactors> actual_array_contactors_status{};
    std::atomic<flare_can::Direction> actual_direction{};
    std::atomic<uint16_t> supp_batt_voltage_mv{};
    std::atomic<uint16_t> main_batt_voltage_cv{};  // V * 100
    std::atomic<uint16_t> high_temp_dc{};          // C * 10
    std::atomic<uint8_t> brake_state{};            // 1 = brake pressed

    // debug receive
    std::atomic<uint16_t> throttle_percent_debug{};

    // for screen indicator blinking logic
    std::atomic<bool> left_blink_active{false};
    std::atomic<bool> right_blink_active{false};
    std::atomic<bool> blink_state{false};

    // timer
    std::atomic<bool> timer_requested_on{false};
    std::atomic<uint32_t> timer_value{};

    std::atomic<uint16_t> bms_state{};
    std::atomic<uint32_t> mc_fault_bits{};
    std::atomic<uint8_t> mc_overheat_level{};
};

inline SteeringState state;

void initScreen();

#endif  //FLAREFIRMWARE_STEERING_INTERNAL_H
