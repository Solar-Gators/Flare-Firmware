#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.h"
#include "main.h"
#include "maxm10s.hpp"
#include "rfd900x.h"
#include "telem_state.h"
#include "telemetry.h"

void init_user()
{
    // turn lights off
    HAL_GPIO_WritePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin, GPIO_PIN_RESET);
    // TODO: turn off middle one here when we get it

    telem::init();
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        HAL_GPIO_TogglePin(GPS_LED_GPIO_Port, GPS_LED_Pin);
        HAL_GPIO_TogglePin(RADIO_LED_GPIO_Port, RADIO_LED_Pin);
        osDelay(500);
    }
}

void startGPSReadBufferTask_user(void* argument)
{
    for (;;)
    {
        telem::readGpsData();

        osDelay(100);
    }
}

void startGPSProcessTask_user(void* argument)
{
    for (;;)
    {
        telem::queueGpsData();

        osDelay(1000);
    }
}

void startTXRadioTask_user(void* argument)
{
    for (;;)
    {
        telem::waitAndSendRadioData();
        // no os delay needed internally calls wait forever on an os queue
    }
}

void startKillSwitchMessageTask_user(void* argument)
{
    for (;;)
    {
        telem::sendKillFrame();
        osDelay(50);
    }
}
void startLightsOutputsTask_user(void* argument)
{
    for (;;)
    {
        telem::processLightsOutputs();
        osDelay(30);  // tracks time internally
    }
}