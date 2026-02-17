#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.h"
#include "can_protocol.h"
#include "main.h"
#include "telem_state.h"

void init_user()
{
    telem::can_init();
}

void startHeartbeatTask_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        HAL_GPIO_TogglePin(RL_CTRL_GPIO_Port, RL_CTRL_Pin);
        HAL_GPIO_TogglePin(RR_CTRL_GPIO_Port, RR_CTRL_Pin);
        HAL_GPIO_TogglePin(STROBE_CTRL_GPIO_Port, STROBE_CTRL_Pin);
        osDelay(500);
    }
}

void startGPSReadBufferTask_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startGPSParseNMEATask_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

void startTXRadioTask_user(void* argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}
void startKillSwitchTask_user(void* argument)
{
    sg::CANFrame kill_frame{0x010,
                            sg::CANFrameIDType::STANDARD,
                            sg::CANFrameRTRMode::DATA,
                            sg::CANFrameLen::BYTES_1,
                            0,
                            {}};

    for (;;)
    {
        // send current kill switch message
        kill_frame.data[0] =
            static_cast<uint8_t>(telem::killed_status.load(std::memory_order_relaxed));
        telem::can_device.send(kill_frame);
        osDelay(50);
    }
}