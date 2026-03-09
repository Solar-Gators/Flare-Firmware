//
// Created by justin on 2/25/26.
//

#include "Steering_wheel_buttons.hpp"
#include "can.h"
#include "can_protocol.h"
#include "main.h"
#include "maxm10s.hpp"
#include "telem_state.h"
#include "user_threads.hpp"

#include <atomic>

namespace
{

sg::Button kill_switch_button(KILL_SW_INPUT_GPIO_Port, KILL_SW_INPUT_Pin, 50, GPIO_PIN_SET);
void killSwitchButtonInit()
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
}

void sendSpeedFrame()
{
    sg::CANFrame speed_frame{0x0A0,
                             sg::CANFrameIDType::STANDARD,
                             sg::CANFrameRTRMode::DATA,
                             sg::CANFrameLen::BYTES_1,
                             0,
                             {}};
    speed_frame.data[0] = static_cast<uint8_t>(std::round(gps().getSpeed()));
    can_device.send(speed_frame);
}

void processLightsOutputs()
{
    static flare_can::TurnSignals turn_signals{};

    // kill switch led logic
    if (killed_status.load(std::memory_order_relaxed) == flare_can::CarKilledStatus::DEAD)
    {
        HAL_GPIO_TogglePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin);
    }

    // turn signal led logic
    static bool left_light{};
    static bool right_light{};
    static bool middle_light{};

    // if status changed, turn the lights off
    if (flare_can::TurnSignals new_turn_signals =
            turn_signals_status.load(std::memory_order_relaxed);
        new_turn_signals != turn_signals)
    {
        HAL_GPIO_WritePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin, GPIO_PIN_RESET);
        // TODO: turn off middle one here when we get it
        turn_signals = new_turn_signals;
    }
    // toggle correct led's
    switch (turn_signals)
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
            break;
        case flare_can::TurnSignals::OFF:
            break;
        default:
            Error_Handler();
    }
}

void queueGPSData()
{
    MaxM10S::Position coords = gps().getPosition();
    float speed_kmh = gps().getSpeed();
    uint8_t num_sats = gps().getNumSatellites();

    uint8_t frame[10];

    frame[0] = 0x20;
    frame[1] = 0x00;
    memcpy(frame + 2, &coords.latitude_deg, 8);
    xQueueSend(radioTXQueue, &frame, pdMS_TO_TICKS(100));

    frame[0] = 0x20;
    frame[1] = 0x01;
    memcpy(frame + 2, &coords.longitude_deg, 8);
    xQueueSend(radioTXQueue, &frame, pdMS_TO_TICKS(100));

    frame[0] = 0x20;
    frame[1] = 0x02;
    memcpy(frame + 2, &speed_kmh, 4);
    frame[6] = num_sats;
    frame[7] = 0;
    frame[8] = 0;
    frame[9] = 0;
    xQueueSend(radioTXQueue, &frame, pdMS_TO_TICKS(100));
}

}  // namespace telem
