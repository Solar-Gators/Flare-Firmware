#pragma once
#include <cstdint>

#include <atomic>

enum class ArrayContactors : uint8_t
{
    BOTH_OPEN = 0,
    PRECHARGE_CLOSED = 1,
    MAIN_CLOSED = 2
};

enum class Direction : uint8_t
{
    REVERSE = 0,
    FORWARD = 1
};

enum class MCPowerMode : uint8_t
{
    ECO = 0,
    POWER = 1
};

struct VCUState
{
    std::atomic<uint16_t> throttle_requested{};  // 0 - 65535 from front vcu
    std::atomic<uint8_t> regen_requested{};      // 0 - 255
    std::atomic<uint8_t> cc_mph_requested{};

    std::atomic<uint8_t> car_speed{};

    std::atomic<bool> mc_enabled_requested;  // true if requested enabled
    std::atomic<bool> array_contactors_requested_closed{};
    std::atomic<Direction> direction_requested{};
    std::atomic<MCPowerMode> mc_power_mode_requested{};

    std::atomic<uint32_t> can_messages_received{};
    std::atomic<uint32_t> can_messages_sent{};
};

inline VCUState vcu_state;
