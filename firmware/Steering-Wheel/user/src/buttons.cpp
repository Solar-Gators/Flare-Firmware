//
// Created by justin on 1/31/26.
//
#include "buttons.h"

#include "Steering_wheel_buttons.hpp"
#include "main.h"
#include "steering_state.h"

#include <array>

namespace steering
{

static std::array<sg::Button, 8> buttons = {{
    {BUTTON1_GPIO_Port, BUTTON1_Pin},
    {BUTTON2_GPIO_Port, BUTTON2_Pin},
    {BUTTON3_GPIO_Port, BUTTON3_Pin},
    {BUTTON4_GPIO_Port, BUTTON4_Pin},
    {BUTTON5_GPIO_Port, BUTTON5_Pin},
    {BUTTON6_GPIO_Port, BUTTON6_Pin},
    {BUTTON7_GPIO_Port, BUTTON7_Pin},
    {BUTTON8_GPIO_Port, BUTTON8_Pin},
}};

// left turn
void button1PressedCallback()
{
    if (buttons[0].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, GPIO_PIN_SET);
    }
    recalculateTurnSignals();
}
// right turn
void button5PressedCallback()
{
    if (buttons[4].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin, GPIO_PIN_RESET);
    }
    recalculateTurnSignals();
}

// headlights
void button2PressedCallback()
{
    if (buttons[1].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, GPIO_PIN_SET);
        steering::state.headlights_requested_on.store(true);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, GPIO_PIN_RESET);
        steering::state.headlights_requested_on.store(false);
    }
}

// array
void button3PressedCallback()
{
    if (buttons[2].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin, GPIO_PIN_SET);
        steering::state.array_contactors_requested_closed.store(true);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin, GPIO_PIN_RESET);
        steering::state.array_contactors_requested_closed.store(false);
    }
}

// frwd / rev
void button6PressedCallback()
{
    if (buttons[5].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, GPIO_PIN_SET);
        steering::state.direction_requested.store(flare_can::Direction::FORWARD);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, GPIO_PIN_RESET);
        steering::state.direction_requested.store(flare_can::Direction::REVERSE);
    }
}

// horn, should just poll for this one, not a toggle ideally
void button7PressedCallback()
{
    // don't toggle this light this light is messing with other stuff i think its hardware issues
    /*
    if (buttons[6].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON7_LED_GPIO_Port, BUTTON7_LED_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON7_LED_GPIO_Port, BUTTON7_LED_Pin, GPIO_PIN_RESET);
    }
    */
}

// cc-
void button4PressedCallback()
{
    if (buttons[3].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_RESET);
    }
}

// cc+
void button8PressedCallback()
{
    if (buttons[7].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_SET);
        steering::state.mc_power_mode_requested.store(flare_can::MCPowerMode::POWER);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_RESET);
        steering::state.mc_power_mode_requested.store(flare_can::MCPowerMode::ECO);
    }
}

void initButtons()
{
    buttons[0].RegisterNormalPressCallback(&button1PressedCallback);
    buttons[1].RegisterNormalPressCallback(&button2PressedCallback);
    buttons[2].RegisterNormalPressCallback(&button3PressedCallback);
    buttons[3].RegisterNormalPressCallback(&button4PressedCallback);
    buttons[4].RegisterNormalPressCallback(&button5PressedCallback);
    buttons[5].RegisterNormalPressCallback(&button6PressedCallback);
    buttons[6].RegisterNormalPressCallback(&button7PressedCallback);
    buttons[7].RegisterNormalPressCallback(&button8PressedCallback);
}

void recalculateTurnSignals()
{
    bool left = buttons[0].GetToggleState();
    bool right = buttons[4].GetToggleState();

    if (left && right)
        steering::state.turn_signals_requested.store(flare_can::TurnSignals::HAZARDS);
    else if (left)
        steering::state.turn_signals_requested.store(flare_can::TurnSignals::LEFT);
    else if (right)
        steering::state.turn_signals_requested.store(flare_can::TurnSignals::RIGHT);
    else
        steering::state.turn_signals_requested.store(flare_can::TurnSignals::OFF);
}

}  // namespace steering
