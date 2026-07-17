//
// Created by justin on 4/1/26.
//

#ifndef FLAREFIRMWARE_RADIO_H
#define FLAREFIRMWARE_RADIO_H

#include "main.h"

#include <array>

constexpr size_t max_radio_message_array_size = 21;
// 21 because thats the max packet we can send
// 8 (long) + 8 (lat) + 4 (speed) + 1 (satellites?)

struct RadioMessage
{
    uint32_t id{};   // id of the radio message
    uint8_t size{};  // how many bytes in this array are we actually using
    std::array<uint8_t, max_radio_message_array_size> data{};  // this is so fragile im kms
};

void radioInit();
bool enqueueRadioMessage(uint32_t id, const uint8_t* data, uint8_t len);
RadioMessage waitForRadioMessageData();
void radioSend(const RadioMessage& msg);

#endif  //FLAREFIRMWARE_RADIO_H
