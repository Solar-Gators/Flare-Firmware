//
// Created by justin on 2/25/26.
//

#include "telemetry.h"

#include "Steering_wheel_buttons.hpp"
#include "can.h"
#include "can_protocol.h"
#include "main.h"
#include "maxm10s.hpp"
#include "radio.h"
#include "telem_state.h"
#include "user_threads.hpp"

#include <atomic>

extern "C" I2C_HandleTypeDef hi2c1;

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

MaxM10S gps(&hi2c1);
}  // namespace

namespace telem
{

void init()
{
    canInit();
    killSwitchButtonInit();
    gps.init();
    radioInit();
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
    speed_frame.data[0] = static_cast<uint8_t>(std::round(gps.getSpeed()));
    can_device.send(speed_frame);
}

void processLightsOutputs()
{
    static flare_can::TurnSignals turn_signals{};
    static uint32_t last_toggle_tick{};

    uint32_t current_tick = HAL_GetTick();
    if (current_tick - last_toggle_tick < led_toggle_period_ms)
    {
        return;  // rest of code toggles
    }

    last_toggle_tick = current_tick;

    // kill switch led logic
    if (killed_status.load(std::memory_order_relaxed) == flare_can::CarKilledStatus::DEAD)
    {
        led_toggle_period_ms = 250;
        HAL_GPIO_TogglePin(STROBE_LIGHT_CTRL_GPIO_Port, STROBE_LIGHT_CTRL_Pin);
        HAL_GPIO_TogglePin(REAR_LEFT_LIGHT_CTRL_GPIO_Port, REAR_LEFT_LIGHT_CTRL_Pin);
        HAL_GPIO_TogglePin(REAR_RIGHT_LIGHT_CTRL_GPIO_Port, REAR_RIGHT_LIGHT_CTRL_Pin);
    }
    else
    {
        // if status changed, turn the lights off
        if (flare_can::TurnSignals new_turn_signals =
                turn_signals_status.load(std::memory_order_relaxed);
            new_turn_signals != turn_signals)
        {
            HAL_GPIO_WritePin(
                REAR_LEFT_LIGHT_CTRL_GPIO_Port, REAR_LEFT_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(
                REAR_RIGHT_LIGHT_CTRL_GPIO_Port, REAR_RIGHT_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
            // TODO: turn off middle one here when we get it
            turn_signals = new_turn_signals;
        }
        // toggle correct led's
        switch (turn_signals)
        {
            case flare_can::TurnSignals::LEFT:
                HAL_GPIO_TogglePin(REAR_LEFT_LIGHT_CTRL_GPIO_Port, REAR_LEFT_LIGHT_CTRL_Pin);
                break;
            case flare_can::TurnSignals::RIGHT:
                HAL_GPIO_TogglePin(REAR_RIGHT_LIGHT_CTRL_GPIO_Port, REAR_RIGHT_LIGHT_CTRL_Pin);
                break;
            case flare_can::TurnSignals::HAZARDS:
                HAL_GPIO_TogglePin(REAR_LEFT_LIGHT_CTRL_GPIO_Port, REAR_LEFT_LIGHT_CTRL_Pin);
                HAL_GPIO_TogglePin(REAR_RIGHT_LIGHT_CTRL_GPIO_Port, REAR_RIGHT_LIGHT_CTRL_Pin);
                break;
            case flare_can::TurnSignals::OFF:
                break;
            default:
                Error_Handler();
        }
    }
}

void queueGpsData()
{
    MaxM10S::Position coords = gps.getPosition();
    float speed = gps.getSpeed();
    uint8_t num_sats = gps.getNumSatellites();

    addGpsDataToRadioQueue(coords.latitude_deg, coords.longitude_deg, speed, num_sats);
}

void readGpsData()
{
    gps.readOutputBuffer();
    gps.parseNMEA();
}

void waitAndSendRadioData()
{
    RadioMessage msg = waitForRadioMessageData();
    radioSend(msg);
}

}  // namespace telem
