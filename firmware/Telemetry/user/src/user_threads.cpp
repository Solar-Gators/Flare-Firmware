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

        telem::processLightsOutputs();

        osDelay(telem::led_toggle_period_ms);
    }
}

void startGPSReadBufferTask_user(void* argument)
{
    for (;;)
    {
        gps().readOutputBuffer();
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
    rfd900SetUartHandle(&huart2);

    rfd900EnterLocalATCommandMode();
    uint8_t resp[256];
    uint16_t len;
    rfd900Read(resp, sizeof(resp), &len, 500);

    if (rfd900GetLocalFirmwareData(0) == HAL_OK)
    {
        if (rfd900Read(resp, sizeof(resp), &len, 500) == HAL_OK)
        {
            volatile int x = 4;
            //printf("Received (%d bytes): %s\r\n", len, resp);
        }
        else
        {
            volatile int x = 4;
            //printf("No response\r\n");
        }
    }

    volatile HAL_StatusTypeDef check = rfd900EnterLocalATCommandMode();
    volatile HAL_StatusTypeDef status = rfd900RebootLocalRadio();  //WORKS
    status = rfd900GetLocalRegisterValue(10);                      //WORKS
    volatile HAL_StatusTypeDef t = rfd900ExitLocalATCommandMode();
    status = rfd900ResetLocalParameters();

    //frame.id_type = sg::CANFrameIDType::STANDARD;
    //frame.len = sg::CANFrameLen::BYTES_8;
    //frame.rtr_mode = sg::CANFrameRTRMode::DATA;
    //frame.timestamp = 10482;
    //frame.can_id = 0x20;

    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

    while (1)
    {
        uint8_t frame_packet[30];

        frame_packet[0] = 0x02;  // START
        frame_packet[1] = 0x08;
        frame_packet[2] = 0x00;

        uint8_t pos = 3;

        for (uint8_t i = 0; i < 8; i++)
        {
            uint8_t byte = data[i];

            if (byte == 0x02 || byte == 0x03 || byte == 0x1B)
            {
                frame_packet[pos++] = 0x1B;  // escape
            }

            frame_packet[pos++] = byte;
        }

        frame_packet[pos++] = 0x03;  // END

        volatile HAL_StatusTypeDef status = rfd900SendData(frame_packet, pos);

        osDelay(1000);
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