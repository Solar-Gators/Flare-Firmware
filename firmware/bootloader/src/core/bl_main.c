#include "bl_board.h"
#include "bl_config.h"
#include "bl_port.h"
#include "bl_protocol.h"
#include "bl_slots.h"
#include "main.h"

void bl_clock_init(void);
void bl_can_init(void);

int main(void)
{
    HAL_Init();
    bl_clock_init();
    bl_can_init();

    const bl_board_config_t* board = bl_board_get();
    bl_protocol_init(board->board_id);

    const bl_resolve_t choice = bl_slots_resolve(board->board_id);
    if (choice >= 0)
    {
        const uint32_t start = bl_port_millis();
        while ((bl_port_millis() - start) < BL_ENTER_WAIT_MS)
        {
            bl_protocol_poll();
            if (bl_protocol_enter_requested())
            {
                break;
            }
        }
        if (!bl_protocol_enter_requested())
        {
            bl_port_jump(BL_SLOT_VECTORS(bl_slot_base((uint8_t)choice)));
        }
    }

    for (;;)
    {
        bl_protocol_poll();
    }
}
