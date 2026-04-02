//
// Created by justin on 4/1/26.
//

#include "radio.h"

#include "cmsis_os2.h"

#include "FreeRTOSConfig.h"
#include "main.h"
#include "projdefs.h"
#include "rfd900x.h"

#include <algorithm>
#include <array>

extern UART_HandleTypeDef huart2;

namespace
{
osMessageQueueId_t queue;

// Constructor wires the radio object to UART2 once for this task.
sg::Rfd900 radio(&huart2);
}  // namespace

void radioInit()
{
    radio.enterLocalATCommandMode();
    queue = osMessageQueueNew(10, sizeof(RadioMessage), nullptr);
    if (!queue)
    {
        Error_Handler();
    }
}

bool addCanMessageToRadioQueue(uint32_t id, const uint8_t* data, uint8_t len)
{
    RadioMessage msg{};
    msg.size = len;
    msg.id = id;
    std::copy(data, data + len, msg.data.data());

    if (osMessageQueuePut(queue, &msg, 0, 100) != osOK)
    {
        return false;
    }

    return true;
}

bool addGpsDataToRadioQueue(
    const double& latitude,
    const double& longitude,
    float speed,
    uint8_t
        num_satellites)  // if we weren't trash this would be like a gps packet or something not a bunch of params
                         // gps packet could be defined in the max10 files
{
    RadioMessage msg{};
    msg.size = 21;
    msg.id = 0xFFFFFFFF;  // gps packets id
    std::copy_n(
        reinterpret_cast<const uint8_t*>(&latitude),
        sizeof(latitude),
        msg.data
            .data());  // this is so bad but we have to send the number of satellites in the same packet as the gps data for it to be useful and we only have 21 bytes to work with so we have to do this hacky stuff
    std::copy_n(reinterpret_cast<const uint8_t*>(&longitude),
                sizeof(longitude),
                msg.data.data() + sizeof(latitude));
    std::copy_n(reinterpret_cast<const uint8_t*>(&speed),
                sizeof(speed),
                msg.data.data() + sizeof(latitude) + sizeof(longitude));
    // msg.data[20] = num_satellites;
    msg.data.back() = num_satellites;

    if (osMessageQueuePut(queue, &msg, 0, 100) != osOK)
    {
        return false;
    }

    return true;
}

RadioMessage waitForRadioMessageData()
{
    RadioMessage msg{};
    if (osMessageQueueGet(queue, &msg, nullptr, osWaitForever) != osOK)
    {
        Error_Handler();
    }
    return msg;
}

void radioSend(const RadioMessage& msg)
{
    static std::array<uint8_t, max_radio_message_array_size + sizeof(uint32_t)> buffer{};

    // fill buffer
    // pack id into first 4 bytes
    buffer[0] = msg.id & 0xFF;
    buffer[1] = (msg.id >> 8) & 0xFF;
    buffer[2] = (msg.id >> 16) & 0xFF;
    buffer[3] = (msg.id >> 24) & 0xFF;

    for (size_t i = 0; i < msg.size; i++)
    {
        buffer[4 + i] = msg.data[i];
    }

    radio.sendData(buffer.data(), msg.size + sizeof(uint32_t));
}
