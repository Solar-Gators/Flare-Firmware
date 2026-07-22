#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.h"
#include "main.h"
#include "maxm10s.hpp"
#include "rfd900x.h"
#include "telem_state.h"
#include "telemetry.h"
#include "watchdog.hpp"

void init_user()
{
    // turn lights off
    HAL_GPIO_WritePin(STROBE_LIGHT_CTRL_GPIO_Port, STROBE_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(REAR_LEFT_LIGHT_CTRL_GPIO_Port, REAR_LEFT_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(REAR_RIGHT_LIGHT_CTRL_GPIO_Port, REAR_RIGHT_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BRAKE_LIGHT_CTRL_GPIO_Port, BRAKE_LIGHT_CTRL_Pin, GPIO_PIN_RESET);

    telem::init();
}

void startGPSReadBufferTask_user(void* argument)
{
    sg::Watchdog wdog2;
    wdog2.IWDG_Init();

    for (;;)
    {
        telem::readGpsData();
        wdog2.Kick();
        osDelay(100);
    }
}

void startGPSProcessTask_user(void* argument)
{
    sg::Watchdog wdog3;
    wdog3.IWDG_Init();

    for (;;)
    {
        // queue GPS data for radio
        telem::queueGpsData();
        telem::queueRadioStats();

        // Toggle heartbeat LED
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);

        wdog3.Kick();
        osDelay(500);
    }
}

void startTXRadioTask_user(void* argument)
{
    sg::Watchdog wdog4;
    wdog4.IWDG_Init();

    for (;;)
    {
        // Empty the radio queue and send the data
        telem::waitAndSendRadioData();
        wdog4.Kick();
        // no os delay needed internally calls wait forever on an os queue
    }
}

void startKillSwitchMessageTask_user(void* argument)
{
    sg::Watchdog wdog5;
    wdog5.IWDG_Init();

    for (;;)
    {
        // On e-stop kill the car and require power cycle to reset
        if (HAL_GPIO_ReadPin(KILL_SW_INPUT_GPIO_Port, KILL_SW_INPUT_Pin) == GPIO_PIN_RESET)
        {
            telem::killed_status.store(flare_can::CarKilledStatus::DEAD, std::memory_order_relaxed);
        }
        telem::sendKillFrame();
        wdog5.Kick();
        osDelay(50);
    }
}

void startLightsOutputsTask_user(void* argument)
{
    sg::Watchdog wdog6;
    wdog6.IWDG_Init();

    for (;;)
    {
        // Update light outputs based on the current state
        telem::processLightsOutputs();
        wdog6.Kick();
        osDelay(30);  // tracks time internally
    }
}
void startSpeedMessageTask_user(void* argument)
{
    sg::Watchdog wdog7;
    wdog7.IWDG_Init();

    for (;;)
    {
        // Send speed frame for steering wheel driver display
        telem::sendSpeedFrame();
        wdog7.Kick();
        osDelay(250);
    }
}