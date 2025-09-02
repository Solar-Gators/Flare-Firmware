#pragma once
#include "CanDriverApi.hpp"  // only typedefs / C-safe stuff

class CANDevice
{
   public:
    static CANDevice& getInstance()
    {
        static CANDevice inst;
        return inst;
    }

    HAL_StatusTypeDef StartCANDevice(CanHandle_t* hcan);
    void printCANTest();

   private:
    CanHandle_t* hcan = nullptr;
    CANDevice() = default;

    CANDevice(const CANDevice&) = delete;
    CANDevice& operator=(const CANDevice&) = delete;
};