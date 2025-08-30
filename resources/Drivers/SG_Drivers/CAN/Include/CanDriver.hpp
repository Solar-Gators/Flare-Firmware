#include "CanDriverApi.hpp"

class CANDevice {
public:

    static CANDevice& getInstance() {
        static CANDevice CANDevice;
        return CANDevice;
    }

    HAL_StatusTypeDef StartCANDevice(CanHandle_t *hcan);

    void printCANTest();

private:

    CanHandle_t *hcan;

    CANDevice(){}

public:
    CANDevice(CANDevice const&)         = delete;
    void operator=(CANDevice const&)    = delete;
};

