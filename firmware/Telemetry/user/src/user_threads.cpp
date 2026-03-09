#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.h"
#include "main.h"
#include "maxm10s.hpp"
#include "rfd900x.h"
#include "telem_state.h"
#include "telemetry.h"

QueueHandle_t radioTXQueue;

void init_user()
{
    // turn lights off
    HAL_GPIO_WritePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin, GPIO_PIN_RESET);
    // TODO: turn off middle one here when we get it

    telem::init();
    gps().init();

    // Can hold ten standard can frames (2 bytes for ID + 8 data bytes)
    radioTXQueue = xQueueCreate(10, 10);
    if (!radioTXQueue)
    {
        Error_Handler();
    }
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        HAL_GPIO_TogglePin(GPS_LED_GPIO_Port, GPS_LED_Pin);
        HAL_GPIO_TogglePin(RADIO_LED_GPIO_Port, RADIO_LED_Pin);

        telem::processLightsOutputs();

        osDelay(telem::led_toggle_period_ms);
    }
}

void startGPSReadBufferTask_user(void* argument)
{
    for (;;)
    {
        gps().readOutputBuffer();
        telem::queueGPSData();

        osDelay(500);
    }
}

void startGPSParseNMEATask_user(void* argument)
{
    for (;;)
    {
        gps().parseNMEA();

        osDelay(200);
    }
}

extern UART_HandleTypeDef huart2;
void startTXRadioTask_user(void* argument)
{
    // TODO: clean up this initialization
    rfd900SetUartHandle(&huart2);
    rfd900EnterLocalATCommandMode();

    for (;;)
    {
        uint8_t long_frame[10];

        if (xQueueReceive(radioTXQueue, long_frame, 100) != pdTRUE)
        {
            continue;
        }

        uint8_t frame_packet[30];

        frame_packet[0] = 0x02;  // START

        uint8_t pos = 1;

        for (uint8_t i = 0; i < 10; i++)
        {
            uint8_t byte = long_frame[i];

            if (byte == 0x02 || byte == 0x03 || byte == 0x1B)
            {
                frame_packet[pos++] = 0x1B;  // escape
            }

            frame_packet[pos++] = byte;
        }

        frame_packet[pos++] = 0x03;  // END

        rfd900SendData(frame_packet, pos);
    }
}

void startKillSwitchTask_user(void* argument)
{
    for (;;)
    {
        telem::sendKillFrame();
        osDelay(50);
    }
}