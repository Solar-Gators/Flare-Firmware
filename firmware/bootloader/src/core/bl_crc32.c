#include "bl_crc32.h"

static uint32_t crc_table(uint32_t n)
{
    uint32_t c = n;
    for (int i = 0; i < 8; i++)
    {
        if ((c & 1U) != 0U)
        {
            c = 0xEDB88320UL ^ (c >> 1U);
        }
        else
        {
            c = c >> 1U;
        }
    }
    return c;
}

uint32_t bl_crc32_init(void)
{
    return 0xFFFFFFFFUL;
}

uint32_t bl_crc32_feed(uint32_t crc, const void* data, size_t len)
{
    const uint8_t* p = (const uint8_t*)data;
    for (size_t i = 0; i < len; i++)
    {
        crc = crc_table((crc ^ p[i]) & 0xFFU) ^ (crc >> 8U);
    }
    return crc;
}

uint32_t bl_crc32_final(uint32_t crc)
{
    return crc ^ 0xFFFFFFFFUL;
}

uint32_t bl_crc32(const void* data, size_t len)
{
    return bl_crc32_final(bl_crc32_feed(bl_crc32_init(), data, len));
}
