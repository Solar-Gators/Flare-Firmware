#include "bl_board.h"

#include "bl_config.h"

static const bl_board_config_t cfg = {
    .board_id = BL_BOARD_ID_STEERING_WHEEL,
    .can_rx_port = GPIOA,
    .can_rx_pin = GPIO_PIN_11,
    .can_tx_port = GPIOA,
    .can_tx_pin = GPIO_PIN_12,
    .can_af = GPIO_AF9_FDCAN1,
};

const bl_board_config_t* bl_board_get(void)
{
    return &cfg;
}
