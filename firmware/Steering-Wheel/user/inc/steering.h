#pragma once

#include "main.h"

namespace steering
{

// public functions
HAL_StatusTypeDef init();
void sendRequestsMessage();
void sendMitsubaRequestMessage();
void processScreen();
void flareDance();
void processHornButton();
void processRegen();
void processTurnSignals();
void processKill();
void processCC();
void processTimer();

inline uint32_t mitsuba_request_message_send_period_ms = 1000;

}  // namespace steering
