#include "bl_config.h"
#include "bl_crc32.h"
#include "bl_image.h"
#include "bl_port.h"
#include "bl_slots.h"

#include <stdio.h>
#include <string.h>

void bl_fake_flash_reset(void);
extern uint32_t bl_test_jumped_to;

#define ASSERT_TRUE(cond)                                                                      \
    do                                                                                         \
    {                                                                                          \
        if (!(cond))                                                                           \
        {                                                                                      \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                    \
            return 1;                                                                          \
        }                                                                                      \
    } while (0)

static void plant_image(uint8_t slot, uint32_t board_id, uint32_t version, const uint8_t* payload, uint32_t size)
{
    bl_image_header_t hdr;
    memset(&hdr, 0xFF, sizeof(hdr));
    hdr.magic = BL_IMG_MAGIC;
    hdr.board_id = board_id;
    hdr.version = version;
    hdr.payload_size = size;
    hdr.crc32 = bl_crc32(payload, size);
    (void)bl_port_flash_erase(bl_slot_base(slot), BL_SLOT_SIZE);
    (void)bl_write_slot_header(slot, &hdr);
    (void)bl_port_flash_program(BL_SLOT_VECTORS(bl_slot_base(slot)), payload, size);
}

static const uint8_t k_payload_a[] = {0x00, 0x00, 0x00, 0x20, 0x01, 0x02, 0x03, 0x04};
static const uint8_t k_payload_b[] = {0x00, 0x00, 0x00, 0x20, 0xAA, 0xBB, 0xCC, 0xDD};

int main(void)
{
    const uint32_t board = BL_BOARD_ID_STEERING_WHEEL;

    bl_fake_flash_reset();
    ASSERT_TRUE(bl_slots_resolve(board) == BL_RESOLVE_STAY);

    plant_image(0, board, 1, k_payload_a, sizeof(k_payload_a));
    ASSERT_TRUE(bl_slots_mark_valid(0, board, 1, sizeof(k_payload_a), bl_crc32(k_payload_a, sizeof(k_payload_a))) ==
                0);
    ASSERT_TRUE(bl_slots_activate(0) == 0);

    bl_resolve_t r = bl_slots_resolve(board);
    ASSERT_TRUE(r == BL_RESOLVE_SLOT_A);
    ASSERT_TRUE(bl_slots_confirm() == 0);

    r = bl_slots_resolve(board);
    ASSERT_TRUE(r == BL_RESOLVE_SLOT_A);

    plant_image(1, board, 2, k_payload_b, sizeof(k_payload_b));
    ASSERT_TRUE(bl_slots_mark_valid(1, board, 2, sizeof(k_payload_b), bl_crc32(k_payload_b, sizeof(k_payload_b))) ==
                0);
    ASSERT_TRUE(bl_slots_activate(1) == 0);

    for (unsigned i = 0; i < BL_MAX_UNCONFIRMED_BOOTS; i++)
    {
        r = bl_slots_resolve(board);
        ASSERT_TRUE(r == BL_RESOLVE_SLOT_B);
    }
    r = bl_slots_resolve(board);
    ASSERT_TRUE(r == BL_RESOLVE_SLOT_A);

    bl_metadata_t meta;
    ASSERT_TRUE(bl_slots_load(&meta) == 0);
    ASSERT_TRUE(meta.active_slot == 0);
    ASSERT_TRUE(meta.last_confirmed_slot == 0);
    ASSERT_TRUE(meta.slot_valid[1] == 0);

    plant_image(1, board, 3, k_payload_b, sizeof(k_payload_b));
    ASSERT_TRUE(bl_slots_mark_valid(1, board, 3, sizeof(k_payload_b), bl_crc32(k_payload_b, sizeof(k_payload_b))) ==
                0);
    ASSERT_TRUE(bl_slots_activate(1) == 0);
    ASSERT_TRUE(bl_slots_resolve(board) == BL_RESOLVE_SLOT_B);
    ASSERT_TRUE(bl_slots_confirm() == 0);
    ASSERT_TRUE(bl_slots_resolve(board) == BL_RESOLVE_SLOT_B);
    ASSERT_TRUE(bl_slots_load(&meta) == 0);
    ASSERT_TRUE(meta.last_confirmed_slot == 1);

    /* Corrupt slot B payload; fallback to last confirmed is B which is now bad, then stay or A? */
    uint8_t junk = 0x00;
    (void)bl_port_flash_program(BL_SLOT_VECTORS(BL_SLOT_B_BASE) + 4U, &junk, 1);
    /* last confirmed is B, A still valid. resolve should try B, fail, fall back to A if last_confirmed is B...
       Implementation: active B invalid CRC -> fallback last_confirmed B also bad -> STAY.
       Restore A as last confirmed by planting a confirmed A again. */
    bl_fake_flash_reset();
    plant_image(0, board, 1, k_payload_a, sizeof(k_payload_a));
    ASSERT_TRUE(bl_slots_mark_valid(0, board, 1, sizeof(k_payload_a), bl_crc32(k_payload_a, sizeof(k_payload_a))) ==
                0);
    ASSERT_TRUE(bl_slots_activate(0) == 0);
    ASSERT_TRUE(bl_slots_confirm() == 0);
    plant_image(1, board, 2, k_payload_b, sizeof(k_payload_b));
    ASSERT_TRUE(bl_slots_mark_valid(1, board, 2, sizeof(k_payload_b), bl_crc32(k_payload_b, sizeof(k_payload_b))) ==
                0);
    ASSERT_TRUE(bl_slots_activate(1) == 0);
    (void)bl_port_flash_program(BL_SLOT_VECTORS(BL_SLOT_B_BASE) + 4U, &junk, 1);
    r = bl_slots_resolve(board);
    ASSERT_TRUE(r == BL_RESOLVE_SLOT_A);

    /* Mid-write of B must not disturb A: erase B only. */
    ASSERT_TRUE(bl_port_flash_erase(BL_SLOT_B_BASE, BL_SLOT_SIZE) == 0);
    r = bl_slots_resolve(board);
    ASSERT_TRUE(r == BL_RESOLVE_SLOT_A);

    printf("test_bl_slots: PASS\n");
    return 0;
}
