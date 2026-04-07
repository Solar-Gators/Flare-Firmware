//
// Created by justin on 1/31/26.
//
#include "buttons.h"

#include <cstddef>

#include "Steering_wheel_buttons.hpp"
#include "main.h"
#include "steering_state.h"

#include <array>

namespace
{
// using ButtonIndex to access this array
std::array<sg::Button, 8> buttons = {{
    {BUTTON1_GPIO_Port, BUTTON1_Pin},
    {BUTTON2_GPIO_Port, BUTTON2_Pin},
    {BUTTON3_GPIO_Port, BUTTON3_Pin},
    {BUTTON4_GPIO_Port, BUTTON4_Pin},
    {BUTTON5_GPIO_Port, BUTTON5_Pin},
    {BUTTON6_GPIO_Port, BUTTON6_Pin},
    {BUTTON7_GPIO_Port, BUTTON7_Pin},
    {BUTTON8_GPIO_Port, BUTTON8_Pin},
}};

constexpr std::size_t button_count = 8;

struct PortPin
{
    GPIO_TypeDef* port;
    uint16_t pin;
};

const std::array<PortPin, button_count> button_pins = {{
    {BUTTON1_GPIO_Port, BUTTON1_Pin},
    {BUTTON2_GPIO_Port, BUTTON2_Pin},
    {BUTTON3_GPIO_Port, BUTTON3_Pin},
    {BUTTON4_GPIO_Port, BUTTON4_Pin},
    {BUTTON5_GPIO_Port, BUTTON5_Pin},
    {BUTTON6_GPIO_Port, BUTTON6_Pin},
    {BUTTON7_GPIO_Port, BUTTON7_Pin},
    {BUTTON8_GPIO_Port, BUTTON8_Pin},
}};

const std::array<PortPin, button_count> led_pins = {{
    {BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin},
    {BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin},
    {BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin},
    {BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin},
    {BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin},
    {BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin},
    {BUTTON7_LED_GPIO_Port, BUTTON7_LED_Pin},
    {BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin},
}};

constexpr std::size_t toIndex(ButtonIndex slot)
{
    return static_cast<std::size_t>(slot);
}

sg::Button& getButton(ButtonIndex slot)
{
    return buttons[toIndex(slot)];
}

const PortPin& getLedPin(ButtonIndex slot)
{
    return led_pins[toIndex(slot)];
}

void setLedState(ButtonIndex slot, GPIO_PinState state)
{
    const auto& led = getLedPin(slot);
    HAL_GPIO_WritePin(led.port, led.pin, state);
}

bool isButtonCurrentlyHeld(ButtonIndex slot)
{
    const auto& input = button_pins[toIndex(slot)];
    return HAL_GPIO_ReadPin(input.port, input.pin) == GPIO_PIN_RESET;
}

// updates led based on the buttons toggle state
void syncLed(ButtonIndex slot)
{
    GPIO_PinState state = getButton(slot).GetToggleState() ? GPIO_PIN_SET : GPIO_PIN_RESET;
    setLedState(slot, state);
}
}  // namespace

void leftTurnPressedCallback()
{
    // LED blinking implemented in user_threads.cpp
    // if hazards on and one pressed -> deactivate both
    if (state.turn_signals_requested.load() == flare_can::TurnSignals::HAZARDS)
    {
        getButton(ButtonIndex::LEFT_TURN).SetToggleState(false);
        getButton(ButtonIndex::RIGHT_TURN).SetToggleState(false);
        setLedState(ButtonIndex::RIGHT_TURN, GPIO_PIN_RESET);
    }

    syncLed(ButtonIndex::LEFT_TURN);

    recalculateTurnSignals();
}

void rightTurnPressedCallback()
{
    // LED blinking implemented in user_threads.cpp
    // if hazards on and one pressed -> deactivate both
    if (state.turn_signals_requested.load() == flare_can::TurnSignals::HAZARDS)
    {
        getButton(ButtonIndex::LEFT_TURN).SetToggleState(false);
        getButton(ButtonIndex::RIGHT_TURN).SetToggleState(false);
        setLedState(ButtonIndex::LEFT_TURN, GPIO_PIN_RESET);
    }

    syncLed(ButtonIndex::RIGHT_TURN);

    recalculateTurnSignals();
}

void powerModePressedCallback()
{
    if (getButton(ButtonIndex::POWER_MODE).GetToggleState())
    {
        setLedState(ButtonIndex::POWER_MODE, GPIO_PIN_SET);
        state.mc_power_mode_requested.store(flare_can::MCPowerMode::POWER);
    }
    else
    {
        setLedState(ButtonIndex::POWER_MODE, GPIO_PIN_RESET);
        state.mc_power_mode_requested.store(flare_can::MCPowerMode::ECO);
    }
}

void arrayPressedCallback()
{
    if (getButton(ButtonIndex::ARRAY).GetToggleState())
    {
        setLedState(ButtonIndex::ARRAY, GPIO_PIN_SET);
        state.array_contactors_requested_closed.store(true);
    }
    else
    {
        setLedState(ButtonIndex::ARRAY, GPIO_PIN_RESET);
        state.array_contactors_requested_closed.store(false);
    }
}

void directionPressedCallback()
{
    if (getButton(ButtonIndex::DIRECTION).GetToggleState())
    {
        setLedState(ButtonIndex::DIRECTION, GPIO_PIN_SET);
        state.direction_requested.store(flare_can::Direction::REVERSE);  // light on when rev
    }
    else
    {
        setLedState(ButtonIndex::DIRECTION, GPIO_PIN_RESET);
        state.direction_requested.store(flare_can::Direction::FORWARD);  // light off when fwd
    }
}

void hornPressedCallback()
{
    // poll logic in user_threads
    // don't toggle this light, this light is messing with other stuff i think its hardware issues
    // update: still messing with b8, keep this light off
}

// TODO: for CC buttons make it so holding will go in multiples of 5
void ccDecPressedCallback()
{
    bool down_pressed = isButtonCurrentlyHeld(ButtonIndex::CC_DEC);
    bool up_pressed = isButtonCurrentlyHeld(ButtonIndex::CC_INC);

    if (down_pressed && up_pressed)
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
        // its okay it will just block thread that handles buttons so no other presses will register if we make it quick its fine
        for (int i = 0; i < 2; i++)
        {
            setLedState(ButtonIndex::CC_DEC, GPIO_PIN_SET);
            setLedState(ButtonIndex::CC_INC, GPIO_PIN_SET);
            osDelay(100);
            setLedState(ButtonIndex::CC_DEC, GPIO_PIN_RESET);
            setLedState(ButtonIndex::CC_INC, GPIO_PIN_RESET);
            osDelay(50);
        }
        return;
    }

    // normal dec
    if (down_pressed)
    {
        uint8_t current_val = state.cc_mph_requested.load();
        if (current_val > 0)
        {
            state.cc_mph_requested.store(current_val - 1);
        }
    }
}

void ccIncPressedCallback()
{
    // Read the current states of both buttons
    bool down_pressed = isButtonCurrentlyHeld(ButtonIndex::CC_DEC);
    bool up_pressed = isButtonCurrentlyHeld(ButtonIndex::CC_INC);

    if (down_pressed && up_pressed)
    {
        // if both being pressed return (don't inc)
        // other callbacks handles this to toggle state
        return;
    }

    // normal inc
    if (up_pressed)
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
    getButton(ButtonIndex::LEFT_TURN).RegisterNormalPressCallback(&leftTurnPressedCallback);
    getButton(ButtonIndex::POWER_MODE).RegisterNormalPressCallback(&powerModePressedCallback);
    getButton(ButtonIndex::ARRAY).RegisterNormalPressCallback(&arrayPressedCallback);
    getButton(ButtonIndex::CC_DEC).RegisterNormalPressCallback(&ccDecPressedCallback);
    getButton(ButtonIndex::RIGHT_TURN).RegisterNormalPressCallback(&rightTurnPressedCallback);
    getButton(ButtonIndex::DIRECTION).RegisterNormalPressCallback(&directionPressedCallback);
    getButton(ButtonIndex::HORN).RegisterNormalPressCallback(&hornPressedCallback);
    getButton(ButtonIndex::CC_INC).RegisterNormalPressCallback(&ccIncPressedCallback);

    sg::Button::InitButtons();
}

void recalculateTurnSignals()
{
    // TODO: might want to consider when left signal on, then press right signal -> it goes to right signal instead of hazards.
    bool left = getButton(ButtonIndex::LEFT_TURN).GetToggleState();
    bool right = getButton(ButtonIndex::RIGHT_TURN).GetToggleState();

    if (left && right)
        state.turn_signals_requested.store(flare_can::TurnSignals::HAZARDS);
    else if (left)
        state.turn_signals_requested.store(flare_can::TurnSignals::LEFT);
    else if (right)
        state.turn_signals_requested.store(flare_can::TurnSignals::RIGHT);
    else
        state.turn_signals_requested.store(flare_can::TurnSignals::OFF);
}
