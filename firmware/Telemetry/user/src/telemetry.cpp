//
// Created by justin on 2/25/26.
//

#include "Steering_wheel_buttons.hpp"
#include "can.h"
#include "can_protocol.h"
#include "telem_state.h"

#include <atomic>

namespace
{

flare_can::TurnSignals old_turn_signals{};
inline sg::Button kill_switch_button(KILL_SW_INPUT_GPIO_Port, KILL_SW_INPUT_Pin, 50, GPIO_PIN_SET);
inline void killSwitchButtonInit()
{
    // when only normal press is registered, oon any button press it should call the callback
    kill_switch_button.RegisterNormalPressCallback(
        []() { killed_status.store(flare_can::CarKilledStatus::DEAD, std::memory_order_relaxed); });
    sg::Button::InitButtons();
}

}  // namespace

namespace telem
{

void init()
{
    canInit();
    killSwitchButtonInit();
}

void sendKillFrame()
{
    sg::CANFrame kill_frame{0x010,
                            sg::CANFrameIDType::STANDARD,
                            sg::CANFrameRTRMode::DATA,
                            sg::CANFrameLen::BYTES_1,
                            0,
                            {}};
    // send current kill switch message
    kill_frame.data[0] = static_cast<uint8_t>(killed_status.load(std::memory_order_relaxed));
    can_device.send(kill_frame);

    // Temporary test of motor controller
    sg::CANFrame mitsuba_request{0x08F89540,
                                 sg::CANFrameIDType::EXTENDED,
                                 sg::CANFrameRTRMode::REMOTE,
                                 sg::CANFrameLen::BYTES_1,
                                 0,
                                 {}};
    // send current kill switch message
    mitsuba_request.data[0] = 0b00000111;
    can_device.send(mitsuba_request);
}

void processLightsOutputs()
{
    // kill switch led logic
    if (killed_status.load(std::memory_order_relaxed) == flare_can::CarKilledStatus::DEAD)
    {
        HAL_GPIO_TogglePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin);
    }

    // turn signal led logic
    // if status changed, turn the lights off
    if (flare_can::TurnSignals new_turn_signals =
            turn_signals_status.load(std::memory_order_relaxed);
        new_turn_signals != old_turn_signals)
    {
        HAL_GPIO_WritePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin, GPIO_PIN_RESET);
        // TODO: turn off middle one here when we get it
        old_turn_signals = new_turn_signals;
    }
    // toggle correct led's
    switch (old_turn_signals)
    {
        case flare_can::TurnSignals::LEFT:
            HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
            break;
        case flare_can::TurnSignals::RIGHT:
            HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
            break;
        case flare_can::TurnSignals::HAZARDS:
            HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
            HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
            // TODO: toggle middle one here when we get it
            break;
        case flare_can::TurnSignals::OFF:
            break;
        default:
            Error_Handler();
    }
}

}  // namespace telem
