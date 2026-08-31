#include "bl_port.h"

#include <string.h>

#include "bl_config.h"

#include "stm32u5xx_hal.h"

static uint8_t s_prog_buf[16] __attribute__((aligned(16)));
static uint32_t s_prog_addr;
static uint8_t s_prog_len;
static int s_prog_open;

static int addr_allowed(uint32_t addr, size_t len)
{
    const uint32_t end = addr + (uint32_t)len;
    if (bl_addr_is_metadata(addr) && (end <= (BL_META_BASE + BL_META_SIZE)))
    {
        return 1;
    }
    if (bl_addr_in_slot(addr, 0) && bl_addr_in_slot(end - 1U, 0))
    {
        return 1;
    }
    if (bl_addr_in_slot(addr, 1) && bl_addr_in_slot(end - 1U, 1))
    {
        return 1;
    }
    return 0;
}

static void addr_to_bank_page(uint32_t addr, uint32_t* bank, uint32_t* page)
{
    const uint32_t offset = addr - FLASH_BASE;
    if (offset >= FLASH_BANK_SIZE)
    {
        *bank = FLASH_BANK_2;
        *page = (offset - FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
    }
    else
    {
        *bank = FLASH_BANK_1;
        *page = offset / FLASH_PAGE_SIZE;
    }
}

int bl_port_flash_read(uint32_t addr, void* dst, size_t len)
{
    memcpy(dst, (const void*)addr, len);
    return 0;
}

static int flush_prog(void)
{
    if (!s_prog_open || s_prog_len == 0U)
    {
        s_prog_open = 0;
        s_prog_len = 0;
        return 0;
    }
    while (s_prog_len < 16U)
    {
        s_prog_buf[s_prog_len++] = 0xFFU;
    }
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        return -1;
    }
    const int rc = (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, s_prog_addr, (uint32_t)s_prog_buf) ==
                    HAL_OK)
                       ? 0
                       : -1;
    (void)HAL_FLASH_Lock();
    s_prog_open = 0;
    s_prog_len = 0;
    return rc;
}

int bl_port_flash_erase(uint32_t addr, size_t len)
{
    if (len == 0U || !addr_allowed(addr, len))
    {
        return -1;
    }
    (void)flush_prog();

    uint32_t remaining = (uint32_t)len;
    uint32_t cursor = addr;
    if ((cursor % FLASH_PAGE_SIZE) != 0U)
    {
        return -1;
    }

    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        return -1;
    }

    while (remaining > 0U)
    {
        uint32_t bank = 0;
        uint32_t page = 0;
        addr_to_bank_page(cursor, &bank, &page);
        uint32_t pages_this_bank = FLASH_PAGE_NB - page;
        uint32_t pages_needed = remaining / FLASH_PAGE_SIZE;
        if ((remaining % FLASH_PAGE_SIZE) != 0U)
        {
            pages_needed++;
        }
        if (pages_needed > pages_this_bank)
        {
            pages_needed = pages_this_bank;
        }

        FLASH_EraseInitTypeDef erase = {0};
        uint32_t page_error = 0;
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.Banks = bank;
        erase.Page = page;
        erase.NbPages = pages_needed;
        if (HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK)
        {
            (void)HAL_FLASH_Lock();
            return -1;
        }
        const uint32_t erased = pages_needed * FLASH_PAGE_SIZE;
        cursor += erased;
        remaining = (remaining > erased) ? (remaining - erased) : 0U;
    }

    (void)HAL_FLASH_Lock();
    return 0;
}

int bl_port_flash_program(uint32_t addr, const void* src, size_t len)
{
    if (len == 0U)
    {
        return 0;
    }
    if (!addr_allowed(addr, len))
    {
        return -1;
    }

    const uint8_t* p = (const uint8_t*)src;
    uint32_t cursor = addr;
    size_t left = len;

    while (left > 0U)
    {
        if (!s_prog_open)
        {
            s_prog_addr = cursor & ~0xFU;
            s_prog_len = (uint8_t)(cursor - s_prog_addr);
            memset(s_prog_buf, 0xFF, sizeof(s_prog_buf));
            s_prog_open = 1;
        }
        else if (cursor != (s_prog_addr + s_prog_len))
        {
            if (flush_prog() != 0)
            {
                return -1;
            }
            continue;
        }

        const size_t space = 16U - s_prog_len;
        const size_t n = (left < space) ? left : space;
        memcpy(&s_prog_buf[s_prog_len], p, n);
        s_prog_len = (uint8_t)(s_prog_len + n);
        p += n;
        cursor += (uint32_t)n;
        left -= n;
        if (s_prog_len == 16U)
        {
            if (flush_prog() != 0)
            {
                return -1;
            }
        }
    }
    return 0;
}

int bl_port_flash_flush(void)
{
    return flush_prog();
}

void bl_port_jump(uint32_t vector_table_addr)
{
    (void)flush_prog();
    typedef void (*reset_fn)(void);
    const uint32_t* vt = (const uint32_t*)vector_table_addr;
    const uint32_t msp = vt[0];
    const reset_fn reset = (reset_fn)vt[1];

    __disable_irq();
    HAL_DeInit();
    SCB->VTOR = vector_table_addr;
    __set_MSP(msp);
    __ISB();
    __DSB();
    reset();
    for (;;)
    {
    }
}

uint32_t bl_port_millis(void)
{
    return HAL_GetTick();
}

void bl_port_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
