#include "bl_confirm.h"
#include "bl_port.h"
#include "main.h"

#ifndef BL_TEST_CONFIRM
#define BL_TEST_CONFIRM 1
#endif

void bl_clock_init(void);

int main(void)
{
    HAL_Init();
    bl_clock_init();

#if BL_TEST_CONFIRM
    (void)bl_confirm_image();
    for (;;)
    {
        bl_port_delay_ms(500);
    }
#else
    /* Unconfirmed image: reset so the bootloader can count attempts and revert. */
    bl_port_delay_ms(50);
    NVIC_SystemReset();
#endif
}
