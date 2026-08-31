#include "bl_protocol.h"

#include "bl_board.h"
#include "bl_config.h"
#include "main.h"

#include "stm32u5xx_hal.h"

FDCAN_HandleTypeDef hfdcan1;

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* hfdcan)
{
    if (hfdcan->Instance != FDCAN1)
    {
        return;
    }

    const bl_board_config_t* board = bl_board_get();
    RCC_PeriphCLKInitTypeDef periph = {0};
    periph.PeriphClockSelection = RCC_PERIPHCLK_FDCAN1;
    periph.Fdcan1ClockSelection = RCC_FDCAN1CLKSOURCE_PLL1;
    if (HAL_RCCEx_PeriphCLKConfig(&periph) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_RCC_FDCAN1_CLK_ENABLE();
    if (board->can_rx_port == GPIOA || board->can_tx_port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    if (board->can_rx_port == GPIOB || board->can_tx_port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = board->can_af;

    gpio.Pin = board->can_rx_pin;
    HAL_GPIO_Init(board->can_rx_port, &gpio);
    gpio.Pin = board->can_tx_pin;
    HAL_GPIO_Init(board->can_tx_port, &gpio);
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* hfdcan)
{
    if (hfdcan->Instance != FDCAN1)
    {
        return;
    }
    const bl_board_config_t* board = bl_board_get();
    __HAL_RCC_FDCAN1_CLK_DISABLE();
    HAL_GPIO_DeInit(board->can_rx_port, board->can_rx_pin);
    HAL_GPIO_DeInit(board->can_tx_port, board->can_tx_pin);
}

void bl_can_init(void)
{
    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;
    hfdcan1.Init.NominalPrescaler = 16;
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1 = 12;
    hfdcan1.Init.NominalTimeSeg2 = 3;
    hfdcan1.Init.DataPrescaler = 1;
    hfdcan1.Init.DataSyncJumpWidth = 1;
    hfdcan1.Init.DataTimeSeg1 = 1;
    hfdcan1.Init.DataTimeSeg2 = 1;
    hfdcan1.Init.StdFiltersNbr = 1;
    hfdcan1.Init.ExtFiltersNbr = 0;
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
    {
        Error_Handler();
    }

    FDCAN_FilterTypeDef filter = {0};
    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = 0;
    filter.FilterType = FDCAN_FILTER_MASK;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = 0x7E0;
    filter.FilterID2 = 0x7FC; /* match 0x7E0–0x7E3 */
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
                                     FDCAN_REJECT,
                                     FDCAN_REJECT,
                                     FDCAN_REJECT_REMOTE,
                                     FDCAN_REJECT_REMOTE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        Error_Handler();
    }
}

int bl_can_send(uint32_t id, const uint8_t* data, uint8_t dlc)
{
    FDCAN_TxHeaderTypeDef tx = {0};
    tx.Identifier = id;
    tx.IdType = FDCAN_STANDARD_ID;
    tx.TxFrameType = FDCAN_DATA_FRAME;
    tx.DataLength = FDCAN_DLC_BYTES_8;
    (void)dlc;
    tx.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx.BitRateSwitch = FDCAN_BRS_OFF;
    tx.FDFormat = FDCAN_CLASSIC_CAN;
    tx.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx.MessageMarker = 0;
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx, (uint8_t*)data) != HAL_OK)
    {
        return -1;
    }
    return 0;
}

int bl_can_recv(uint32_t* id, uint8_t* data, uint8_t* dlc)
{
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) == 0U)
    {
        return -1;
    }
    FDCAN_RxHeaderTypeDef rx = {0};
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rx, data) != HAL_OK)
    {
        return -1;
    }
    *id = rx.Identifier;
    *dlc = 8;
    return 0;
}
