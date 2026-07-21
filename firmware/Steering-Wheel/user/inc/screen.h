//
// Created by justin on 2/24/26.
//

#ifndef FLAREFIRMWARE_SCREEN_H
#define FLAREFIRMWARE_SCREEN_H
#include <cstdint>

#include "can_protocol.h"

void initScreen();

void drawLabels();
void drawStartup();
void drawSpeed(uint8_t mph);  // mph shouldn't be more than 2 digit
void drawSuppBatt(uint16_t millivolts);
void drawDirection(flare_can::Direction direction);
void drawMainBatt(uint16_t centivolts);
void drawHighTemp(uint16_t decicelcius);
void drawArrayContactors(flare_can::ArrayContactors contactors);
void drawPowerMode(flare_can::MCPowerMode mode);
void drawKillStatus(flare_can::CarKilledStatus killed);
void drawHeadlightsStatus(bool on);
void drawHornStatus(bool on);
void drawFanStatus(bool on);
void drawTimer(uint32_t);
void drawTurnIndicator(bool left_active, bool right_active, bool blink_state);
void drawThrottlePercent(uint16_t percent);
void drawRegenPercent(uint8_t);
void drawMPPTOutput(uint16_t);

void drawCar(flare_can::Direction direction);

#endif  //FLAREFIRMWARE_SCREEN_H
