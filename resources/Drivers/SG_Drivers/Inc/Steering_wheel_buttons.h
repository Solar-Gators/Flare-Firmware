#pragma once
//#include

#include "main.h"

//#ifndef MAX_BUTTONS
#define MAX_BUTTONS 8
//#endif

//#ifndef BUTTON_THREAD_STACK_SIZE
#define BUTTON_THREAD_STACK_SIZE 256
//#endif

namespace sg
{


// need to handle debounce
// need to toggle/hold states

// prev implementations (car 4 matthew) had like buttons presses as a class instance
// prev also defined semaphores for os and used interrupts

// ok so basically in car 4 they defined a vector of "buttons" like instances of button presses
// and differentiated them on short press, long press, and double press
// also defined max number of different buttons and max num of button threads
// kinda lastly need to store Peripheral information for button press

class Steering_wheel_buttons
{
   public:
    Steering_wheel_buttons(GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time_ms = 50,
           GPIO_PinState default_state = GPIO_PIN_SET, bool initial_toggle_state = false);
   private:

};

}