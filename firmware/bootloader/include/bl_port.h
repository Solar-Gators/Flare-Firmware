#pragma once

#include <stddef.h>
#include <stdint.h>

int bl_port_flash_read(uint32_t addr, void* dst, size_t len);
int bl_port_flash_erase(uint32_t addr, size_t len);
int bl_port_flash_program(uint32_t addr, const void* src, size_t len);
int bl_port_flash_flush(void);

void bl_port_jump(uint32_t vector_table_addr);
uint32_t bl_port_millis(void);
void bl_port_delay_ms(uint32_t ms);
