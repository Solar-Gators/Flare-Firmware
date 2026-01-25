#pragma once
//#include

#include "main.h"
//#include <stm32u5xx_hal.h>
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAX_BUTTONS 8
#define BUTTON_THREAD_STACK_SIZE 256

namespace sg
{

class Button
{
   public:
    Button(GPIO_TypeDef *port, uint16_t pin, uint32_t debounce_time_ms = 50,
           GPIO_PinState default_state = GPIO_PIN_SET, bool initial_toggle_state = false);

    void RegisterNormalPressCallback(void (*callback)(void));
    void RegisterLongPressCallback(void (*callback)(void),
                                   uint32_t long_press_time_ms = 500,
                                   bool initial_toggle_state = false);
    void RegisterDoublePressCallback(void (*callback)(void),
                                     uint32_t double_press_time_ms = 500,
                                     bool initial_toggle_state = false);


    uint32_t GetPin();
    GPIO_PinState ReadPin();
    bool GetToggleState();
    void SetToggleState(bool state);
    bool GetLongToggleState();
    void SetLongToggleState(bool state);
    bool GetDoubleToggleState();
    GPIO_PinState GetDefaultState();

    // Current triggered button
    static inline Button *triggered_button_ = nullptr;

    // Button trigger semaphore
    static inline osSemaphoreId_t button_semaphore_id_ = osSemaphoreNew(1, 0, NULL);

    // Global button list (Fixed array instead of vector)
    static inline Button* button_list_[MAX_BUTTONS] = { nullptr };

   private:

    // Button handler thread definitions
    static void HandleEvent(void *argument);
    static inline uint32_t handle_press_task_buffer_[BUTTON_THREAD_STACK_SIZE];
    static inline StaticTask_t handle_press_task_tcb_;

    static constexpr osThreadAttr_t handle_press_task_attributes_ = {
        .name = "Steering Button Thread",
        .attr_bits = osThreadDetached,
        .cb_mem = &handle_press_task_tcb_,
        .cb_size = sizeof(handle_press_task_tcb_),
        .stack_mem = handle_press_task_buffer_,
        .stack_size = sizeof(handle_press_task_buffer_),
        .priority = (osPriority_t)osPriorityAboveNormal,
    };

    // This static initialization spawns the thread automatically
    static inline osThreadId_t handle_press_task_id_ = osThreadNew(HandleEvent, NULL, &handle_press_task_attributes_);

    void DisableInterrupt();
    void EnableInterrupt();
    void ClearInterrupt();

    void PollForLongPress();
    void HandlePress();

    //Peripheral information
    GPIO_TypeDef *port_;             // HAL GPIO port
    uint16_t pin_;                   // Pin number
    uint32_t debounce_time_ms_;      // debounce time in ms
    GPIO_PinState default_state_;    // default pin state
    bool toggle_state_;              // toggle state of button, toggled on single and double press
    bool long_toggle_state_;        // toggle state of button, toggled on long press
    bool double_toggle_state_;      // toggle state of button, toggled on double press

    void (*normal_callback_)(void);  // User-provided callback function
    void (*long_press_callback_)(void) = nullptr;   // User-provided callback function for long press
    void (*double_press_callback_)(void) = nullptr; // User-provided callback function for double press

    uint32_t long_press_time_ms_ = 0;    // time to count as a long press
    uint32_t double_press_time_ms_ = 0; // time between presses

    uint32_t last_press_time_ = 0;       // Last time valid event was recorded on button
};

}