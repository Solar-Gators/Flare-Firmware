#pragma once
#include <cstdint>

#include <atomic>

enum class ArrayContactors : uint8_t
{
    PRECHARGE_CLOSED,
    MAIN_CLOSED,
    BOTH_OPEN
};

enum class Direction : uint8_t
{
    FORWARD = 1,
    REVERSE = 0
};

enum class MCPowerMode : uint8_t
{
    ECO = 0,
    POWER = 1
};

struct VCUState
{
    std::atomic<uint16_t> throttle{};                 // 0 - 65535
    std::atomic<uint8_t> regen{};                     // 0 - 255
    std::atomic<uint16_t> supp_battery_voltage_mv{};  // millivolts
    std::atomic<bool> mc_enabled;                     // true if enabled
    std::atomic<ArrayContactors> array_contactors{};
    std::atomic<Direction> direction{};
    std::atomic<MCPowerMode> mc_power_mode{};

    std::atomic<uint32_t> can_messages_received{};
    std::atomic<uint32_t> can_messages_sent{};
};

inline VCUState vcu_state;
