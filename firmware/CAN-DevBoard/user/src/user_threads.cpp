#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "main.h"

extern CanHandle_t hfdcan1;

void StartDefaultTask_user(void* argument)
{
    for (;;)
    {
        osDelay(500);
    }
}
