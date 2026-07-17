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

MaxM10S gps(&hi2c1);

// light helpers?
void writeLeft(bool on)
{
    HAL_GPIO_WritePin(REAR_LEFT_LIGHT_CTRL_GPIO_Port,
                      REAR_LEFT_LIGHT_CTRL_Pin,
                      on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void writeRight(bool on)
{
    HAL_GPIO_WritePin(REAR_RIGHT_LIGHT_CTRL_GPIO_Port,
                      REAR_RIGHT_LIGHT_CTRL_Pin,
                      on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void writeBrake(bool on)
{
    HAL_GPIO_WritePin(
        BRAKE_LIGHT_CTRL_GPIO_Port, BRAKE_LIGHT_CTRL_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void writeStrobe(bool on)
{
    HAL_GPIO_WritePin(
        STROBE_LIGHT_CTRL_GPIO_Port, STROBE_LIGHT_CTRL_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
}  // namespace

namespace telem
{

void init()
{
    gps.init();
    radioInit();
    canInit();
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
    static bool kill_latched{};

    if (killed_status.load(std::memory_order_relaxed) == flare_can::CarKilledStatus::DEAD)
    {
        kill_latched = true;
    }

    // Hold kill forever after first assertion.
    if (kill_latched)
    {
        killed_status.store(flare_can::CarKilledStatus::DEAD, std::memory_order_relaxed);
    }

    const bool brake = brake_state.load(std::memory_order_relaxed);
    const auto turn_signals = turn_signals_status.load(std::memory_order_relaxed);

    bool left_on = false;
    bool right_on = false;
    bool brake_on = brake;
    bool strobe_on = false;

    // calculate states then write at the end
    if (kill_latched)
    {
        // flashes at ~85 pulses per minute
        // regulations between 60-120
        static uint32_t last_toggle_tick{};
        bool strobe_phase_on{};

        uint32_t current_tick = HAL_GetTick();
        if (current_tick - last_toggle_tick > 350)
        {
            strobe_phase_on = !strobe_phase_on;
            last_toggle_tick = current_tick;
        }
        strobe_on = strobe_phase_on;
    }

    bool turn_signals_phase_on = turn_signals_phase.load(std::memory_order_relaxed);

    switch (turn_signals)
    {
        // brake_on written to at beginning directly by brake status
        case flare_can::TurnSignals::LEFT:
            left_on = turn_signals_phase_on;
            right_on = brake;
            break;
        case flare_can::TurnSignals::RIGHT:
            right_on = turn_signals_phase_on;
            left_on = brake;
            break;
        case flare_can::TurnSignals::HAZARDS:
            left_on = turn_signals_phase_on;
            right_on = turn_signals_phase_on;
            break;
        case flare_can::TurnSignals::OFF:
            left_on = brake;
            right_on = brake;
            break;
        default:
            Error_Handler();
    }

    writeLeft(left_on);
    writeRight(right_on);
    writeBrake(brake_on);
    writeStrobe(strobe_on);
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
