#ifndef FLAREFIRMWARE_SENSOR_DATA_H
#define FLAREFIRMWARE_SENSOR_DATA_H

#include <atomic>

struct MpptInputMeasurements
{
    std::atomic<float> voltage{0.0f};
    std::atomic<float> current{0.0f};
};

struct MpptOutputMeasurements
{
    std::atomic<float> voltage{0.0f};
    std::atomic<float> current{0.0f};
};

struct MpptState
{
    MpptInputMeasurements input;
    MpptOutputMeasurements output;
};

inline MpptState mppt1;
inline MpptState mppt2;
inline MpptState mppt3;

#endif  //FLAREFIRMWARE_SENSOR_DATA_H
