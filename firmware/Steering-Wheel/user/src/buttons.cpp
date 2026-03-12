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
        state.direction_requested.store(flare_can::Direction::REVERSE);  // light on when rev
    }
    else
    {
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin, GPIO_PIN_RESET);
        state.direction_requested.store(flare_can::Direction::FORWARD);  // light off when fwd
    }
}

// horn, should just poll for this one, not a toggle ideally
void button7PressedCallback()
{
    // poll logic in user_threads
    // don't toggle this light, this light is messing with other stuff i think its hardware issues
    // update: still messing with b8, keep this light off
}

// TODO: for CC buttons make it so holding will go in multiples of 5
// cc- button
void button4PressedCallback()
{
    bool b4_pressed = (HAL_GPIO_ReadPin(BUTTON4_GPIO_Port, BUTTON4_Pin) == GPIO_PIN_RESET);
    bool b8_pressed = (HAL_GPIO_ReadPin(BUTTON8_GPIO_Port, BUTTON8_Pin) == GPIO_PIN_RESET);

    if (b4_pressed && b8_pressed)
    {
        // toggle cc state (both pressed)
        bool current_state = state.is_cc_on.load();
        state.is_cc_on.store(!current_state);

        if (!current_state)
        {
            // if turning on, set the cc to be the currect speed
            uint8_t current_speed = state.car_speed.load();
            if (current_speed >= 1 && current_speed <= 99)
            {
                state.cc_mph_requested.store(current_speed);
            }
        }

        // blink led to alert entering/exiting cc
        // TODO: evaluate this, migth not be needed and blocking operation
        for (int i = 0; i < 3; i++)
        {
            HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_SET);
            HAL_Delay(150);
            HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin, GPIO_PIN_RESET);
            HAL_Delay(50);
        }
        return;
    }

    // normal dec
    if (b4_pressed)
    {
        uint8_t current_val = state.cc_mph_requested.load();
        if (current_val > 1)
        {
            state.cc_mph_requested.store(current_val - 1);
        }
    }
}

// cc+ button
void button8PressedCallback()
{
    // Read the current states of both buttons
    bool b4_pressed = (HAL_GPIO_ReadPin(BUTTON4_GPIO_Port, BUTTON4_Pin) == GPIO_PIN_RESET);
    bool b8_pressed = (HAL_GPIO_ReadPin(BUTTON8_GPIO_Port, BUTTON8_Pin) == GPIO_PIN_RESET);

    if (b4_pressed && b8_pressed)
    {
        // if both being pressed return (don't inc)
        return;
    }

    // normal inc
    if (b8_pressed)
    {
        uint8_t current_val = state.cc_mph_requested.load();
        if (current_val < 99)
        {
            state.cc_mph_requested.store(current_val + 1);
        }
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
    // TODO: might want to consider when left signal on, then press right signal -> it goes to right signal instead of hazards.
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
