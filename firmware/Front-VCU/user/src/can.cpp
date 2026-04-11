//
// Created by vigne on 3/13/2026.
//

#include "can.hpp"

#include "frontvcu_state.hpp"
#include "main.h"

#define ASSERT_TRUE(statement) \
    if (!statement)            \
        Error_Handler();

#define ASSERT_HAL_OK(statement) \
    if (statement != HAL_OK)     \
        Error_Handler();

namespace frontvcu
{
HAL_StatusTypeDef steering_wheel_msg_cb(const sg::CANFrame& msg, void* ctx)
{
    uint8_t turn_signal_rq = msg.data[0];
    if (turn_signal_rq == 0)
    {
        for (auto& light : frontvcu::state.lights_req)
        {
            if (light == LIGHTS_STATES::TURN)
            {
                light = LIGHTS_STATES::OFF;
            }
        }
    }
    else if (turn_signal_rq == 1)
    {
        frontvcu::state.lights_req[0] = LIGHTS_STATES::TURN;
        frontvcu::state.lights_req[1] = LIGHTS_STATES::OFF;
    }
    else if (turn_signal_rq == 2)
    {
        frontvcu::state.lights_req[0] = LIGHTS_STATES::OFF;
        frontvcu::state.lights_req[1] = LIGHTS_STATES::TURN;
    }
    else if (turn_signal_rq == 3)
    {
        for (auto& light : frontvcu::state.lights_req)
        {
            light = LIGHTS_STATES::HAZARDS;
        }
    }

    uint8_t horn_req = msg.data[3];
    frontvcu::state.horn_state.store(horn_req);

    return HAL_OK;
}

void can_init()
{
    //ASSERT_TRUE(can_device.addFilterId(0x064, sg::CANFrameIDType::STANDARD, sg::CANFrameRTRMode::DATA, sg::CANFramePriority::HIGH));
    ASSERT_HAL_OK(can_device.startCANDevice());
}
}  // namespace frontvcu
