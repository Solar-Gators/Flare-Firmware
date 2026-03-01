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
    display.ClearScreen(background_color);

    drawLabels();
    drawSpeed(state.car_speed.load(std::memory_order_relaxed));
    drawSuppBatt(state.supp_batt_voltage_mv.load(std::memory_order_relaxed));
    drawDirection(state.actual_direction.load(std::memory_order_relaxed));
    drawMainBatt(state.main_batt_voltage_cv.load(std::memory_order_relaxed));
    drawHighTemp(state.high_temp_dc.load(std::memory_order_relaxed));
    drawArrayContactors(state.actual_array_contactors_status.load(std::memory_order_relaxed));
    drawKillStatus(state.killed_status.load(std::memory_order_relaxed));
    drawHeadlightsStatus(state.headlights_requested_on.load(std::memory_order_relaxed));
    drawHornStatus(state.horn_requested_on.load(std::memory_order_relaxed));
}

void drawLabels()
{
    display.SetTextSize(2);
    display.DrawText(140, 90, "MPH", RGB565_GRAY);
    display.DrawText(210, 90, "CC", RGB565_GRAY);
    display.DrawText(260, 90, "RB", RGB565_GRAY);
    display.DrawText(5, 130, "MAIN V:", RGB565_BLACK);
    display.DrawText(5, 155, "SUPP V:", RGB565_BLACK);
    display.DrawText(5, 180, "H TEMP:", RGB565_BLACK);
    display.DrawText(5, 205, "ARR CN:", RGB565_BLACK);

    display.SetTextSize(3);
    display.DrawText(10, 40, "R N F", RGB565_BLACK);
    display.DrawText(10, 70, "EC PW", RGB565_BLACK);
}

void drawSpeed(uint8_t mph)
{
    display.SetTextSize(5);
    if (mph > 99)  // >150 means can/sensor error
    {
        snprintf(text_buffer.data(), text_buffer.size(), "ER");
    }
    else
    {
        snprintf(text_buffer.data(), sizeof(text_buffer), "%02lu", static_cast<unsigned long>(mph));
    }
    display.FillRect(130, 45, 100, 40, RGB565_BLUE);
    display.DrawText(130, 50, text_buffer.data(), RGB565_WHITE);
}

void drawSuppBatt(uint16_t millivolts)
{
    // supp batt v draw
    display.SetTextSize(2);
    uint32_t whole = millivolts / 1000;
    uint32_t frac = (millivolts % 1000);
    snprintf(text_buffer.data(),
             sizeof(text_buffer),
             "%lu.%02lu",
             static_cast<unsigned long>(whole),
             static_cast<unsigned long>(frac));
    display.FillRect(110, 155, 80, 16, RGB565_BLUE);
    display.DrawText(110, 155, text_buffer.data(), RGB565_BLACK);
}

void drawDirection(flare_can::Direction direction)
{
    display.SetTextSize(3);
    display.DrawText(10, 40, "R N F", RGB565_BLACK);

    if (direction == flare_can::Direction::REVERSE)
    {
        display.DrawText(10, 40, "R", RGB565_ORANGE);
    }
    else if (direction == flare_can::Direction::FORWARD)
    {
        display.DrawText(82, 40, "F", RGB565_ORANGE);
    }
    else
    {
        display.DrawText(46, 40, "N", RGB565_ORANGE);
    }
}

void drawMainBatt(uint16_t centivolts)
{
    // main batt v draw
    display.SetTextSize(2);
    uint16_t whole = centivolts / 100;
    uint16_t frac = (centivolts % 100);
    snprintf(text_buffer.data(),
             sizeof(text_buffer),
             "%lu.%02lu",
             static_cast<unsigned long>(whole),
             static_cast<unsigned long>(frac));
    display.FillRect(110, 130, 80, 16, RGB565_BLUE);
    display.DrawText(110, 130, text_buffer.data(), RGB565_BLACK);
}

void drawHighTemp(uint16_t decicelcius)
{
    // main batt high temp draw
    display.SetTextSize(2);
    uint16_t whole = decicelcius / 10;
    uint16_t frac = (decicelcius % 10);
    snprintf(text_buffer.data(),
             sizeof(text_buffer),
             "%lu.%lu",
             static_cast<unsigned long>(whole),
             static_cast<unsigned long>(frac));
    display.FillRect(110, 180, 60, 16, RGB565_BLUE);
    display.DrawText(110, 180, text_buffer.data(), RGB565_BLACK);
}

void drawArrayContactors(flare_can::ArrayContactors contactors)
{
    // array contactors draw
    display.SetTextSize(2);
    display.FillRect(110, 205, 100, 16, RGB565_BLUE);
    if (contactors == flare_can::ArrayContactors::MAIN_CLOSED)
    {
        display.DrawText(110, 205, "CLSD", RGB565_BLACK);
    }
    else
    {
        display.DrawText(110, 205, "OPEN", RGB565_BLACK);
    }
}
void drawPowerMode(flare_can::MCPowerMode mode)
{
    // power mode highlight
    display.SetTextSize(3);
    display.DrawText(10, 70, "EC PW", RGB565_BLACK);

    if (mode == flare_can::MCPowerMode::ECO)
    {
        display.DrawText(10, 70, "EC", RGB565_ORANGE);
    }
    else
    {
        display.DrawText(64, 70, "PW", RGB565_ORANGE);
    }
}
void drawCC(uint8_t mph)
{
    display.SetTextSize(3);
    if (mph > 99)
    {
        snprintf(text_buffer.data(), text_buffer.size(), "ER");
    }
    else
    {
        snprintf(text_buffer.data(), sizeof(text_buffer), "%02lu", static_cast<unsigned long>(mph));
    }
    display.FillRect(210, 60, 30, 16, RGB565_BLUE);
    display.DrawText(210, 60, text_buffer.data(), RGB565_WHITE);
}

void drawKillStatus(flare_can::CarKilledStatus killed)
{
    // kill status draw
    display.SetTextSize(2);
    display.FillRect(240, 205, 75, 16, RGB565_BLUE);
    if (killed == flare_can::CarKilledStatus::DEAD)
    {
        display.DrawText(240, 205, "KILLED", RGB565_RED);
    }
}

void drawHeadlightsStatus(bool on)
{
    // headlights draw
    display.SetTextSize(2);
    display.FillRect(228, 155, 72, 16, RGB565_BLUE);
    if (on)
    {
        display.DrawText(228, 155, "HDLGTS", RGB565_BLACK);
    }
}

void drawHornStatus(bool on)
{
    // horn draw
    display.SetTextSize(2);
    display.FillRect(252, 130, 48, 16, RGB565_BLUE);
    if (on)
    {
        display.DrawText(252, 130, "HORN", RGB565_BLACK);
    }
}

}  // namespace steering
