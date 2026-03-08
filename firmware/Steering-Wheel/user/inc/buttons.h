#include "Steering_wheel_buttons.hpp"
#include "main.h"

#define HORN_PORT BUTTON7_GPIO_Port
#define HORN_PIN BUTTON7_Pin

// button layout
// 1(lt)      5(rt) (both for hazards)
// 2(pwr,eco)    6(fwrd/rev) // prev. 2 was headlights
// 3(array)   7(horn)
// 4(cc down/disable)     8(cc inc/enable)

// paddles do cc or regen strengths depending on mode maybe

namespace steering
{

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

}  // namespace steering