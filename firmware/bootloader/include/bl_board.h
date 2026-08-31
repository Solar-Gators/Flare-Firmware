#pragma once

#include <stdint.h>

#ifdef STM32U575xx
#include "stm32u5xx_hal.h"
#endif

typedef struct
{
    uint32_t board_id;
    GPIO_TypeDef* can_rx_port;
    uint16_t can_rx_pin;
    GPIO_TypeDef* can_tx_port;
    uint16_t can_tx_pin;
    uint8_t can_af;
} bl_board_config_t;

const bl_board_config_t* bl_board_get(void);
