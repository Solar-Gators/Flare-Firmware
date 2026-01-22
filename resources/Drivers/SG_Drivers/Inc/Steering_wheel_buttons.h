#pragma once
//#include

#include "main.h"
#include <vector>

//#ifndef MAX_BUTTONS
#define MAX_BUTTONS 8
//#endif

//#ifndef BUTTON_THREAD_STACK_SIZE
#define BUTTON_THREAD_STACK_SIZE 256
//#endif

namespace sg
{
// TODO: need to handle debounce and need to toggle/hold states

class Steering_wheel_buttons
{
   public:
    Steering_wheel_buttons(GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time_ms = 50,
           GPIO_PinState default_state = GPIO_PIN_SET, bool initial_toggle_state = false);

    void RegisterNormalPressCallback(void (*callback)(void));

    uint32_t GetPin();
    GPIO_PinState ReadPin();
    bool GetToggleState();
    void SetToggleState(bool state);
    GPIO_PinState GetDefaultState();

    static inline Steering_wheel_buttons *triggered_button;

    // TODO: define button trigger semaphore

    // TODO: define global button list
    //static inline std::vector<Steering_wheel_buttons*, MAX_BUTTONS> button_list

   private:

    // TODO: Button handler thread definitions

    void DisableInterrupt();
    void EnableInterrupt();
    void ClearInterrupt();

    void HandlePress();

    //Peripheral information
    GPIO_TypeDef *port;             // HAL GPIO port
    uint16_t pin;                   // Pin number
    uint32_t debounce_time_ms;      // debounce time in ms
    GPIO_PinState default_state;    // default pin state
    bool toggle_state;              // toggle state of button

    void (*normal_callback)(void); // User-provided callback function

};

}