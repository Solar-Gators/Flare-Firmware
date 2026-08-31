#include "bl_slots.h"

#include <string.h>

#include "bl_crc32.h"
#include "bl_config.h"
#include "bl_port.h"

static uint32_t metadata_crc(const bl_metadata_t* meta)
{
    bl_metadata_t tmp = *meta;
    tmp.crc32 = 0U;
    return bl_crc32(&tmp, sizeof(tmp));
}

void bl_slots_init_empty(bl_metadata_t* meta)
{
    memset(meta, 0, sizeof(*meta));
    meta->magic = BL_META_MAGIC;
    meta->active_slot = BL_SLOT_NONE;
    meta->last_confirmed_slot = BL_SLOT_NONE;
    meta->pending_slot = BL_SLOT_NONE;
    meta->boot_attempts = 0U;
    meta->slot_valid[0] = 0U;
    meta->slot_valid[1] = 0U;
    meta->crc32 = metadata_crc(meta);
}

int bl_slots_load(bl_metadata_t* out)
{
    if (bl_port_flash_read(BL_META_BASE, out, sizeof(*out)) != 0)
    {
        return -1;
    }
    if (out->magic != BL_META_MAGIC)
    {
        return -1;
    }
    const uint32_t expect = out->crc32;
    if (metadata_crc(out) != expect)
    {
        return -1;
    }
    return 0;
}

int bl_slots_save(const bl_metadata_t* meta)
{
    bl_metadata_t tmp = *meta;
    tmp.magic = BL_META_MAGIC;
    tmp.write_seq = meta->write_seq + 1U;
    tmp.crc32 = metadata_crc(&tmp);

    if (bl_port_flash_erase(BL_META_BASE, BL_META_SIZE) != 0)
    {
        return -1;
    }
    if (bl_port_flash_program(BL_META_BASE, &tmp, sizeof(tmp)) != 0)
    {
        return -1;
    }
    return 0;
}

uint8_t bl_slots_inactive(const bl_metadata_t* meta)
{
    if (meta->active_slot == 0U)
    {
        return 1U;
    }
    return 0U;
}

static int crc_payload(uint8_t slot, uint32_t size, uint32_t* out_crc)
{
    if (size == 0U || size > (BL_SLOT_SIZE - BL_HEADER_SIZE))
    {
        return -1;
    }
    uint32_t crc = bl_crc32_init();
    uint8_t buf[64];
    uint32_t remaining = size;
    uint32_t addr = BL_SLOT_VECTORS(bl_slot_base(slot));
    while (remaining > 0U)
    {
        const uint32_t chunk = remaining > sizeof(buf) ? (uint32_t)sizeof(buf) : remaining;
        if (bl_port_flash_read(addr, buf, chunk) != 0)
        {
            return -1;
        }
        crc = bl_crc32_feed(crc, buf, chunk);
        addr += chunk;
        remaining -= chunk;
    }
    *out_crc = bl_crc32_final(crc);
    return 0;
}

int bl_slot_image_valid(uint8_t slot, uint32_t expected_board_id, uint32_t* out_crc)
{
    if (slot > 1U)
    {
        return 0;
    }
    bl_image_header_t hdr;
    const uint32_t base = bl_slot_base(slot);
    if (bl_port_flash_read(base, &hdr, sizeof(hdr)) != 0)
    {
        return 0;
    }
    if (hdr.magic != BL_IMG_MAGIC || hdr.board_id != expected_board_id)
    {
        return 0;
    }
    if (hdr.payload_size == 0U || hdr.payload_size > (BL_SLOT_SIZE - BL_HEADER_SIZE))
    {
        return 0;
    }
    uint32_t crc = 0U;
    if (crc_payload(slot, hdr.payload_size, &crc) != 0)
    {
        return 0;
    }
    if (crc != hdr.crc32)
    {
        return 0;
    }
    if (out_crc != NULL)
    {
        *out_crc = crc;
    }
    return 1;
}

static int slot_ok(uint8_t slot, uint32_t board_id, const bl_metadata_t* meta)
{
    if (slot > 1U)
    {
        return 0;
    }
    if (meta->slot_valid[slot] == 0U)
    {
        return 0;
    }
    uint32_t crc = 0U;
    if (!bl_slot_image_valid(slot, board_id, &crc))
    {
        return 0;
    }
    if (crc != meta->slot_crc[slot])
    {
        return 0;
    }
    return 1;
}

bl_resolve_t bl_slots_resolve(uint32_t board_id)
{
    bl_metadata_t meta;
    if (bl_slots_load(&meta) != 0)
    {
        return BL_RESOLVE_STAY;
    }

    uint8_t active = meta.active_slot;
    if (active <= 1U && slot_ok(active, board_id, &meta))
    {
        if (meta.slot_confirmed[active] == 0U)
        {
            if (meta.boot_attempts >= BL_MAX_UNCONFIRMED_BOOTS)
            {
                meta.slot_valid[active] = 0U;
                meta.slot_confirmed[active] = 0U;
                const uint8_t fallback = meta.last_confirmed_slot;
                meta.active_slot = fallback;
                meta.pending_slot = BL_SLOT_NONE;
                meta.boot_attempts = 0U;
                (void)bl_slots_save(&meta);
                if (fallback <= 1U && slot_ok(fallback, board_id, &meta))
                {
                    return (bl_resolve_t)fallback;
                }
                return BL_RESOLVE_STAY;
            }
            meta.boot_attempts = (uint8_t)(meta.boot_attempts + 1U);
            (void)bl_slots_save(&meta);
        }
        return (bl_resolve_t)active;
    }

    const uint8_t fallback = meta.last_confirmed_slot;
    if (fallback <= 1U && fallback != active && slot_ok(fallback, board_id, &meta))
    {
        meta.active_slot = fallback;
        meta.boot_attempts = 0U;
        (void)bl_slots_save(&meta);
        return (bl_resolve_t)fallback;
    }

    return BL_RESOLVE_STAY;
}

int bl_write_slot_header(uint8_t slot, const bl_image_header_t* hdr)
{
    if (slot > 1U)
    {
        return -1;
    }
    return bl_port_flash_program(bl_slot_base(slot), hdr, sizeof(*hdr));
}

int bl_slots_mark_valid(uint8_t slot,
                        uint32_t board_id,
                        uint32_t version,
                        uint32_t size,
                        uint32_t crc32)
{
    (void)board_id;
    if (slot > 1U)
    {
        return -1;
    }
    bl_metadata_t meta;
    if (bl_slots_load(&meta) != 0)
    {
        bl_slots_init_empty(&meta);
    }
    meta.slot_valid[slot] = 1U;
    meta.slot_confirmed[slot] = 0U;
    meta.slot_crc[slot] = crc32;
    meta.slot_size[slot] = size;
    meta.slot_version[slot] = version;
    return bl_slots_save(&meta);
}

int bl_slots_activate(uint8_t slot)
{
    if (slot > 1U)
    {
        return -1;
    }
    bl_metadata_t meta;
    if (bl_slots_load(&meta) != 0)
    {
        bl_slots_init_empty(&meta);
    }
    if (meta.slot_valid[slot] == 0U)
    {
        return -1;
    }
    meta.pending_slot = slot;
    meta.active_slot = slot;
    meta.slot_confirmed[slot] = 0U;
    meta.boot_attempts = 0U;
    return bl_slots_save(&meta);
}

int bl_slots_confirm(void)
{
    bl_metadata_t meta;
    if (bl_slots_load(&meta) != 0)
    {
        return -1;
    }
    if (meta.active_slot > 1U)
    {
        return -1;
    }
    meta.slot_confirmed[meta.active_slot] = 1U;
    meta.last_confirmed_slot = meta.active_slot;
    meta.pending_slot = BL_SLOT_NONE;
    meta.boot_attempts = 0U;
    return bl_slots_save(&meta);
}
