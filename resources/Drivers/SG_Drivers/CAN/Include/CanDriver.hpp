#pragma once
#include "CanDriverApi.hpp"

#include <vector>

#define NUM_FILTER_BANKS 14

class CANDevice
{
   public:
    static CANDevice& getInstance()
    {
        static CANDevice inst;
        return inst;
    }

    static HAL_StatusTypeDef StartCANDevice(CanHandle_t* hcan);
    static HAL_StatusTypeDef AddFilterId(uint32_t can_id,
                                         uint32_t id_type,
                                         uint32_t rtr_mode,
                                         uint32_t priority);
    static HAL_StatusTypeDef AddFilterRange(
        uint32_t can_id, uint32_t range, uint32_t id_type, uint32_t rtr_mode, uint32_t priority);

    static HAL_StatusTypeDef RxCallback(CanHandle_t* hcan);
    void printCANTest();

   private:
    CanHandle_t* hcan = nullptr;
    std::vector<CanFilter_t> filters_;

    CANDevice() = default;

    CANDevice(const CANDevice&) = delete;
    CANDevice& operator=(const CANDevice&) = delete;
};