#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "Steering_wheel_buttons.hpp"

void button1PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin);
}
void button2PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin);
}
void button3PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin);
}
void button4PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin);
}
void button5PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin);
}
void button6PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin);
}
void button7PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON7_LED_GPIO_Port, BUTTON7_LED_Pin);
}
void button8PressedCallback(void)
{
    HAL_GPIO_TogglePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin);
}

void StartDefaultTask_user(void* argument)
{
    sg::Button button1(BUTTON1_GPIO_Port, BUTTON1_Pin);
    button1.RegisterNormalPressCallback(&button1PressedCallback);

    sg::Button button2(BUTTON2_GPIO_Port, BUTTON2_Pin);
    button2.RegisterNormalPressCallback(&button2PressedCallback);

    sg::Button button3(BUTTON3_GPIO_Port, BUTTON3_Pin);
    button3.RegisterNormalPressCallback(&button3PressedCallback);

    sg::Button button4(BUTTON4_GPIO_Port, BUTTON4_Pin);
    button4.RegisterNormalPressCallback(&button4PressedCallback);

    sg::Button button5(BUTTON5_GPIO_Port, BUTTON5_Pin);
    button5.RegisterNormalPressCallback(&button5PressedCallback);

    sg::Button button6(BUTTON6_GPIO_Port, BUTTON6_Pin);
    button6.RegisterNormalPressCallback(&button6PressedCallback);

    sg::Button button7(BUTTON7_GPIO_Port, BUTTON7_Pin);
    button7.RegisterNormalPressCallback(&button7PressedCallback);

    sg::Button button8(BUTTON8_GPIO_Port, BUTTON8_Pin);
    button8.RegisterNormalPressCallback(&button8PressedCallback);

    for (;;)
    {
        volatile GPIO_PinState pin = HAL_GPIO_ReadPin(BUTTON8_GPIO_Port, BUTTON8_Pin);
        if (!pin)
        {
            volatile int x = 50;
        }
        osDelay(500);
    }
}
