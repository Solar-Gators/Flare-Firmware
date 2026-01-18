#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"

void StartDefaultTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}
