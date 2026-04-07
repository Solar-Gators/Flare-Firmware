#pragma once

#include "main.h"

enum class ButtonIndex : uint8_t
{
    LEFT_TURN = 0,
    POWER_MODE = 1,
    ARRAY = 2,
    CC_DEC = 3,
    RIGHT_TURN = 4,
    DIRECTION = 5,
    HORN = 6,  // be careful changing this one cause its light is messed up
    CC_INC
};

#define HORN_PORT BUTTON7_GPIO_Port
#define HORN_PIN BUTTON7_Pin

// button layout
// 1(lt)      5(rt) (both for hazards)
// 2(pwr,eco)    6(fwrd/rev) // prev. 2 was headlights
// 3(array)   7(horn)
// 4(cc down)     8(cc inc) || to en/dis cc press both

// paddles do cc or regen strengths depending on mode maybe

void leftTurnPressedCallback();
void powerModePressedCallback();
void arrayPressedCallback();
void ccDecPressedCallback();
void rightTurnPressedCallback();
void directionPressedCallback();
void hornPressedCallback();
void ccIncPressedCallback();

void initButtons();

void recalculateTurnSignals();
