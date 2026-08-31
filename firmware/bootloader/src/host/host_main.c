#include "bl_board.h"
#include "bl_config.h"
#include "bl_crc32.h"
#include "bl_port.h"
#include "bl_protocol.h"
#include "main.h"

#include <string.h>

void bl_clock_init(void);
void bl_can_init(void);

extern const uint8_t bl_image_slot_a[];
extern const unsigned bl_image_slot_a_len;
extern const uint8_t bl_image_slot_b[];
extern const unsigned bl_image_slot_b_len;
extern const uint8_t bl_image_noconfirm_b[];
extern const unsigned bl_image_noconfirm_b_len;

#ifndef BL_HOST_TARGET_BOARD_ID
#define BL_HOST_TARGET_BOARD_ID BL_BOARD_ID_STEERING_WHEEL
#endif

#ifndef BL_HOST_SEND_NOCONFIRM
#define BL_HOST_SEND_NOCONFIRM 0
#endif

static int wait_rsp(uint8_t expect_cmd, uint8_t* extra, uint32_t timeout_ms)
{
    const uint32_t start = bl_port_millis();
    while ((bl_port_millis() - start) < timeout_ms)
    {
        uint32_t id = 0;
        uint8_t data[8];
        uint8_t dlc = 0;
        if (bl_can_recv(&id, data, &dlc) != 0)
        {
            continue;
        }
        if (id != BL_CAN_RSP_ID || dlc < 3U)
        {
            continue;
        }
        if (data[1] == expect_cmd || (expect_cmd == 0xD0U && data[1] == 0xD0U))
        {
            if (data[0] != BL_RSP_OK)
            {
                return -1;
            }
            if (extra != NULL)
            {
                *extra = data[3];
            }
            return 0;
        }
    }
    return -1;
}

static int send_cmd(uint8_t cmd, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7)
{
    uint8_t d[8] = {cmd, b1, b2, b3, b4, b5, b6, b7};
    return bl_can_send(BL_CAN_CMD_ID, d, 8);
}

static int send_image(const uint8_t* image, unsigned len, uint8_t slot, uint32_t version)
{
    const uint32_t crc = bl_crc32(image, len);
    if (send_cmd(BL_CMD_START,
                 (uint8_t)BL_HOST_TARGET_BOARD_ID,
                 slot,
                 (uint8_t)len,
                 (uint8_t)(len >> 8),
                 (uint8_t)(len >> 16),
                 (uint8_t)(len >> 24),
                 0) != 0)
    {
        return -1;
    }
    if (wait_rsp(BL_CMD_START, NULL, 2000) != 0)
    {
        return -1;
    }
    if (send_cmd(BL_CMD_START2,
                 (uint8_t)BL_HOST_TARGET_BOARD_ID,
                 (uint8_t)crc,
                 (uint8_t)(crc >> 8),
                 (uint8_t)(crc >> 16),
                 (uint8_t)(crc >> 24),
                 (uint8_t)version,
                 (uint8_t)(version >> 8)) != 0)
    {
        return -1;
    }
    if (wait_rsp(BL_CMD_START2, NULL, 30000) != 0)
    {
        return -1;
    }

    uint16_t seq = 0;
    unsigned offset = 0;
    while (offset < len)
    {
        uint8_t frame[8] = {0};
        frame[0] = (uint8_t)seq;
        frame[1] = (uint8_t)(seq >> 8);
        unsigned n = len - offset;
        if (n > BL_DATA_BYTES_PER_FRAME)
        {
            n = BL_DATA_BYTES_PER_FRAME;
        }
        memcpy(&frame[2], &image[offset], n);
        if (bl_can_send(BL_CAN_DATA_ID, frame, (uint8_t)(2U + n)) != 0)
        {
            return -1;
        }
        const int last = ((offset + n) >= len);
        if (last || ((seq % BL_ACK_PERIOD) == (BL_ACK_PERIOD - 1U)))
        {
            if (wait_rsp(0xD0U, NULL, 5000) != 0)
            {
                return -1;
            }
        }
        offset += n;
        seq++;
        bl_port_delay_ms(1);
    }

    if (send_cmd(BL_CMD_VERIFY, (uint8_t)BL_HOST_TARGET_BOARD_ID, 0, 0, 0, 0, 0, 0) != 0)
    {
        return -1;
    }
    if (wait_rsp(BL_CMD_VERIFY, NULL, 10000) != 0)
    {
        return -1;
    }
    if (send_cmd(BL_CMD_ACTIVATE, (uint8_t)BL_HOST_TARGET_BOARD_ID, 0, 0, 0, 0, 0, 0) != 0)
    {
        return -1;
    }
    if (wait_rsp(BL_CMD_ACTIVATE, NULL, 2000) != 0)
    {
        return -1;
    }
    if (send_cmd(BL_CMD_JUMP, (uint8_t)BL_HOST_TARGET_BOARD_ID, 0, 0, 0, 0, 0, 0) != 0)
    {
        return -1;
    }
    return 0;
}

int main(void)
{
    HAL_Init();
    bl_clock_init();
    bl_can_init();

    /* Cover the target's 1 s ENTER window. */
    const uint32_t burst_end = bl_port_millis() + 1500U;
    while (bl_port_millis() < burst_end)
    {
        (void)send_cmd(BL_CMD_ENTER, (uint8_t)BL_HOST_TARGET_BOARD_ID, 0, 0, 0, 0, 0, 0);
        bl_port_delay_ms(50);
    }
    (void)wait_rsp(BL_CMD_ENTER, NULL, 500);

    (void)send_cmd(BL_CMD_SELECT, (uint8_t)BL_HOST_TARGET_BOARD_ID, 0, 0, 0, 0, 0, 0);
    (void)wait_rsp(BL_CMD_SELECT, NULL, 500);

    (void)send_cmd(BL_CMD_STATUS, (uint8_t)BL_HOST_TARGET_BOARD_ID, 0, 0, 0, 0, 0, 0);
    uint8_t inactive = 1;
    uint32_t id = 0;
    uint8_t data[8];
    uint8_t dlc = 0;
    const uint32_t t0 = bl_port_millis();
    while ((bl_port_millis() - t0) < 500U)
    {
        if (bl_can_recv(&id, data, &dlc) == 0 && id == BL_CAN_RSP_ID && data[1] == BL_CMD_STATUS)
        {
            inactive = data[5];
            break;
        }
    }

    const uint8_t* image = bl_image_slot_b;
    unsigned image_len = bl_image_slot_b_len;
    if (inactive == 0U)
    {
        image = bl_image_slot_a;
        image_len = bl_image_slot_a_len;
    }
#if BL_HOST_SEND_NOCONFIRM
    if (inactive == 1U)
    {
        image = bl_image_noconfirm_b;
        image_len = bl_image_noconfirm_b_len;
    }
#endif

    (void)send_image(image, image_len, inactive, 1U);

    for (;;)
    {
        bl_port_delay_ms(1000);
    }
}
