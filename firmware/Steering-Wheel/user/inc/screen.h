//
// Created by justin on 2/24/26.
//

#ifndef FLAREFIRMWARE_SCREEN_H
#define FLAREFIRMWARE_SCREEN_H
#include <cstdint>

#include "can_protocol.h"

namespace steering
{

void initScreen();

void drawLabels();
void drawSpeed(uint8_t mph);
void drawSuppBatt(uint16_t millivolts);
void drawDirection(flare_can::Direction direction);
void drawMainBatt(uint16_t centivolts);
void drawHighTemp(uint16_t decicelcius);
void drawArrayContactors(flare_can::ArrayContactors contactors);

}  // namespace steering

#endif  //FLAREFIRMWARE_SCREEN_H
