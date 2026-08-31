#pragma once

#include <stddef.h>
#include <stdint.h>

uint32_t bl_crc32(const void* data, size_t len);
uint32_t bl_crc32_init(void);
uint32_t bl_crc32_feed(uint32_t crc, const void* data, size_t len);
uint32_t bl_crc32_final(uint32_t crc);
