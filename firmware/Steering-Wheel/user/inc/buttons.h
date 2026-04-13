#include "Steering_wheel_buttons.hpp"
#include "main.h"

#define HORN_PORT BUTTON7_GPIO_Port
#define HORN_PIN BUTTON7_Pin
#define REGEN_PLUS_PORT REGEN_INCREASE_GPIO_Port
#define REGEN_PLUS_PIN REGEN_INCREASE_Pin
#define REGEN_MINUS_PORT REGEN_DECREASE_GPIO_Port
#define REGEN_MINUS_PIN REGEN_DECREASE_Pin

// button layout
// 1(lt)      5(rt) (both for hazards)
// 2(pwr,eco)    6(fwrd/rev) // prev. 2 was headlights
// 3(array)   7(horn)
// 4(cc down)     8(cc inc) || to en/dis cc press both

// paddles do cc or regen strengths depending on mode maybe

void button1PressedCallback();
void button2PressedCallback();
void button3PressedCallback();
void button4PressedCallback();
void button5PressedCallback();
void button6PressedCallback();
void button7PressedCallback();
void button8PressedCallback();

void initButtons();

void recalculateTurnSignals();
