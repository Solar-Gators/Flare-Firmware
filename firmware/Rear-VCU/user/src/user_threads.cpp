#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

void StartDefaultTask_user(void* argument)
{
    for (;;)
    {
        osDelay(500);
    }
}
