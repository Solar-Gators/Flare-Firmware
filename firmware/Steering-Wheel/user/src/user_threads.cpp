#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "main.h"
#include "steering.h"

#include <array>
#include <string>

void init_user()
{
    // total init
    steering::init();
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}

void startScreenTask_user(void* argument)
{
    for (;;)
    {
        steering::processScreen();
        osDelay(400);
    }
}

// this should be renamed it just sends a can message
void startPollButtons_user(void* argument)
{
    for (;;)
    {
        steering::sendRequestsMessage();
        osDelay(30);
    }
}