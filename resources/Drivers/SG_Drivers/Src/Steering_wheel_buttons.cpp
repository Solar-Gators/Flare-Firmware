#include "Steering_wheel_buttons.h"


namespace sg
{

/*Steering_wheel_buttons::Steering_wheel_buttons(GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time_ms = 50,
                                               GPIO_PinState default_state = GPIO_PIN_SET, bool initial_toggle_state = false){

}*/

uint32_t Steering_wheel_buttons::GetPin(){
    return pin;
}

bool Steering_wheel_buttons::GetToggleState(){
    return toggle_state;
}


}