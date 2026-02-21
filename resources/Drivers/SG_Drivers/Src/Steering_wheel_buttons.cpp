#include "Steering_wheel_buttons.hpp"

namespace sg
{
Button::Button(GPIO_TypeDef* port,
               uint16_t pin,
               uint32_t debounce_time_ms,
               GPIO_PinState default_state,
               bool initial_toggle_state)
    : port_(port),
      pin_(pin),
      debounce_time_ms_(debounce_time_ms),
      default_state_(default_state),
      toggle_state_(initial_toggle_state)
{
    // Add button to global list
    for (int i = 0; i < MAX_BUTTONS; i++)
    {
        if (button_list_[i] == nullptr)
        {
            button_list_[i] = this;
            break;
        }
    }

    last_press_time_ = 0;
    long_press_time_ms_ = 0;
    double_press_time_ms_ = 0;
    long_toggle_state_ = initial_toggle_state;
    double_toggle_state_ = initial_toggle_state;

    EnableInterrupt();
}

void Button::RegisterNormalPressCallback(void (*callback)(void))
{
    normal_callback_ = callback;
}

void Button::RegisterLongPressCallback(void (*callback)(void),
                                       uint32_t long_press_time_ms,
                                       bool initial_toggle_state)
{
    long_press_callback_ = callback;
    long_press_time_ms_ = long_press_time_ms;
    long_toggle_state_ = initial_toggle_state;
}

void Button::RegisterDoublePressCallback(void (*callback)(void),
                                         uint32_t double_press_time_ms,
                                         bool initial_toggle_state)
{
    double_press_callback_ = callback;
    double_press_time_ms_ = double_press_time_ms;
    double_toggle_state_ = initial_toggle_state;
}

uint32_t Button::GetPin()
{
    return pin_;
}

bool Button::GetToggleState()
{
    return toggle_state_;
}

void Button::SetToggleState(bool state)
{
    toggle_state_ = state;
}

bool Button::GetLongToggleState()
{
    return long_toggle_state_;
}

void Button::SetLongToggleState(bool state)
{
    long_toggle_state_ = state;
}

bool Button::GetDoubleToggleState()
{
    return double_toggle_state_;
}

GPIO_PinState Button::GetDefaultState()
{
    return default_state_;
}

GPIO_PinState Button::ReadPin()
{
    return HAL_GPIO_ReadPin(port_, pin_);
}

void Button::HandleEvent(void* argument)
{
    (void) argument;
    while (1)
    {
        osSemaphoreAcquire(button_semaphore_id_, osWaitForever);
        Button* button = triggered_button_;
        if (button == nullptr)
        {
            continue;
        }

        button->DisableInterrupt();
        osDelay(button->debounce_time_ms_);

        if (button->ReadPin() != button->default_state_)
        {
            if (button->long_press_callback_ != nullptr)
            {
                button->PollForLongPress();
            }
            else
            {
                button->HandlePress();
            }
        }

        button->ClearInterrupt();
        button->EnableInterrupt();
    }
}

void Button::PollForLongPress()
{
    for (uint32_t i = 0; i < long_press_time_ms_; i += 25)
    {
        osDelay(25);
        if (ReadPin() == default_state_)
        {
            HandlePress();
            return;
        }
    }

    long_toggle_state_ = !long_toggle_state_;
    if (long_press_callback_)
    {
        long_press_callback_();
    }

    while (ReadPin() != default_state_)
    {
        osDelay(25);
    }
}

void Button::HandlePress()
{
    if (double_press_callback_ != nullptr)
    {
        if (osKernelGetTickCount() - last_press_time_ < double_press_time_ms_)
        {
            double_toggle_state_ = !double_toggle_state_;
            if (double_press_callback_)
            {
                double_press_callback_();
            }
            last_press_time_ = 0;
            toggle_state_ = !toggle_state_;
            return;
        }
    }
    last_press_time_ = osKernelGetTickCount();
    toggle_state_ = !toggle_state_;
    if (normal_callback_)
    {
        normal_callback_();
    }
}

void Button::DisableInterrupt()
{
    IRQn_Type irq;
    switch (pin_)
    {
        case GPIO_PIN_0:
            irq = EXTI0_IRQn;
            break;
        case GPIO_PIN_1:
            irq = EXTI1_IRQn;
            break;
        case GPIO_PIN_2:
            irq = EXTI2_IRQn;
            break;
        case GPIO_PIN_3:
            irq = EXTI3_IRQn;
            break;
        case GPIO_PIN_4:
            irq = EXTI4_IRQn;
            break;
        case GPIO_PIN_5:
            irq = EXTI5_IRQn;
            break;
        case GPIO_PIN_6:
            irq = EXTI6_IRQn;
            break;
        case GPIO_PIN_7:
            irq = EXTI7_IRQn;
            break;
        case GPIO_PIN_8:
            irq = EXTI8_IRQn;
            break;
        case GPIO_PIN_9:
            irq = EXTI9_IRQn;
            break;
        case GPIO_PIN_10:
            irq = EXTI10_IRQn;
            break;
        case GPIO_PIN_11:
            irq = EXTI11_IRQn;
            break;
        case GPIO_PIN_12:
            irq = EXTI12_IRQn;
            break;
        case GPIO_PIN_13:
            irq = EXTI13_IRQn;
            break;
        case GPIO_PIN_14:
            irq = EXTI14_IRQn;
            break;
        case GPIO_PIN_15:
            irq = EXTI15_IRQn;
            break;
        default:
            return;
    }
    HAL_NVIC_DisableIRQ(irq);
}

void Button::EnableInterrupt()
{
    IRQn_Type irq;
    switch (pin_)
    {
        case GPIO_PIN_0:
            irq = EXTI0_IRQn;
            break;
        case GPIO_PIN_1:
            irq = EXTI1_IRQn;
            break;
        case GPIO_PIN_2:
            irq = EXTI2_IRQn;
            break;
        case GPIO_PIN_3:
            irq = EXTI3_IRQn;
            break;
        case GPIO_PIN_4:
            irq = EXTI4_IRQn;
            break;
        case GPIO_PIN_5:
            irq = EXTI5_IRQn;
            break;
        case GPIO_PIN_6:
            irq = EXTI6_IRQn;
            break;
        case GPIO_PIN_7:
            irq = EXTI7_IRQn;
            break;
        case GPIO_PIN_8:
            irq = EXTI8_IRQn;
            break;
        case GPIO_PIN_9:
            irq = EXTI9_IRQn;
            break;
        case GPIO_PIN_10:
            irq = EXTI10_IRQn;
            break;
        case GPIO_PIN_11:
            irq = EXTI11_IRQn;
            break;
        case GPIO_PIN_12:
            irq = EXTI12_IRQn;
            break;
        case GPIO_PIN_13:
            irq = EXTI13_IRQn;
            break;
        case GPIO_PIN_14:
            irq = EXTI14_IRQn;
            break;
        case GPIO_PIN_15:
            irq = EXTI15_IRQn;
            break;
        default:
            return;
    }
    HAL_NVIC_SetPriority(irq, 5, 0);
    HAL_NVIC_EnableIRQ(irq);
}

void Button::ClearInterrupt()
{
    __HAL_GPIO_EXTI_CLEAR_FLAG(pin_);
}

}  // namespace sg

extern "C" void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    for (int i = 0; i < MAX_BUTTONS; i++)
    {
        sg::Button* button = sg::Button::button_list_[i];
        if (button == nullptr)
        {
            break;
        }
        if (button->GetPin() == GPIO_Pin && button->ReadPin() != button->GetDefaultState())
        {
            sg::Button::triggered_button_ = button;
            osSemaphoreRelease(sg::Button::button_semaphore_id_);
            break;
        }
    }
}
