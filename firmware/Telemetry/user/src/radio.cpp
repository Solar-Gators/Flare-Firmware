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
#include <atomic>

extern UART_HandleTypeDef huart2;

namespace
{
osMessageQueueId_t queue;

constexpr uint32_t RADIO_QUEUE_DEPTH = 32;

// Radio-link health counters (see radioGetStats). Relaxed atomics: producers run
// in the CAN RX task, the consumer in the radio TX task, and the reader in the
// telemetry task; we only need coherent counts, not ordering.
std::atomic<uint32_t> stat_enqueued{0};
std::atomic<uint32_t> stat_dropped{0};
std::atomic<uint32_t> stat_sent{0};
std::atomic<uint16_t> stat_queue_high_water{0};

// Pre-COBS body layout: [can_id (4 LE)][payload size (2 LE)][payload][crc (2 LE)]
constexpr size_t RADIO_ID_SIZE = sizeof(uint32_t);
constexpr size_t RADIO_LEN_SIZE = sizeof(uint16_t);
constexpr size_t RADIO_CRC_SIZE = sizeof(uint16_t);
constexpr size_t RADIO_BODY_MAX_SIZE =
    RADIO_ID_SIZE + RADIO_LEN_SIZE + max_radio_message_array_size + RADIO_CRC_SIZE;

constexpr size_t COBS_FRAME_MAX_SIZE = RADIO_BODY_MAX_SIZE + (RADIO_BODY_MAX_SIZE / 254) + 2;

uint16_t crc16Ccitt(const uint8_t* data, size_t length)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++)
    {
        crc ^= static_cast<uint16_t>(static_cast<uint16_t>(data[i]) << 8);
        for (int bit = 0; bit < 8; bit++)
        {
            crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                                 : static_cast<uint16_t>(crc << 1);
        }
    }
    return crc;
}

size_t cobsEncode(const uint8_t* input, size_t length, uint8_t* output)
{
    size_t code_index = 0;
    size_t write_index = 1;
    uint8_t code = 1;

    for (size_t read_index = 0; read_index < length; read_index++)
    {
        if (input[read_index] != 0)
        {
            output[write_index++] = input[read_index];
            code++;
        }
        if (input[read_index] == 0 || code == 0xFF)
        {
            output[code_index] = code;
            code = 1;
            code_index = write_index;
            if (input[read_index] == 0 || read_index + 1 < length)
            {
                write_index++;
            }
        }
    }
    output[code_index] = code;
    return write_index;
}

// Constructor wires the radio object to UART2 once for this task.
sg::Rfd900 radio(&huart2);
}  // namespace

void radioInit()
{
    // radio.enterLocalATCommandMode(); // we probably don't wanna call this? we wanna be in data mode not command (at) mode
    queue = osMessageQueueNew(RADIO_QUEUE_DEPTH, sizeof(RadioMessage), nullptr);
    if (!queue)
    {
        Error_Handler();
    }
}

bool enqueueRadioMessage(uint32_t id, const uint8_t* data, uint8_t len)
{
    if (len > max_radio_message_array_size)
    {
        return false;
    }

    RadioMessage msg{};
    msg.size = len;
    msg.id = id;
    std::copy_n(data, len, msg.data.data());

    // Best-effort, non-blocking: if the radio can't keep up we drop the frame and
    // count it rather than block the CAN RX task (which would stall CAN dispatch).
    if (osMessageQueuePut(queue, &msg, 0, 0) != osOK)
    {
        stat_dropped.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    stat_enqueued.fetch_add(1, std::memory_order_relaxed);

    // Track the peak queue occupancy so the diagnostics frame can report how close
    // to saturation we got, even if it has drained by the time stats are sampled.
    uint16_t used = static_cast<uint16_t>(osMessageQueueGetCount(queue));
    uint16_t peak = stat_queue_high_water.load(std::memory_order_relaxed);
    while (used > peak &&
           !stat_queue_high_water.compare_exchange_weak(peak, used, std::memory_order_relaxed))
    {
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
    if (msg.size > max_radio_message_array_size)
    {
        return;
    }

    std::array<uint8_t, RADIO_BODY_MAX_SIZE> body{};
    size_t body_len = 0;

    body[body_len++] = static_cast<uint8_t>(msg.id & 0xFF);
    body[body_len++] = static_cast<uint8_t>((msg.id >> 8) & 0xFF);
    body[body_len++] = static_cast<uint8_t>((msg.id >> 16) & 0xFF);
    body[body_len++] = static_cast<uint8_t>((msg.id >> 24) & 0xFF);

    body[body_len++] = static_cast<uint8_t>(msg.size & 0xFF);
    body[body_len++] = static_cast<uint8_t>((msg.size >> 8) & 0xFF);

    for (size_t i = 0; i < msg.size; i++)
    {
        body[body_len++] = msg.data[i];
    }

    uint16_t crc = crc16Ccitt(body.data(), body_len);
    body[body_len++] = static_cast<uint8_t>(crc & 0xFF);
    body[body_len++] = static_cast<uint8_t>((crc >> 8) & 0xFF);

    std::array<uint8_t, COBS_FRAME_MAX_SIZE> frame{};
    size_t frame_len = cobsEncode(body.data(), body_len, frame.data());
    frame[frame_len++] = 0x00;  // frame delimiter

    radio.sendData(frame.data(), frame_len);
    stat_sent.fetch_add(1, std::memory_order_relaxed);
}

RadioStats radioGetStats()
{
    RadioStats stats{};
    stats.queue_used = queue ? static_cast<uint16_t>(osMessageQueueGetCount(queue)) : 0;
    stats.queue_capacity = queue ? static_cast<uint16_t>(osMessageQueueGetCapacity(queue)) : 0;
    stats.queue_high_water = stat_queue_high_water.load(std::memory_order_relaxed);
    stats.enqueued = stat_enqueued.load(std::memory_order_relaxed);
    stats.dropped = stat_dropped.load(std::memory_order_relaxed);
    stats.sent = stat_sent.load(std::memory_order_relaxed);
    return stats;
}
