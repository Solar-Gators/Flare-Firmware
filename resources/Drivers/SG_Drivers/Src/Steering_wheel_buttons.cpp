#include "Steering_wheel_buttons.hpp"

namespace sg
{

Steering_wheel_buttons::Steering_wheel_buttons(
    GPIO_TypeDef *port,
    uint16_t pin,
    uint32_t debounce_time_ms,
    GPIO_PinState default_state,
    bool initial_toggle_state)
    : port(port),
      pin(pin),
      debounce_time_ms(debounce_time_ms),
      default_state(default_state),
      toggle_state(initial_toggle_state),
      normal_callback(nullptr),
      last_press_time(0)
{

    // Add button to global list
    for (uint8_t i = 0; i < MAX_BUTTONS; i++) {
        if (button_list[i] == nullptr) {
            button_list[i] = this;
            break;
        }
    }

    if (button_semaphore == nullptr) {
        button_semaphore = osSemaphoreNew(1, 0, nullptr);
    }

    // Enable GPIO interrupt for this button
    EnableInterrupt();
}


uint32_t Steering_wheel_buttons::GetPin(){
    return pin;
}

GPIO_PinState Steering_wheel_buttons::ReadPin(){
    return HAL_GPIO_ReadPin(port, pin);
}

bool Steering_wheel_buttons::GetToggleState(){
    return toggle_state;
}

void Steering_wheel_buttons::SetToggleState(bool state){
    toggle_state = state;
}

GPIO_PinState Steering_wheel_buttons::GetDefaultState(){
    return default_state;
}


}