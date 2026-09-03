//
// Unified telemetry radio packets.
//
// Every message sent over the radio is a tagged byte blob: enqueueRadioMessage()
// is the single sink, and radioSend() serializes it as [size][id:4 LE][payload].
// CAN frames use their CAN id directly; synthetic sources (GPS, MPPT) define a
// wire struct here and a typed packer that forwards it through the same sink.
//

#ifndef FLAREFIRMWARE_TELEM_PACKETS_H
#define FLAREFIRMWARE_TELEM_PACKETS_H

#include <cstdint>

#include "radio.h"

// Packet IDs
enum : uint32_t
{
    TELEM_ID_MPPT1 = 0x600,  // matches MPPT1_BASE_ADDR
    TELEM_ID_MPPT2 = 0x610,  // matches MPPT2_BASE_ADDR
    TELEM_ID_MPPT3 = 0x620,  // matches MPPT3_BASE_ADDR

    TELEM_ID_GPS = 0x10000000,
    TELEM_ID_RADIO_STATS = 0x10000001,
};

struct __attribute__((packed)) GpsPacket
{
    double latitude;         // 8
    double longitude;        // 8
    float speed;             // 4
    uint8_t num_satellites;  // 1  -> 21 bytes
};
static_assert(sizeof(GpsPacket) <= max_radio_message_array_size,
              "GpsPacket exceeds radio payload size");

struct __attribute__((packed)) MpptPacket
{
    float input_voltage;   // 4
    float input_current;   // 4
    float output_voltage;  // 4
    float output_current;  // 4  -> 16 bytes
};
static_assert(sizeof(MpptPacket) <= max_radio_message_array_size,
              "MpptPacket exceeds radio payload size");

struct __attribute__((packed)) RadioStatsPacket
{
    uint16_t queue_used;        // 2  messages waiting in the TX queue right now
    uint8_t queue_capacity;     // 1  TX queue depth
    uint16_t queue_high_water;  // 2  peak queue_used since boot
    uint32_t enqueued;          // 4  total frames queued
    uint32_t dropped;           // 4  total frames dropped (queue full)
    uint32_t sent;              // 4  total frames transmitted over UART
    uint16_t mean_interval_ms;  // 2  mean gap between sent frames over last window
};  // -> 19 bytes
static_assert(sizeof(RadioStatsPacket) <= max_radio_message_array_size,
              "RadioStatsPacket exceeds radio payload size");

inline bool enqueueTelemPacket(uint32_t id, const void* packet, uint8_t len)
{
    return enqueueRadioMessage(id, static_cast<const uint8_t*>(packet), len);
}

inline bool enqueueGpsData(const GpsPacket& packet)
{
    return enqueueTelemPacket(TELEM_ID_GPS, &packet, sizeof(packet));
}

inline bool enqueueMpptData(uint32_t id, const MpptPacket& packet)
{
    return enqueueTelemPacket(id, &packet, sizeof(packet));
}

inline bool enqueueRadioStats(const RadioStatsPacket& packet)
{
    return enqueueTelemPacket(TELEM_ID_RADIO_STATS, &packet, sizeof(packet));
}

#endif  //FLAREFIRMWARE_TELEM_PACKETS_H
