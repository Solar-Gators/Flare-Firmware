#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "buttons.h"
#include "can.h"
#include "main.h"
#include "steering_state.h"

void init_user()
{
    can_init();
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
    ILI9341 display(320, 240);
    display.Init();

    for (;;)
    {
        display.FillRect(50, 50, 50, 50, RGB565_RED);

        osDelay(500);
    }
}

void startPollButtons_user(void* argument)
{
    sg::CANFrame frame{0x064,
                       sg::CANFrameIDType::STANDARD,
                       sg::CANFrameRTRMode::DATA,
                       sg::CANFrameLen::BYTES_8,
                       0,
                       {}};

    initButtons();

    for (;;)
    {
        // turn signals
        frame.data[0] = static_cast<uint8_t>(steering_state.turn_signals_requested.load());

        // frwrd / reverse
        frame.data[1] = static_cast<uint8_t>(steering_state.direction_requested.load());

        // array
        frame.data[2] =
            static_cast<uint8_t>(steering_state.array_contactors_requested_closed.load());

        // horn
        frame.data[3] = static_cast<uint8_t>(!HAL_GPIO_ReadPin(HORN_PORT, HORN_PIN));

        // headlights
        frame.data[4] = static_cast<uint8_t>(steering_state.headlights_requested_on.load());

        // regen breaking strength
        frame.data[5] = 0;

        // pwr/eco request
        frame.data[6] = static_cast<uint8_t>(steering_state.mc_power_mode_requested.load());

        // cc mph
        frame.data[7] = 0;

        if (can_device.Send(frame) == HAL_OK)
            ++steering_state.can_messages_sent;

        osDelay(20);
    }
}