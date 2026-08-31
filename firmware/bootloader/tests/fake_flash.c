#include "bl_port.h"

#include "bl_config.h"

#include <string.h>

#define FAKE_FLASH_SIZE 0x200000U

static uint8_t s_flash[FAKE_FLASH_SIZE];
static uint32_t s_millis;
uint32_t bl_test_jumped_to;

void bl_fake_flash_reset(void)
{
    memset(s_flash, 0xFF, sizeof(s_flash));
    s_millis = 0;
    bl_test_jumped_to = 0;
}

static uint8_t* at(uint32_t addr)
{
    return &s_flash[addr - BL_FLASH_BASE];
}

int bl_port_flash_read(uint32_t addr, void* dst, size_t len)
{
    memcpy(dst, at(addr), len);
    return 0;
}

int bl_port_flash_erase(uint32_t addr, size_t len)
{
    memset(at(addr), 0xFF, len);
    return 0;
}

int bl_port_flash_program(uint32_t addr, const void* src, size_t len)
{
    memcpy(at(addr), src, len);
    return 0;
}

int bl_port_flash_flush(void)
{
    return 0;
}

void bl_port_jump(uint32_t vector_table_addr)
{
    bl_test_jumped_to = vector_table_addr;
}

uint32_t bl_port_millis(void)
{
    return s_millis;
}

void bl_port_delay_ms(uint32_t ms)
{
    s_millis += ms;
}

void bl_test_set_millis(uint32_t ms)
{
    s_millis = ms;
}
