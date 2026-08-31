#pragma once

#include <stdint.h>

void bl_protocol_init(uint32_t board_id);
void bl_protocol_poll(void);
int bl_protocol_enter_requested(void);

void bl_protocol_on_cmd(const uint8_t data[8], uint8_t dlc);
void bl_protocol_on_data(const uint8_t data[8], uint8_t dlc);

int bl_can_send(uint32_t id, const uint8_t* data, uint8_t dlc);
int bl_can_recv(uint32_t* id, uint8_t* data, uint8_t* dlc);
