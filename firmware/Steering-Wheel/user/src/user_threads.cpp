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
    ILI9341 display(320, 240);
    display.Init();

    for (;;)
    {
        //240 320
        display.FillRect(50, 50, 50, 50, RGB565_RED);

        display.FillRect(0, 0, 50, 50, RGB565_RED);

        // one of the CAN messages to be recieved and displayed is car_speed{}
        //uint8_t speed = steering_state.car_speed.load();

        // display.DrawText(uint16_t x, uint16_t y, const char* str, uint16_t color);
        // display.DrawText(100, 100, (speed), RGB565_RED);
        display.DrawText(150, 150, "0", RGB565_RED);
        // possible solutions - snprintf normal but more lines of code
        // to_string then .c_str, uses heap, not best practice for embedded
        // itoa, not standard and may not be avaliable


        // display.SetTextColor(uint16_t color);
        // display.SetTextSize(uint8_t size);
        // display.FillScreen(uint16_t color);
        // display.ClearScreen(uint16_t color);
        // also has lines, circles, triangles
        // use draw rect to refresh the lcd to make smoother with things like speed
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