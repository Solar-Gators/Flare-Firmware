#include "bl_protocol.h"

#include <string.h>

#include "bl_config.h"
#include "bl_image.h"
#include "bl_port.h"
#include "bl_slots.h"

typedef enum
{
    ST_IDLE = 0,
    ST_SELECTED,
    ST_START2,
    ST_DATA
} bl_state_t;

static uint32_t s_board_id;
static bl_state_t s_state;
static int s_enter;
static uint8_t s_slot;
static uint32_t s_size;
static uint32_t s_crc;
static uint32_t s_version;
static uint32_t s_offset;
static uint16_t s_seq;

static void send_rsp(uint8_t status, uint8_t cmd, uint8_t extra)
{
    uint8_t d[8] = {status, cmd, (uint8_t)s_board_id, extra, 0, 0, 0, 0};
    (void)bl_can_send(BL_CAN_RSP_ID, d, 8);
}

static void send_status(void)
{
    bl_metadata_t meta;
    uint8_t d[8] = {BL_RSP_OK, BL_CMD_STATUS, (uint8_t)s_board_id, 0, 0, 0, 0, 0};
    if (bl_slots_load(&meta) != 0)
    {
        bl_slots_init_empty(&meta);
    }
    d[3] = meta.active_slot;
    d[4] = meta.last_confirmed_slot;
    d[5] = bl_slots_inactive(&meta);
    d[6] = meta.boot_attempts;
    d[7] = (uint8_t)((meta.slot_valid[0] & 1U) | ((meta.slot_valid[1] & 1U) << 1) |
                     ((meta.slot_confirmed[0] & 1U) << 2) | ((meta.slot_confirmed[1] & 1U) << 3));
    (void)bl_can_send(BL_CAN_RSP_ID, d, 8);
}

void bl_protocol_init(uint32_t board_id)
{
    s_board_id = board_id;
    s_state = ST_IDLE;
    s_enter = 0;
    s_slot = 0;
    s_size = 0;
    s_crc = 0;
    s_version = 0;
    s_offset = 0;
    s_seq = 0;
}

int bl_protocol_enter_requested(void)
{
    return s_enter;
}

void bl_protocol_poll(void)
{
    uint32_t id = 0;
    uint8_t data[8];
    uint8_t dlc = 0;
    while (bl_can_recv(&id, data, &dlc) == 0)
    {
        if (id == BL_CAN_CMD_ID)
        {
            bl_protocol_on_cmd(data, dlc);
        }
        else if (id == BL_CAN_DATA_ID)
        {
            bl_protocol_on_data(data, dlc);
        }
    }
}

void bl_protocol_on_cmd(const uint8_t data[8], uint8_t dlc)
{
    if (dlc < 2U)
    {
        return;
    }
    const uint8_t cmd = data[0];
    const uint8_t board = data[1];
    if (board != (uint8_t)s_board_id && cmd != BL_CMD_SELECT)
    {
        return;
    }

    switch (cmd)
    {
        case BL_CMD_SELECT:
            if (board == (uint8_t)s_board_id)
            {
                s_state = ST_SELECTED;
                s_enter = 1;
                send_rsp(BL_RSP_OK, cmd, 0);
            }
            break;
        case BL_CMD_ENTER:
            if (board == (uint8_t)s_board_id)
            {
                s_enter = 1;
                s_state = ST_SELECTED;
                send_rsp(BL_RSP_OK, cmd, 0);
            }
            break;
        case BL_CMD_STATUS:
            send_status();
            break;
        case BL_CMD_ABORT:
            s_state = ST_SELECTED;
            s_offset = 0;
            send_rsp(BL_RSP_OK, cmd, 0);
            break;
        case BL_CMD_START:
        {
            if (s_state != ST_SELECTED && s_state != ST_IDLE && s_state != ST_DATA)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_STATE);
                break;
            }
            if (board != (uint8_t)s_board_id)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_BAD_BOARD);
                break;
            }
            const uint8_t slot = data[2];
            bl_metadata_t meta;
            if (bl_slots_load(&meta) != 0)
            {
                bl_slots_init_empty(&meta);
            }
            const uint8_t inactive = bl_slots_inactive(&meta);
            if (slot > 1U || slot != inactive)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_ACTIVE_SLOT);
                break;
            }
            s_slot = slot;
            s_size = (uint32_t)data[3] | ((uint32_t)data[4] << 8) | ((uint32_t)data[5] << 16) |
                     ((uint32_t)data[6] << 24);
            if (s_size == 0U || s_size > (BL_SLOT_SIZE - BL_HEADER_SIZE))
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_BAD_SIZE);
                break;
            }
            s_state = ST_START2;
            send_rsp(BL_RSP_OK, cmd, s_slot);
            break;
        }
        case BL_CMD_START2:
        {
            if (s_state != ST_START2)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_STATE);
                break;
            }
            s_crc = (uint32_t)data[2] | ((uint32_t)data[3] << 8) | ((uint32_t)data[4] << 16) |
                    ((uint32_t)data[5] << 24);
            s_version = (uint32_t)data[6] | ((uint32_t)data[7] << 8);
            if (bl_port_flash_erase(bl_slot_base(s_slot), BL_SLOT_SIZE) != 0)
            {
                s_state = ST_SELECTED;
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_FLASH);
                break;
            }
            s_offset = 0;
            s_seq = 0;
            s_state = ST_DATA;
            send_rsp(BL_RSP_OK, cmd, s_slot);
            break;
        }
        case BL_CMD_VERIFY:
        {
            if (s_offset != s_size)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_STATE);
                break;
            }
            if (bl_port_flash_flush() != 0)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_FLASH);
                break;
            }
            bl_image_header_t hdr;
            memset(&hdr, 0xFF, sizeof(hdr));
            hdr.magic = BL_IMG_MAGIC;
            hdr.board_id = s_board_id;
            hdr.version = s_version;
            hdr.payload_size = s_size;
            hdr.crc32 = s_crc;
            if (bl_write_slot_header(s_slot, &hdr) != 0)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_FLASH);
                break;
            }
            uint32_t crc = 0U;
            if (!bl_slot_image_valid(s_slot, s_board_id, &crc) || crc != s_crc)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_CRC);
                break;
            }
            if (bl_slots_mark_valid(s_slot, s_board_id, s_version, s_size, s_crc) != 0)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_FLASH);
                break;
            }
            s_state = ST_SELECTED;
            send_rsp(BL_RSP_OK, cmd, s_slot);
            break;
        }
        case BL_CMD_ACTIVATE:
            if (bl_slots_activate(s_slot) != 0)
            {
                send_rsp(BL_RSP_NAK, cmd, BL_NAK_STATE);
                break;
            }
            send_rsp(BL_RSP_OK, cmd, s_slot);
            break;
        case BL_CMD_JUMP:
        {
            bl_metadata_t meta;
            send_rsp(BL_RSP_OK, cmd, 0);
            bl_port_delay_ms(10);
            if (bl_slots_load(&meta) == 0 && meta.active_slot <= 1U)
            {
                bl_port_jump(BL_SLOT_VECTORS(bl_slot_base(meta.active_slot)));
            }
            break;
        }
        default:
            send_rsp(BL_RSP_NAK, cmd, BL_NAK_STATE);
            break;
    }
}

void bl_protocol_on_data(const uint8_t data[8], uint8_t dlc)
{
    if (s_state != ST_DATA || dlc < 2U)
    {
        return;
    }
    const uint16_t seq = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    if (seq != s_seq)
    {
        send_rsp(BL_RSP_NAK, BL_CAN_DATA_ID & 0xFFU, BL_NAK_BAD_SEQ);
        return;
    }
    const uint8_t payload_len = (uint8_t)(dlc - 2U);
    if (payload_len > BL_DATA_BYTES_PER_FRAME)
    {
        send_rsp(BL_RSP_NAK, BL_CAN_DATA_ID & 0xFFU, BL_NAK_BAD_SIZE);
        return;
    }
    uint32_t remaining = s_size - s_offset;
    uint32_t write_len = payload_len;
    if (write_len > remaining)
    {
        write_len = remaining;
    }
    if (write_len > 0U)
    {
        const uint32_t addr = BL_SLOT_VECTORS(bl_slot_base(s_slot)) + s_offset;
        if (bl_port_flash_program(addr, &data[2], write_len) != 0)
        {
            send_rsp(BL_RSP_NAK, BL_CAN_DATA_ID & 0xFFU, BL_NAK_FLASH);
            s_state = ST_SELECTED;
            return;
        }
        s_offset += write_len;
    }
    s_seq++;
    const int last = (s_offset >= s_size);
    if (last || ((seq % BL_ACK_PERIOD) == (BL_ACK_PERIOD - 1U)))
    {
        uint8_t d[8] = {BL_RSP_OK,
                        0xD0U,
                        (uint8_t)s_board_id,
                        s_slot,
                        (uint8_t)seq,
                        (uint8_t)(seq >> 8),
                        0,
                        0};
        (void)bl_can_send(BL_CAN_RSP_ID, d, 8);
    }
}
