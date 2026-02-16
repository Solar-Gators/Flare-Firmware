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
    display.SetRotation(3);
    display.ClearScreen(RGB565_ORANGE);
    display.SetTextSize(2);
    //display.FillRect(220, 0, 20, 10, RGB565_GREEN);
    // bool screen_is_switched = false; // starting screen (just an idea for different screens with different info)
    char buf[6];

    for (;;)
    {

        // TODO: create indicators for the right buttons to turn on as the lights on the actual buttons arent working
        // TODO: important info: cruise control, regenerative breaking, car speed, array connectors, sup batt voltage

        uint8_t speed = steering_state.car_speed.load();
        uint16_t sup_batt_volt = steering_state.supp_batt_voltage.load();
        Direction direction = steering_state.direction_requested.load();

        speed++; //remove after testing

        itoa(speed, buf, 10);
        display.DrawText(5, 20, "Speed: ", RGB565_BLUE);
        display.DrawText(75, 20, buf, RGB565_BLUE);

        itoa(sup_batt_volt, buf, 10);
        display.DrawText(5, 40, "Sup volt: ", RGB565_BLUE);
        display.DrawText(115, 40, buf, RGB565_BLUE);

        display.DrawText(5, 60, "Direction: ", RGB565_BLUE);
        if (static_cast<int>(direction) == 1)
            display.DrawText(130, 60, "Forward", RGB565_BLUE);
        else if (static_cast<int>(direction) == 0)
            display.DrawText(130, 60, "Backward", RGB565_BLUE);
        else
            display.DrawText(130, 60, "ERROR", RGB565_BLUE);

        // snprintf causes hard fault
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