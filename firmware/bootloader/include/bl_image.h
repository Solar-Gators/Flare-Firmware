#pragma once

#include <stdint.h>

#include "bl_config.h"

typedef struct
{
    uint32_t magic;
    uint32_t board_id;
    uint32_t version;
    uint32_t payload_size;
    uint32_t crc32;
    uint8_t reserved[BL_HEADER_SIZE - 20U];
} bl_image_header_t;

_Static_assert(sizeof(bl_image_header_t) == BL_HEADER_SIZE, "image header must fill 512 bytes");
