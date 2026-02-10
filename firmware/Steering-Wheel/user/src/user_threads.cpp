#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "CanDriver.hpp"
#include "ILI9341.hpp"
#include "buttons.h"
#include "can.h"
#include "main.h"
#include "steering_state.h"

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
    // Top Right (0,0)
    // Bottom Right (240, 0)
    // Top Left (0, 320)
    // Bottom Left (240, 320)
    // Items build from top right to bottom left

    ILI9341 display(240, 320);
    display.Init();
    display.ClearScreen(RGB565_RED);
    //display.FillRect(220, 0, 20, 10, RGB565_GREEN);
    uint8_t speed = steering_state.car_speed.load(); // delete after testing

    for (;;)
    {
        //display.FillRect(0, 0, 20, 10, RGB565_RED);

        // uint8_t speed = steering_state.car_speed.load();
        speed++;

        // TODO: create indicators for the right buttons to turn on as the lights on the actual buttons arent working
        // TODO: important info, such as cruise control and regenerative breaking

        // display.SetTextSize(uint8_t size);
        char buf[4];
        itoa(speed, buf, 10);
        display.DrawText(150, 150, buf, RGB565_GREEN);
        // use fill rect to clear old values

        // maybe different screens

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

        can_device.Send(frame);
        ++steering_state.can_messages_sent;

        osDelay(20);
    }
}