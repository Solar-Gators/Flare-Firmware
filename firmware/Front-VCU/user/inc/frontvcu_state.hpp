
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
    // throttle and brake
    std::atomic<uint16_t> throttle_data{};
    std::atomic<uint8_t> brake_state{};
    // lights
    std::atomic<LIGHTS_STATES> lights_req[2]{LIGHTS_STATES::OFF, LIGHTS_STATES::OFF};
    std::atomic<LIGHTS_STATES> lights[2]{LIGHTS_STATES::OFF, LIGHTS_STATES::OFF};
    // horn and fan
    std::atomic<uint8_t> horn_state{};
    std::atomic<uint8_t> fan_state{};
    // fh and lights power
    std::atomic<uint8_t> fh_power_lsb{};
    std::atomic<uint8_t> fh_power_msb{};
    std::atomic<uint8_t> lights_power_lsb{};
    std::atomic<uint8_t> lights_power_msb{};
};

inline FrontVCUState state;

}  // namespace frontvcu

#endif  //FRONTVCU_STATE_H