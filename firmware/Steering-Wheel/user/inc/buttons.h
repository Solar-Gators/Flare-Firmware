#pragma once

#include "main.h"

enum class ButtonIndex : uint8_t
{
    LEFT_TURN = 0,
    POWER_MODE = 1,
    ARRAY = 2,
    FAN = 3,
    RIGHT_TURN = 4,
    DIRECTION = 5,
    HORN = 6,  // be careful changing this one cause its light is messed up
    CC = 7 // I don't think this led is working either
};

#define HORN_PORT BUTTON7_GPIO_Port
#define HORN_PIN BUTTON7_Pin
#define FAN_PORT BUTTON4_GPIO_Port
#define FAN_PIN BUTTON4_Pin
#define REGEN_PLUS_PORT REGEN_INCREASE_GPIO_Port
#define REGEN_PLUS_PIN REGEN_INCREASE_Pin
#define REGEN_MINUS_PORT REGEN_DECREASE_GPIO_Port
#define REGEN_MINUS_PIN REGEN_DECREASE_Pin

// button layout
// 1(lt)      5(rt) (both for hazards)
// 2(pwr,eco)    6(fwrd/rev) // prev. 2 was headlights
// 3(array)   7(horn)
// 4(fan)     8(N/A) || to en/dis cc press both 4 and 8

// paddles do regen strength

void leftTurnPressedCallback();
void powerModePressedCallback();
void arrayPressedCallback();
void fanPressedCallback();
void rightTurnPressedCallback();
void directionPressedCallback();
void hornPressedCallback();
void ccPressedCallback();

void initButtons();

void recalculateTurnSignals();
