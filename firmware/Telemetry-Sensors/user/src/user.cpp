#include "user.hpp"

#include <stm32l4xx_hal.h>

void startDefaultTask_user(void* argument)
{
    for (;;)
        ;
}

void startMsgTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
        HAL_Delay(1000);
    }
}