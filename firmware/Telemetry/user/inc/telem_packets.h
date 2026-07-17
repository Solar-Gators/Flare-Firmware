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

// Synthetic packet IDs for radio-only messages that don't originate from a CAN
// frame. Kept above the 11-bit standard CAN id range (> 0x7FF) so they can never
// collide with a forwarded CAN frame id on the ground-station side.
enum : uint32_t
{
    TELEM_ID_GPS = 0x10000000,
    TELEM_ID_MPPT1 = 0x10000001,
    TELEM_ID_MPPT2 = 0x10000002,
    TELEM_ID_MPPT3 = 0x10000003,
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

#endif  //FLAREFIRMWARE_TELEM_PACKETS_H
