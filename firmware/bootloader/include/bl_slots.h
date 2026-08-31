#pragma once

#include <stdint.h>

#include "bl_config.h"
#include "bl_image.h"

#define BL_SLOT_NONE 0xFFU

typedef struct
{
    uint32_t magic;
    uint32_t crc32;
    uint8_t active_slot;
    uint8_t last_confirmed_slot;
    uint8_t pending_slot;
    uint8_t boot_attempts;
    uint8_t slot_valid[BL_SLOT_COUNT];
    uint8_t slot_confirmed[BL_SLOT_COUNT];
    uint32_t slot_crc[BL_SLOT_COUNT];
    uint32_t slot_size[BL_SLOT_COUNT];
    uint32_t slot_version[BL_SLOT_COUNT];
    uint32_t write_seq;
    uint8_t reserved[20];
} bl_metadata_t;

_Static_assert((sizeof(bl_metadata_t) % 16U) == 0U, "metadata must be quad-word aligned");

typedef enum
{
    BL_RESOLVE_STAY = -1,
    BL_RESOLVE_SLOT_A = 0,
    BL_RESOLVE_SLOT_B = 1
} bl_resolve_t;

int bl_slots_load(bl_metadata_t* out);
int bl_slots_save(const bl_metadata_t* meta);
void bl_slots_init_empty(bl_metadata_t* meta);

int bl_slot_image_valid(uint8_t slot, uint32_t expected_board_id, uint32_t* out_crc);

bl_resolve_t bl_slots_resolve(uint32_t board_id);

uint8_t bl_slots_inactive(const bl_metadata_t* meta);

int bl_slots_mark_valid(uint8_t slot,
                        uint32_t board_id,
                        uint32_t version,
                        uint32_t size,
                        uint32_t crc32);
int bl_slots_activate(uint8_t slot);
int bl_slots_confirm(void);

int bl_write_slot_header(uint8_t slot, const bl_image_header_t* hdr);
