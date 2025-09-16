#include "test.hpp"

#include <cmsis_os2.h>
#include <stm32l4xx_hal.h>

#include "main.h"
#include "spi_api.h"

void StartDefaultTask_user(void *argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        osDelay(500);
    }
}
