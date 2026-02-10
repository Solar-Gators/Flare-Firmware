#pragma once

#include <cstdint>

#include <atomic>

enum class ArrayContactors : uint8_t
{
    BOTH_OPEN = 0,
    PRECHARGE_CLOSED = 1,
    MAIN_CLOSED = 2
};

enum class TurnSignals : uint8_t
{
    OFF = 0,
    LEFT = 1,
    RIGHT = 1,
    HAZARDS = 2
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

struct SteeringState
{
    // sending
    std::atomic<uint8_t> regen_requested{};   // 0 - 255 as percent so 255 = 100% regen strength
    std::atomic<uint8_t> cc_mph_requested{};  // in mph
    std::atomic<bool> array_contactors_requested_closed{};
    std::atomic<bool> horn_requested_on{};
    std::atomic<bool> headlights_requested_on{};
    std::atomic<Direction> direction_requested{};
    std::atomic<MCPowerMode> mc_power_mode_requested{};
    std::atomic<TurnSignals> turn_signals_requested{};

    // recieving
    std::atomic<uint8_t> car_speed{};
    std::atomic<ArrayContactors> array_contactors_status{};
    std::atomic<uint16_t> supp_batt_voltage{};
};

inline SteeringState steering_state;