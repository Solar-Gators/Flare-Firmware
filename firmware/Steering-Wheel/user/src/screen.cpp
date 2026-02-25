//
// Created by justin on 2/24/26.
//

#include "screen.h"

#include "ILI9341.hpp"
#include "steering_state.h"

#include <array>
#include <string>

namespace
{
// private screen variables
static ILI9341 display(240, 320);
static std::array<char, 16> text_buffer{};
// const screen variables
static constexpr uint16_t text_color{RGB565_BLUE};
static constexpr uint16_t background_color{RGB565_ORANGE};
}  // namespace

namespace steering
{

void initScreen()
{
    // Top Right (0,0)
    // Bottom Right (240, 0)
    // Top Left (0, 320)
    // Bottom Left (240, 320)
    // Items build from top right to bottom left
    display.Init();
    display.SetRotation(3);
    display.SetTextSize(2);
    display.ClearScreen(background_color);

    drawLabels();
    drawSpeed(state.car_speed.load(std::memory_order_relaxed));
    drawSuppBatt(state.supp_batt_voltage_mv.load(std::memory_order_relaxed));
    drawDirection(state.actual_direction.load(std::memory_order_relaxed));
    drawMainBatt(state.main_batt_voltage_cv.load(std::memory_order_relaxed));
    drawHighTemp(state.high_temp_dc.load(std::memory_order_relaxed));
    drawArrayContactors(state.actual_array_contactors_status.load(std::memory_order_relaxed));
}

void drawLabels()
{
    display.DrawText(5, 20, "Speed: ", text_color);
    display.DrawText(5, 40, "Sup volt: ", text_color);
    display.DrawText(5, 60, "Direction: ", text_color);
    display.DrawText(5, 80, "Main volt: ", text_color);
    display.DrawText(5, 100, "High temp: ", text_color);
    display.DrawText(5, 120, "Array Cont: ", text_color);
}

void drawSpeed(uint8_t mph)
{
    display.FillRect(80, 20, 320 - 80, 20, background_color);
    snprintf(text_buffer.data(), sizeof(text_buffer), "%lu", static_cast<unsigned long>(mph));
    display.DrawText(80, 20, text_buffer.data(), text_color);
}

void drawSuppBatt(uint16_t millivolts)
{
    display.FillRect(115, 40, 320 - 115, 20, background_color);
    uint16_t whole = millivolts / 1000;
    uint16_t frac = (millivolts % 1000);
    snprintf(text_buffer.data(),
             sizeof(text_buffer),
             "%lu.%02lu",
             static_cast<unsigned long>(whole),
             static_cast<unsigned long>(frac));
    display.DrawText(115, 40, text_buffer.data(), text_color);
}

void drawDirection(flare_can::Direction direction)
{
    display.FillRect(130, 60, 320 - 130, 20, background_color);
    if (static_cast<int>(direction) == 1)
        display.DrawText(130, 60, "Forward", text_color);
    else if (static_cast<int>(direction) == 0)
        display.DrawText(130, 60, "Backward", text_color);
    else
        display.DrawText(130, 60, "ERROR", text_color);
}

void drawMainBatt(uint16_t centivolts)
{
    display.FillRect(130, 80, 320 - 130, 20, background_color);
    uint16_t whole = centivolts / 100;
    uint16_t frac = (centivolts % 100);
    snprintf(text_buffer.data(),
             sizeof(text_buffer),
             "%lu.%02lu",
             static_cast<unsigned long>(whole),
             static_cast<unsigned long>(frac));
    display.DrawText(130, 80, text_buffer.data(), text_color);
}

void drawHighTemp(uint16_t decicelcius)
{
    display.FillRect(145, 100, 320 - 140, 20, background_color);
    uint16_t whole = decicelcius / 10;
    uint16_t frac = (decicelcius % 10);
    snprintf(text_buffer.data(),
             sizeof(text_buffer),
             "%lu.%lu",
             static_cast<unsigned long>(whole),
             static_cast<unsigned long>(frac));
    display.DrawText(145, 100, text_buffer.data(), text_color);
}

void drawArrayContactors(flare_can::ArrayContactors contactors)
{
    display.FillRect(140, 120, 320 - 140, 20, background_color);
    if (contactors == flare_can::ArrayContactors::MAIN_CLOSED)
        display.DrawText(140, 120, "Closed", text_color);
    else
        display.DrawText(140, 120, "Open", text_color);
}

}  // namespace steering
