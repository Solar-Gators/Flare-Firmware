#pragma once

#include <cstdint>

#include "can_protocol.h"

#include <atomic>

namespace steering
{

struct SteeringState
{
    // sending
    std::atomic<uint8_t> regen_requested{};   // 0 - 255 as percent so 255 = 100% regen strength
    std::atomic<uint8_t> cc_mph_requested{};  // in mph
    std::atomic<bool> array_contactors_requested_closed{};
    std::atomic<bool> horn_requested_on{};
    std::atomic<bool> headlights_requested_on{};
    std::atomic<flare_can::Direction> direction_requested{};
    std::atomic<flare_can::MCPowerMode> mc_power_mode_requested{};
    std::atomic<flare_can::TurnSignals> turn_signals_requested{};

    // recieving
    std::atomic<uint8_t> car_speed{};
    std::atomic<flare_can::ArrayContactors> array_contactors_status{};
    std::atomic<uint16_t> supp_batt_voltage{};
    std::atomic<uint32_t> main_batt_voltage_mv{};
    std::atomic<uint16_t> high_temp_dc{};  // C * 10
};

inline SteeringState state;

}  // namespace steering
