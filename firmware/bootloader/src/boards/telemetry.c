#include "bl_board.h"

#include "bl_config.h"

static const bl_board_config_t cfg = {
    .board_id = BL_BOARD_ID_TELEMETRY,
    .can_rx_port = GPIOB,
    .can_rx_pin = GPIO_PIN_8,
    .can_tx_port = GPIOB,
    .can_tx_pin = GPIO_PIN_9,
    .can_af = GPIO_AF9_FDCAN1,
};

const bl_board_config_t* bl_board_get(void)
{
    return &cfg;
}
