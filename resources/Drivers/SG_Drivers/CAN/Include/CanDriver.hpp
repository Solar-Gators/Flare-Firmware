#pragma once
#include "cmsis_os2.h"

#include "CanDriverApi.hpp"

#include <vector>

#define THREAD_PRIORITY osPriorityAboveNormal /* Priority of Rx and Tx threads */

#define NUM_FILTER_BANKS 14

#ifndef CANDEVICE_MAX_BUSES
#define CANDEVICE_MAX_BUSES 2
#endif

struct CANMsg
{
    CanHandle_t* hcan;
    uint32_t id;
    bool id_type;
    bool rtr;
    uint8_t dlc;
    uint32_t timestamp;
#if defined(HAL_FDCAN_MODULE_ENABLED)
    uint8_t data[64];
#else
    uint8_t data[8];
#endif
};

using CanCallback = HAL_StatusTypeDef (*)(const CANMsg& msg, void* ctx);

/**
 * @brief Singleton CAN device controller
 *
 */
class CANDevice
{
   public:
    // static CANDevice& getInstance()
    // {
    //     static CANDevice inst;
    //     return inst;
    // }

    explicit CANDevice(CanHandle_t* hcan);

    /*!
     * @brief Loads filters and starts physical CAN device
     *
     * @param hcan Handle to CAN device
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef StartCANDevice();

    /*!
     * @brief Adds filter to accept single CAN Id
     *
     * @param can_id    CAN Id of incoming message
     * @param id_type   Standard or Extended Message
     * @param rtr_mode  Data or RTR frame
     * @param priority  High or Low priority -> fifo0 or fifo1
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef AddFilterId(uint32_t can_id,
                                  uint32_t id_type,
                                  uint32_t rtr_mode,
                                  uint32_t priority);

    /*!
     * @brief
     *
     * @param can_id    CAN Id of incoming message
     * @param range     Range of accepted messages starting from can_id
     * @param id_type   Standard or Extended Message
     * @param rtr_mode  Data or RTR frame
     * @param priority  High or Low priority -> fifo0 or fifo1
     * @return HAL_StatusTypeDef
     */
    HAL_StatusTypeDef AddFilterRange(uint32_t can_id,
                                     uint32_t range,
                                     uint32_t id_type,
                                     uint32_t rtr_mode,
                                     uint32_t priority);

    HAL_StatusTypeDef addCallbackId(uint32_t can_id,
                                    uint32_t id_type,
                                    CanCallback cb,
                                    void* ctx = nullptr);

    HAL_StatusTypeDef addCallbackRange(uint32_t start_id,
                                       uint32_t range,
                                       uint32_t id_type,
                                       CanCallback cb,
                                       void* ctx = nullptr);

    static HAL_StatusTypeDef RxCallback(CanHandle_t* hcan);

   private:
    CanHandle_t* hcan = nullptr;
    std::vector<CanFilter_t> filters_;

    CANDevice(const CANDevice&) = delete;
    CANDevice& operator=(const CANDevice&) = delete;

    static void HandleRxTrampoline(void* arg);
    void HandleRxLoop();

    struct Entry
    {
        CanHandle_t* h;
        CANDevice* dev;
    };
    inline static Entry s_registry_[CANDEVICE_MAX_BUSES] = {};

    static CANDevice* findByHandle(CanHandle_t* h);
    static bool registerHandle(CanHandle_t* h, CANDevice* d);
    static void unregisterHandle(CanHandle_t* h);

    inline static osThreadId_t rx_task_handle;
    alignas(8) inline static uint32_t rx_task_stack[512];

    static constexpr osThreadAttr_t rx_task_attributes_ = {
        .name = "CAN Rx Task",
        .attr_bits = osThreadDetached,
        .stack_mem = rx_task_stack,
        .stack_size = sizeof(rx_task_stack),
        .priority = osPriorityAboveNormal,
    };
};