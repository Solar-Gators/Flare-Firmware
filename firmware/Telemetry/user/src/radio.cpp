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

constexpr uint8_t FRAME_START = 0x02;
constexpr uint8_t FRAME_END = 0x03;
constexpr uint8_t FRAME_ESCAPE = 0x1B;

constexpr size_t RAW_BODY_MAX_SIZE =
    sizeof(uint8_t) + sizeof(uint32_t) +
    max_radio_message_array_size;  // just the size byte + id + data
constexpr size_t ESCAPED_FRAME_MAX_SIZE =
    2 + (RAW_BODY_MAX_SIZE * 2);  // start + escaped body + end

// Constructor wires the radio object to UART2 once for this task.
sg::Rfd900 radio(&huart2);
}  // namespace

void radioInit()
{
    // radio.enterLocalATCommandMode(); // we probably don't wanna call this? we wanna be in data mode not command (at) mode
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

// TODO: MAKE THIS SHITE CLEANER
// TODO: add checksum
void radioSend(const RadioMessage& msg)
{
    if (msg.size > max_radio_message_array_size)
    {
        return;
    }

    // first construct raw bytes of data (no start/stop/escape)
    std::array<uint8_t, RAW_BODY_MAX_SIZE> raw{};
    size_t raw_len = 0;

    // Body format: [size (1)] [id (4, little-endian)] [payload (size)]
    raw[raw_len++] = static_cast<uint8_t>(msg.size);
    raw[raw_len++] = static_cast<uint8_t>(msg.id & 0xFF);
    raw[raw_len++] = static_cast<uint8_t>((msg.id >> 8) & 0xFF);
    raw[raw_len++] = static_cast<uint8_t>((msg.id >> 16) & 0xFF);
    raw[raw_len++] = static_cast<uint8_t>((msg.id >> 24) & 0xFF);

    for (size_t i = 0; i < msg.size; i++)
    {
        raw[raw_len++] = msg.data[i];
    }

    // now construct escape
    std::array<uint8_t, ESCAPED_FRAME_MAX_SIZE> framed{};
    size_t framed_len = 0;

    framed[framed_len++] = FRAME_START;

    for (size_t i = 0; i < raw_len; i++)
    {
        uint8_t byte = raw[i];
        if (byte == FRAME_START || byte == FRAME_END || byte == FRAME_ESCAPE)
        {
            framed[framed_len++] = FRAME_ESCAPE;
        }
        framed[framed_len++] = byte;
    }

    framed[framed_len++] = FRAME_END;

    radio.sendData(framed.data(), framed_len);
}
