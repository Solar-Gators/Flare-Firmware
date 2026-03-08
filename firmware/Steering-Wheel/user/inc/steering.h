#pragma once

#include "main.h"

namespace steering
{

// public functions
HAL_StatusTypeDef init();
void sendRequestsMessage();
void processScreen();
void processHornButton();
void processTurnAndKill();
void processCC();

}  // namespace steering
