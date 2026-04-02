//
// Created by justin on 2/25/26.
//

#ifndef FLAREFIRMWARE_TELEMETRY_H
#define FLAREFIRMWARE_TELEMETRY_H

#include <cstdint>

namespace telem
{

// give to delay of the lights processing task
inline uint32_t led_toggle_period_ms = 500;

void init();
void sendKillFrame();
void sendSpeedFrame();
void processLightsOutputs();
void queueGpsData();
void readGpsData();
void waitAndSendRadioData();

}  // namespace telem

#endif  //FLAREFIRMWARE_TELEMETRY_H
