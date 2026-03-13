//
// Created by justin on 2/16/26.
//

#ifndef SOLARGATORSSTM32PROJECTS_CAN_PROTOCOL_H
#define SOLARGATORSSTM32PROJECTS_CAN_PROTOCOL_H

#include <cstdint>

// holds enums used between boards for can messages
namespace flare_can
{

enum class ArrayContactors : uint8_t
{
    BOTH_OPEN = 0,
    PRECHARGE_CLOSED = 1,
    MAIN_CLOSED = 2
};

enum class TurnSignals : uint8_t
{
    OFF = 0,
    LEFT = 1,
    RIGHT = 2,
    HAZARDS = 3
};

enum class Direction : uint8_t
{
    REVERSE = 0,
    FORWARD = 1
};

enum class MCPowerMode : uint8_t
{
    ECO = 0,
    POWER = 1
};

enum class CarKilledStatus : uint8_t
{
    ALIVE = 0,
    DEAD = 1
};

}  // namespace flare_can

#endif  //SOLARGATORSSTM32PROJECTS_CAN_PROTOCOL_H
