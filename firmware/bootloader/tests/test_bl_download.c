#include "bl_config.h"
#include "bl_crc32.h"
#include "bl_image.h"
#include "bl_port.h"
#include "bl_protocol.h"
#include "bl_slots.h"

#include <stdio.h>
#include <string.h>

void bl_fake_flash_reset(void);

int bl_can_send(uint32_t id, const uint8_t* data, uint8_t dlc)
{
    (void)id;
    (void)data;
    (void)dlc;
    return 0;
}

int bl_can_recv(uint32_t* id, uint8_t* data, uint8_t* dlc)
{
    (void)id;
    (void)data;
    (void)dlc;
    return -1;
}

#define ASSERT_TRUE(cond)                                                                      \
    do                                                                                         \
    {                                                                                          \
        if (!(cond))                                                                           \
        {                                                                                      \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                    \
            return 1;                                                                          \
        }                                                                                      \
    } while (0)

static int plant_confirmed_a(uint32_t board)
{
    const uint8_t payload[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    bl_image_header_t hdr;
    memset(&hdr, 0xFF, sizeof(hdr));
    hdr.magic = BL_IMG_MAGIC;
    hdr.board_id = board;
    hdr.version = 1;
    hdr.payload_size = sizeof(payload);
    hdr.crc32 = bl_crc32(payload, sizeof(payload));
    if (bl_port_flash_erase(BL_SLOT_A_BASE, BL_SLOT_SIZE) != 0)
    {
        return -1;
    }
    if (bl_write_slot_header(0, &hdr) != 0)
    {
        return -1;
    }
    if (bl_port_flash_program(BL_SLOT_VECTORS(BL_SLOT_A_BASE), payload, sizeof(payload)) != 0)
    {
        return -1;
    }
    if (bl_slots_mark_valid(0, board, 1, sizeof(payload), hdr.crc32) != 0)
    {
        return -1;
    }
    if (bl_slots_activate(0) != 0)
    {
        return -1;
    }
    if (bl_slots_confirm() != 0)
    {
        return -1;
    }
    return 0;
}

int main(void)
{
    const uint32_t board = BL_BOARD_ID_STEERING_WHEEL;
    const uint8_t payload_b[] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
                                 0xB0, 0xB1, 0xB2, 0xB3};
    const uint32_t crc = bl_crc32(payload_b, sizeof(payload_b));

    bl_fake_flash_reset();
    ASSERT_TRUE(plant_confirmed_a(board) == 0);

    bl_protocol_init(board);

    uint8_t cmd[8] = {0};
    cmd[0] = BL_CMD_SELECT;
    cmd[1] = (uint8_t)board;
    bl_protocol_on_cmd(cmd, 8);

    cmd[0] = BL_CMD_STATUS;
    bl_protocol_on_cmd(cmd, 8);

    /* Wrong board_id is ignored / NAK. */
    cmd[0] = BL_CMD_START;
    cmd[1] = (uint8_t)BL_BOARD_ID_FRONT_VCU;
    cmd[2] = 1;
    bl_protocol_on_cmd(cmd, 8);

    uint32_t size = sizeof(payload_b);
    cmd[0] = BL_CMD_START;
    cmd[1] = (uint8_t)board;
    cmd[2] = 1; /* inactive slot B */
    cmd[3] = (uint8_t)size;
    cmd[4] = (uint8_t)(size >> 8);
    cmd[5] = (uint8_t)(size >> 16);
    cmd[6] = (uint8_t)(size >> 24);
    bl_protocol_on_cmd(cmd, 8);

    cmd[0] = BL_CMD_START2;
    cmd[1] = (uint8_t)board;
    cmd[2] = (uint8_t)crc;
    cmd[3] = (uint8_t)(crc >> 8);
    cmd[4] = (uint8_t)(crc >> 16);
    cmd[5] = (uint8_t)(crc >> 24);
    cmd[6] = 2;
    cmd[7] = 0;
    bl_protocol_on_cmd(cmd, 8);

    uint16_t seq = 0;
    unsigned offset = 0;
    while (offset < sizeof(payload_b))
    {
        uint8_t frame[8] = {0};
        frame[0] = (uint8_t)seq;
        frame[1] = (uint8_t)(seq >> 8);
        unsigned n = sizeof(payload_b) - offset;
        if (n > BL_DATA_BYTES_PER_FRAME)
        {
            n = BL_DATA_BYTES_PER_FRAME;
        }
        memcpy(&frame[2], &payload_b[offset], n);
        bl_protocol_on_data(frame, (uint8_t)(2U + n));
        offset += n;
        seq++;
    }

    cmd[0] = BL_CMD_VERIFY;
    cmd[1] = (uint8_t)board;
    bl_protocol_on_cmd(cmd, 8);

    cmd[0] = BL_CMD_ACTIVATE;
    bl_protocol_on_cmd(cmd, 8);

    ASSERT_TRUE(bl_slots_resolve(board) == BL_RESOLVE_SLOT_B);
    ASSERT_TRUE(bl_slot_image_valid(0, board, NULL));
    ASSERT_TRUE(bl_slot_image_valid(1, board, NULL));

    /* Power-loss mid-write of B: erase B, A still boots. */
    ASSERT_TRUE(bl_port_flash_erase(BL_SLOT_B_BASE, 256) == 0);
    ASSERT_TRUE(bl_slots_resolve(board) == BL_RESOLVE_SLOT_A);

    printf("test_bl_download: PASS\n");
    return 0;
}
