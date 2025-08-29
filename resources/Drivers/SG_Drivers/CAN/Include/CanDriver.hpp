#include "CanDriverApi.hpp"

class CANDevice {
public:

    static CANDevice& getInstance() {
        static CANDevice CANDevice;
        return CANDevice;
    }

    HAL_StatusTypeDef StartCANDevice(CAN_HandleTypeDef *hcan);

    void printCANTest();

private:

    CAN_HandleTypeDef *hcan;

    CANDevice(){}

public:
    CANDevice(CANDevice const&)         = delete;
    void operator=(CANDevice const&)    = delete;
};

