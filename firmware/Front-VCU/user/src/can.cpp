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
HAL_StatusTypeDef steeringRequestsCallback(const sg::CANFrame& frame, void* ctx)
{
    frontvcu::state.turn_signals_status.store(static_cast<flare_can::TurnSignals>(frame.data[0]));

    frontvcu::state.horn_state.store(frame.data[3]);

    return HAL_OK;
}

void can_init()
{
    //ASSERT_TRUE(can_device.addFilterId(0x064, sg::CANFrameIDType::STANDARD, sg::CANFrameRTRMode::DATA, sg::CANFramePriority::HIGH));
    ASSERT_HAL_OK(can_device.startCANDevice());
}
}  // namespace frontvcu
