//
// Created by justin on 2/25/26.
//

#ifndef FLAREFIRMWARE_TELEM_STATE_H
#define FLAREFIRMWARE_TELEM_STATE_H

#include "can_protocol.h"

#include <atomic>

namespace telem
{

// internal globals
inline std::atomic killed_status{flare_can::CarKilledStatus::ALIVE};
inline std::atomic turn_signals_status{flare_can::TurnSignals::OFF};
inline std::atomic<bool> turn_signals_phase{};
inline std::atomic<bool> brake_state{};

// mppt sensor data
struct MpptInputMeasurements
{
    std::atomic<float> voltage{0.0f};
    std::atomic<float> current{0.0f};
};

struct MpptOutputMeasurements
{
    std::atomic<float> voltage{0.0f};
    std::atomic<float> current{0.0f};
};

struct MpptState
{
    MpptInputMeasurements input;
    MpptOutputMeasurements output;
};

inline MpptState mppt1;
inline MpptState mppt2;
inline MpptState mppt3;

}  // namespace telem

#endif  //FLAREFIRMWARE_TELEM_STATE_H
