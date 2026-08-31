#pragma once

#include <stdint.h>

/* Flash map (STM32U575 2 MB, 8 KB pages). */
#define BL_FLASH_BASE 0x08000000UL
#define BL_CODE_BASE BL_FLASH_BASE
#define BL_CODE_SIZE 0x0000E000UL /* 56 KB */
#define BL_META_BASE 0x0800E000UL
#define BL_META_SIZE 0x00002000UL /* 8 KB page */
#define BL_SLOT_A_BASE 0x08010000UL
#define BL_SLOT_B_BASE 0x08108000UL
#define BL_SLOT_SIZE 0x000F8000UL /* 992 KB */
#define BL_HEADER_SIZE 0x200UL
#define BL_SLOT_COUNT 2U

#define BL_SLOT_VECTORS(slot_base) ((slot_base) + BL_HEADER_SIZE)

#define BL_IMG_MAGIC 0x4C424C46UL /* 'FLBL' */
#define BL_META_MAGIC 0x4D54424CUL /* 'LBTM' */

#define BL_BOARD_ID_FRONT_VCU 1U
#define BL_BOARD_ID_REAR_VCU 2U
#define BL_BOARD_ID_STEERING_WHEEL 3U
#define BL_BOARD_ID_TELEMETRY 4U

#define BL_MAX_UNCONFIRMED_BOOTS 3U
#define BL_ENTER_WAIT_MS 1000U

#define BL_CAN_CMD_ID 0x7E0U
#define BL_CAN_RSP_ID 0x7E1U
#define BL_CAN_DATA_ID 0x7E2U

#define BL_ACK_PERIOD 8U
#define BL_DATA_BYTES_PER_FRAME 6U

#define BL_CMD_SELECT 0x01U
#define BL_CMD_ENTER 0x02U
#define BL_CMD_START 0x03U
#define BL_CMD_START2 0x04U
#define BL_CMD_ABORT 0x05U
#define BL_CMD_VERIFY 0x06U
#define BL_CMD_ACTIVATE 0x07U
#define BL_CMD_JUMP 0x08U
#define BL_CMD_STATUS 0x09U

#define BL_RSP_OK 0x00U
#define BL_RSP_NAK 0x01U
#define BL_RSP_BUSY 0x02U

#define BL_NAK_BAD_BOARD 0x10U
#define BL_NAK_BAD_SLOT 0x11U
#define BL_NAK_BAD_SIZE 0x12U
#define BL_NAK_BAD_SEQ 0x13U
#define BL_NAK_FLASH 0x14U
#define BL_NAK_CRC 0x15U
#define BL_NAK_STATE 0x16U
#define BL_NAK_ACTIVE_SLOT 0x17U

static inline uint32_t bl_slot_base(uint8_t slot)
{
    return (slot == 0U) ? BL_SLOT_A_BASE : BL_SLOT_B_BASE;
}

static inline int bl_addr_in_slot(uint32_t addr, uint8_t slot)
{
    const uint32_t base = bl_slot_base(slot);
    return (addr >= base) && (addr < (base + BL_SLOT_SIZE));
}

static inline int bl_addr_is_metadata(uint32_t addr)
{
    return (addr >= BL_META_BASE) && (addr < (BL_META_BASE + BL_META_SIZE));
}
