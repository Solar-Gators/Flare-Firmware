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
ILI9341 display(240, 320);
std::array<char, 16> text_buffer{};
// const screen variables
constexpr uint16_t text_color{RGB565_BLUE};
constexpr uint16_t background_color{RGB565_BLUE};
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
    drawStartup();
    drawSpeed(state.car_speed.load(std::memory_order_relaxed));
    drawCC(state.cc_mph_requested.load(std::memory_order_relaxed));
    drawSuppBatt(state.supp_batt_voltage_mv.load(std::memory_order_relaxed));
    drawDirection(state.actual_direction.load(std::memory_order_relaxed));
    drawPowerMode(state.mc_power_mode_requested.load(std::memory_order_relaxed));
    drawMainBatt(state.main_batt_voltage_cv.load(std::memory_order_relaxed));
    drawHighTemp(state.high_temp_dc.load(std::memory_order_relaxed));
    // TODO: change this to actual to work with rvcu
    drawArrayContactors(state.array_contactors_requested_closed.load(std::memory_order_relaxed) ?
                    flare_can::ArrayContactors::MAIN_CLOSED :
                    flare_can::ArrayContactors::BOTH_OPEN);
    drawKillStatus(state.killed_status.load(std::memory_order_relaxed));
    drawHeadlightsStatus(state.headlights_requested_on.load(std::memory_order_relaxed));
    drawHornStatus(state.horn_requested_on.load(std::memory_order_relaxed));
}

// startup for turning everything on, yk like in car where when u start the car all the icons turn on
void drawStartup()
{


    for (int i = 90; i > 0; i-=10)
    {
        drawSpeed(i);
        drawCC(i);
        drawSuppBatt((i+10)*120);
        drawDirection(i<50 ? flare_can::Direction::REVERSE : flare_can::Direction::FORWARD);
        drawPowerMode(i>50 ? flare_can::MCPowerMode::ECO : flare_can::MCPowerMode::POWER);
        drawMainBatt((i+30)*120);
        drawHighTemp(i);
        drawArrayContactors(i>50 ?
            flare_can::ArrayContactors::MAIN_CLOSED : flare_can::ArrayContactors::BOTH_OPEN);
        drawKillStatus(flare_can::CarKilledStatus::DEAD);
        drawHeadlightsStatus(true);
        drawHornStatus(true);
        drawTurnIndicator(true, true, true);

        HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, i<30 ? GPIO_PIN_SET:GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin,  i<40 ? GPIO_PIN_SET:GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin,  i<50 ? GPIO_PIN_SET:GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin,  i<60 ? GPIO_PIN_SET:GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin,  i<70 ? GPIO_PIN_SET:GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin,  i<80 ? GPIO_PIN_SET:GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin,  i<90 ? GPIO_PIN_SET:GPIO_PIN_RESET);

        HAL_Delay(10);
    }

    HAL_Delay(200);

    HAL_GPIO_WritePin(BUTTON1_LED_GPIO_Port, BUTTON1_LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON2_LED_GPIO_Port, BUTTON2_LED_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON3_LED_GPIO_Port, BUTTON3_LED_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON4_LED_GPIO_Port, BUTTON4_LED_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON5_LED_GPIO_Port, BUTTON5_LED_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON6_LED_GPIO_Port, BUTTON6_LED_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUTTON8_LED_GPIO_Port, BUTTON8_LED_Pin,  GPIO_PIN_RESET);

    HAL_Delay(10);
}

void drawLabels()
{
    display.SetTextSize(2);
    display.DrawText(140, 90, "MPH", RGB565_GRAY);
    display.DrawText(210, 90, "CC", RGB565_GRAY);
    display.DrawText(260, 90, "RB", RGB565_GRAY);
    display.DrawText(5, 130, "MAIN V:", RGB565_WHITE);
    display.DrawText(5, 155, "SUPP V:", RGB565_WHITE);
    display.DrawText(5, 180, "H TEMP:", RGB565_WHITE);
    display.DrawText(5, 205, "ARR CN:", RGB565_WHITE);

    display.SetTextSize(3);
    display.DrawText(10, 40, "R N F", RGB565_BLACK);
    display.DrawText(10, 70, "EC PW", RGB565_BLACK);
}

void drawSpeed(uint8_t mph)
{
    auto text_color = state.is_cc_on.load(std::memory_order_relaxed) ?
                                    RGB565_ORANGE : RGB565_WHITE;
    display.SetTextSize(5);
    if (mph > 120)  // >99 means can/sensor error
    {
        snprintf(text_buffer.data(), text_buffer.size(), "ER");
    }
    else
    {
        snprintf(text_buffer.data(), sizeof(text_buffer), "%02lu", static_cast<unsigned long>(mph));
    }
    display.FillRect(130, 45, 100, 40, RGB565_BLUE);
    display.DrawText(130, 50, text_buffer.data(), text_color);
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

    // battery 'bar' drawing
    // battery 'bar' drawing
    // max is 150v
    // min is ~50v
    const uint16_t MAX_VOLT = 12000;
    const uint16_t MIN_VOLT = 1000;
    uint16_t level = 0;
    if (millivolts >= MAX_VOLT) {
        level = 100;
    } else if (millivolts <= MIN_VOLT) {
        level = 0;
    } else {
        level = ((millivolts - MIN_VOLT) * 100) / (MAX_VOLT - MIN_VOLT);
    }

    // outline
    const int16_t batt_x = 190;
    const int16_t batt_y = 155;
    const int16_t batt_width = 120;
    const int16_t batt_height = 16;
    const int16_t batt_tip_width = 4;

    // battery outline
    display.FillRect(batt_x, batt_y, batt_width + batt_tip_width, batt_height, RGB565_BLUE);
    display.DrawRect(batt_x, batt_y, batt_width, batt_height, RGB565_BLACK);
    display.FillRect(batt_x + batt_width, batt_y + 4, batt_tip_width, batt_height - 8, RGB565_BLACK);

    // fill battery
    uint16_t fill_width = (level * (batt_width - 4)) / 100;
    uint16_t fill_color = level > 20 ? RGB565_GREEN : RGB565_RED; // red if low
    if (fill_width > 0)
    {
        display.FillRect(batt_x + 2, batt_y + 2, fill_width, batt_height - 4, fill_color);
    }
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

    // battery 'bar' drawing
    // max is 150v
    // min is ~50v
    const uint16_t MAX_VOLT = 15000;
    const uint16_t MIN_VOLT = 5000;
    uint16_t level = 0;
    if (centivolts >= MAX_VOLT) {
        level = 100;
    } else if (centivolts <= MIN_VOLT) {
        level = 0;
    } else {
        level = ((centivolts - MIN_VOLT) * 100) / (MAX_VOLT - MIN_VOLT);
    }

    // outline
    const int16_t batt_x = 190;
    const int16_t batt_y = 130;
    const int16_t batt_width = 120;
    const int16_t batt_height = 16;
    const int16_t batt_tip_width = 4;

    // battery outline
    display.FillRect(batt_x, batt_y, batt_width + batt_tip_width, batt_height, RGB565_BLUE);
    display.DrawRect(batt_x, batt_y, batt_width, batt_height, RGB565_BLACK);
    display.FillRect(batt_x + batt_width, batt_y + 4, batt_tip_width, batt_height - 8, RGB565_BLACK);

    // fill battery
    uint16_t fill_width = (level * (batt_width - 4)) / 100;
    uint16_t fill_color = level > 20 ? RGB565_GREEN : RGB565_RED; // red if low
    if (fill_width > 0)
    {
        display.FillRect(batt_x + 2, batt_y + 2, fill_width, batt_height - 4, fill_color);
    }
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

    // orange text for cc on, white for off
    auto text_color = state.is_cc_on.load(std::memory_order_relaxed) ?
                                    RGB565_ORANGE : RGB565_WHITE;

    if (mph > 110)
    {
        snprintf(text_buffer.data(), text_buffer.size(), "ER");
    }
    else
    {
        snprintf(text_buffer.data(), sizeof(text_buffer), "%02lu", static_cast<unsigned long>(mph));
    }
    display.FillRect(210, 60, 45, 30, RGB565_BLUE);
    display.DrawText(210, 60, text_buffer.data(), text_color);
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

// TODO: might not need this function anymore
void drawHeadlightsStatus(bool on)
{
    // headlights draw
    display.SetTextSize(2);
    display.FillRect(45, 15, 20, 20, RGB565_BLUE);
    if (on)
    {
        display.DrawText(45, 15, "L", RGB565_ORANGE);
    }
}

void drawHornStatus(bool on)
{
    // horn draw
    display.SetTextSize(2);
    display.FillRect(265, 15, 20, 20, RGB565_BLUE);
    if (on)
    {
        display.DrawText(265, 15, "H", RGB565_ORANGE);
    }
}
void drawTurnIndicator(bool left_active, bool right_active, bool blink_state)
{
    // track the prev states
    static bool prev_left_active = false;
    static bool prev_right_active = false;
    static bool prev_blink_state = false;

    // check if changed, only draw when changed
    bool left_changed = (left_active != prev_left_active) ||
                        (left_active && blink_state != prev_blink_state);
    bool right_changed = (right_active != prev_right_active) ||
                        (right_active && blink_state != prev_blink_state);

    if (left_changed)
    {
        display.FillRect(10, 10, 30, 30, RGB565_BLUE);  // clear Left
        // left 'arrow'
        if (left_active && blink_state) {
            display.FillRect(10, 20, 5, 1, RGB565_GREEN);
            display.FillRect(15, 19, 5, 3, RGB565_GREEN);
            display.FillRect(20, 18, 5, 5, RGB565_GREEN);
            display.FillRect(25, 17, 5, 7, RGB565_GREEN);
            display.FillRect(30, 15, 5, 11, RGB565_GREEN);
        }
    }

    if (right_changed)
    {
        display.FillRect(290, 10, 30, 30, RGB565_BLUE); // clear Right
        // right 'arrow'
        if (right_active && blink_state)
        {
            display.FillRect(310, 20, 5, 1, RGB565_GREEN);
            display.FillRect(305, 19, 5, 3, RGB565_GREEN);
            display.FillRect(300, 18, 5, 5, RGB565_GREEN);
            display.FillRect(295, 17, 5, 7, RGB565_GREEN);
            display.FillRect(290, 15, 5, 11, RGB565_GREEN);
        }
    }

    // update states
    prev_left_active = left_active;
    prev_right_active = right_active;
    prev_blink_state = blink_state;
}
}  // namespace steering
