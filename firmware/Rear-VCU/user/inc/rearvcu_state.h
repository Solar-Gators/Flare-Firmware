#pragma once

#include <cstdint>

#include "can_protocol.h"

#include <atomic>
#include <limits>

struct RearVCUState
{
    std::atomic<uint16_t> throttle_requested{};  // 0 - 65535 from front vcu
    std::atomic<uint8_t> regen_requested{};      // 0 - 255
    std::atomic<uint8_t> cc_mph_requested{};

    std::atomic<uint8_t> car_speed{std::numeric_limits<uint8_t>::max()};

    std::atomic<bool> mc_enabled_requested;  // true if requested enabled
    std::atomic<bool> array_contactors_requested_closed{};
    std::atomic<flare_can::Direction> direction_requested{};
    std::atomic<flare_can::MCPowerMode> mc_power_mode_requested{};

    std::atomic<flare_can::ArrayContactors> array_contactors{flare_can::ArrayContactors::BOTH_OPEN};
};

inline RearVCUState state;
