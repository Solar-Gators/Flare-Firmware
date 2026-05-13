//
// Created by justin on 2/25/26.
//

#ifndef FLAREFIRMWARE_TELEM_STATE_H
#define FLAREFIRMWARE_TELEM_STATE_H

#include "can_protocol.h"

#include <atomic>

// internal globals
inline std::atomic killed_status{flare_can::CarKilledStatus::ALIVE};
inline std::atomic turn_signals_status{flare_can::TurnSignals::OFF};
inline std::atomic<bool> turn_signals_phase{};
inline std::atomic<bool> brake_state{};

#endif  //FLAREFIRMWARE_TELEM_STATE_H
