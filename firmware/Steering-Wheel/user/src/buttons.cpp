//
// Created by justin on 1/31/26.
//
#include "buttons.h"

#include "Steering_wheel_buttons.hpp"
#include "main.h"
#include "steering_state.h"

#include <array>

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

// TODO: somethibng like this so we stop using magiv number indicies down in this file
/*
sg::Button& left_turn_button = buttons[0];
*/

// left turn
void button1PressedCallback()
{
    // LED blinking implemented in user_threads.cpp
    // if hazards on and one pressed -> deactivate both
    if (state.turn_signals_requested.load() == flare_can::TurnSignals::HAZARDS)
    {
        buttons[0].SetToggleState(false);
        buttons[4].SetToggleState(false);
    }
    recalculateTurnSignals();
}
// right turn
void button5PressedCallback()
{
    // LED blinking implemented in user_threads.cpp
    // if hazards on and one pressed -> deactivate both
    if (state.turn_signals_requested.load() == flare_can::TurnSignals::HAZARDS)
    {
        buttons[0].SetToggleState(false);
        buttons[4].SetToggleState(false);
    }
    recalculateTurnSignals();
}

// power/eco
void button2PressedCallback()
{
    if (buttons[1].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, GPIO_PIN_SET);
        state.mc_power_mode_requested.store(flare_can::MCPowerMode::POWER);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin, GPIO_PIN_RESET);
        state.mc_power_mode_requested.store(flare_can::MCPowerMode::ECO);
    }
}

// array
void button3PressedCallback()
{
    if (buttons[2].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin, GPIO_PIN_SET);
        state.array_contactors_requested_closed.store(true);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin, GPIO_PIN_RESET);
        state.array_contactors_requested_closed.store(false);
    }
}

// frwd / rev
void button6PressedCallback()
{
    if (buttons[5].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, GPIO_PIN_SET);
        state.direction_requested.store(flare_can::Direction::FORWARD);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, GPIO_PIN_RESET);
        state.direction_requested.store(flare_can::Direction::REVERSE);
    }
}

// horn, should just poll for this one, not a toggle ideally
void button7PressedCallback()
{
    // poll logic in user_threads
    // don't toggle this light, this light is messing with other stuff i think its hardware issues
    // update: still messing with b8, keep this light off
}

// cc-
void button4PressedCallback()
{
    // Directly control LED first for debugging
    if (buttons[3].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_RESET);
    }

    // Then handle CC functionality
    if (uint8_t current_val = state.cc_mph_requested.load(); current_val > 1)
    {
        state.cc_mph_requested.store(current_val - 1);
    }

    // Check for both buttons pressed (simplified)
    if (HAL_GPIO_ReadPin(BUTTON8_GPIO_Port, BUTTON8_Pin) == GPIO_PIN_RESET)
    {
        bool current_state = state.is_cc_on.load();
        state.is_cc_on.store(!current_state);
    }
}

// cc+
void button8PressedCallback()
{
    // Directly control LED first for debugging
    if (buttons[7].GetToggleState())
    {
        HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_RESET);
    }

    // Then handle CC functionality
    if (uint8_t current_val = state.cc_mph_requested.load(); current_val < 99)
    {
        state.cc_mph_requested.store(current_val + 1);
    }

    // Check for both buttons pressed (simplified)
    if (HAL_GPIO_ReadPin(BUTTON4_GPIO_Port, BUTTON4_Pin) == GPIO_PIN_RESET)
    {
        bool current_state = state.is_cc_on.load();
        state.is_cc_on.store(!current_state);
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
    sg::Button::InitButtons();
}

void recalculateTurnSignals()
{
    bool left = buttons[0].GetToggleState();
    bool right = buttons[4].GetToggleState();

    if (left && right)
        state.turn_signals_requested.store(flare_can::TurnSignals::HAZARDS);
    else if (left)
        state.turn_signals_requested.store(flare_can::TurnSignals::LEFT);
    else if (right)
        state.turn_signals_requested.store(flare_can::TurnSignals::RIGHT);
    else
        state.turn_signals_requested.store(flare_can::TurnSignals::OFF);
}
