#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "Steering_wheel_buttons.hpp"
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
        // speed draw
        // sample code to have the speed cycle through values for appearance
        // if (demo_up)
        // {
        //     demo_speed++;
        //     if (demo_speed >= 70) demo_up = false;
        // } else
        // {
        //     demo_speed--;
        //     if (demo_speed == 0) demo_up = true;
        // }
        // uint8_t speed = demo_speed;
        steering::processScreen();
        osDelay(100);
    }
}

void startPollButtons_user(void* argument)
{
    for (;;)
    {
        steering::sendRequestsMessage();
        // TODO: could consolidate these below functions into smt like processButtonLEDs
        steering::processHornButton();
        steering::processTurnAndKill();
        steering::processCC();

        osDelay(30);
    }
}