//
// Created by justin on 2/16/26.
//

#ifndef FLAREFIRMWARE_TELEM_STATE_H
#define FLAREFIRMWARE_TELEM_STATE_H

#include <cstdint>

#include "Steering_wheel_buttons.hpp"
#include "can_protocol.h"

#include <atomic>

namespace telem
{

inline std::atomic<flare_can::CarKilledStatus> killed_status{flare_can::CarKilledStatus::ALIVE};
inline sg::Button kill_switch_button(KILL_SW_INPUT_GPIO_Port,
                                     KILL_SW_INPUT_Pin,
                                     50,
                                     GPIO_PIN_RESET);

inline void killSwitchPressedCallback()
{
    killed_status.store(flare_can::CarKilledStatus::DEAD, std::memory_order_relaxed);
}

inline void killSwitchButtonInit()
{
    // when only normal press is registered, oon any button press it should call the callback
    kill_switch_button.RegisterNormalPressCallback(&killSwitchPressedCallback);
}

}  // namespace telem

#endif  //FLAREFIRMWARE_TELEM_STATE_H
