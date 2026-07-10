#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "Steering_wheel_buttons.hpp"
#include "../inc/steering_state.h"
#include "app_freertos.h"
#include "main.h"
#include "steering.h"
#include "watchdog.hpp"

void init_user()
{
    // total init
    steering::init();
}

void startHeartbeatTask_user(void* argument)
{
    sg::Watchdog wdog1;
    wdog1.IWDG_Init();

    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);

        wdog1.Kick();

        steering::sendMitsubaRequestMessage();

        osDelay(steering::mitsuba_request_message_send_period_ms);
        osDelay(500);
    }
}

void startScreenTask_user(void* argument)
{
    sg::Watchdog wdog2;
    wdog2.IWDG_Init();

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

        wdog2.Kick();

        osMutexAcquire(screenMutexHandle, osWaitForever);
        steering::processScreen();
        osMutexRelease(screenMutexHandle);

        osDelay(20);  // screen refresh rate
    }
}

void startPollButtons_user(void* argument)
{
    sg::Watchdog wdog3;
    wdog3.IWDG_Init();

    for (;;)
    {
        steering::processHornButton();
        steering::processTurnSignals();
        steering::processCC();
        steering::processRegen();
        steering::sendRequestsMessage();
        steering::processTimer();

        wdog3.Kick();

        osDelay(20);
    }
}

void startDancingFlareScreenTask_user(void* argument)
{
    sg::Watchdog wdog4;
    wdog4.IWDG_Init();

    for (;;)
    {
        osMutexAcquire(screenMutexHandle, osWaitForever);
        steering::flareDance();
        osMutexRelease(screenMutexHandle);

        wdog4.Kick();
        osDelay(30);
    }
}

void HAL_TIM_PeriodElapsedCallback_user(TIM_HandleTypeDef *htim)
{
    // Ensure the interrupt came from the timer you configured (TIM1)
    if (htim->Instance == TIM1) {
        // Atomic increment of your timer value
        uint32_t val = state.timer_value.load(std::memory_order_relaxed);
        state.timer_value.store(val + 1, std::memory_order_relaxed);
    }
}