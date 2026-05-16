
#ifndef FRONTVCU_STATE_H
#define FRONTVCU_STATE_H

#include <cstdint>

#include "can_protocol.h"

#include <atomic>

namespace frontvcu
{

enum class LIGHTS_STATES : uint8_t
{
    OFF = 0,
    ON = 1,
    TURN = 2,
    HAZARDS = 3
};

struct FrontVCUState
{
    static constexpr uint8_t ADC_BUF_LEN = 20;
    // throttle, brake, and cc
    std::atomic<uint16_t> raw_throttle_data{};
    std::atomic<uint16_t> throttle_data{};
    std::atomic<uint8_t> brake_state{};  // 1 = brake pressed
    std::atomic<uint8_t> cc_state{};     // 1 = cc on
    // lights
    std::atomic<flare_can::TurnSignals> turn_signals_status{flare_can::TurnSignals::OFF};
    std::atomic<bool> turn_signals_phase{};
    // horn and fan
    std::atomic<uint8_t> horn_state{};
    std::atomic<uint8_t> fan_state{};
    // fh and lights power
    std::atomic<uint8_t> fh_power_lsb{};
    std::atomic<uint8_t> fh_power_msb{};
    std::atomic<uint8_t> lights_power_lsb{};
    std::atomic<uint8_t> lights_power_msb{};
    // kill car
    std::atomic<flare_can::CarKilledStatus> killed_status{};
};

inline FrontVCUState state;

}  // namespace frontvcu

#endif  //FRONTVCU_STATE_H