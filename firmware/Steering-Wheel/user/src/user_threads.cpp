#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

<<<<<<< Updated upstream
#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "Steering_wheel_buttons.hpp"
#include "main.h"
#include "steering.h"

#include <array>
#include <string>

    void
    init_user()
{
    // total init
    steering::init();
}
=======
#include "main.h"
>>>>>>> Stashed changes

    void
    startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
<<<<<<< Updated upstream

        steering::sendMitsubaRequestMessage();

        osDelay(steering::mitsuba_request_message_send_period_ms);
=======
        osDelay(500);
>>>>>>> Stashed changes
    }
}

void startScreenTask_user(void* argument)
{
    for (;;)
    {
<<<<<<< Updated upstream
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
        osDelay(200);  // screen refresh rate
        // TODO: optimize the refresh rate
    }
}

void startPollButtons_user(void* argument)
{
    for (;;)
    {
        // TODO: could consolidate these below functions into smt like processButtonLEDs
        steering::processHornButton();
        steering::processTurnAndKill();
        steering::processCC();

        steering::sendRequestsMessage();

        osDelay(20);
=======
        osDelay(500);
>>>>>>> Stashed changes
    }
}