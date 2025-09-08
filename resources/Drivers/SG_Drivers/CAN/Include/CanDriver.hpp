/**
 * @file CanDriver.hpp
 * @author Jonathon Brown (jonathonb18b@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-08 
 */

#pragma once
#include <string.h>

#include "cmsis_os2.h"

#include "CanDriverApi.hpp"

#include <queue>
#include <vector>

namespace CANDriver
{

#define THREAD_PRIORITY osPriorityAboveNormal /* Priority of Rx and Tx threads */

#define NUM_FILTER_BANKS 14
#define NUM_CAN_CALLBACKS 16

#define TX_QUEUE_SIZE 3 /* Size of Tx message queue */
#define TX_TIMEOUT 10   /* Timeout for tx thread in ms */

#define MAX_CAN_ID 0x7FFu

#ifndef CANDEVICE_MAX_BUSES
#define CANDEVICE_MAX_BUSES 2
#endif

// struct CANMsg
// {
//     CanHandle_t* hcan;
//     uint32_t id;
//     bool id_type;
//     bool rtr;
//     uint8_t dlc;
//     uint32_t timestamp;
// #if defined(HAL_FDCAN_MODULE_ENABLED)
//     uint8_t data[64];
// #else
//     uint8_t data[8];
// #endif
// };

class CANFrame
{
   public:
    CANFrame(uint32_t can_id, uint32_t id_type, uint32_t rtr_mode, uint32_t len)
        : can_id(can_id), id_type(id_type), rtr_mode(rtr_mode), len(len)
    {
        const osMutexAttr_t attr = {
            .name = "CANFrameMutex",
            .attr_bits = osMutexRecursive | osMutexPrioInherit,
            .cb_mem = nullptr,
            .cb_size = 0,
        };
        mutex_id_ = osMutexNew(&attr);
    };

    CANFrame()
    {
        const osMutexAttr_t attr = {
            .name = "CANFrameMutex",
            .attr_bits = osMutexRecursive | osMutexPrioInherit,
            .cb_mem = nullptr,
            .cb_size = 0,
        };
        mutex_id_ = osMutexNew(&attr);
    }

    inline osStatus_t Lock(uint32_t timeout = osWaitForever)
    {
        return osMutexAcquire(mutex_id_, timeout);
    }

    inline osStatus_t Unlock() { return osMutexRelease(mutex_id_); }

    void LoadData(uint8_t data[], uint32_t len)
    {
        uint8_t copy_len = max_len < len ? max_len : len;
        memcpy(this->data, data, copy_len);
    }

    uint32_t can_id;     /* CAN ID, can be standard or extended */
    uint32_t id_type;    /* CAN ID type, 0 if standard ID, 4 if extended ID */
    uint32_t rtr_mode;   /* RTR mode, 0 if not RTR message, 2 if RTR */
    uint32_t len;        /* payload data length */
    uint32_t timestamp_; /* timestamp of last message received */

#if defined(HAL_FDCAN_MODULE_ENABLED)
    uint8_t data[64];           /* payload data array, maximum of 64 bytes */
    const uint8_t max_len = 64; /* maximum payload length */
#else
    uint8_t data[8];           /* payload data array, maximum of 8 bytes */
    const uint8_t max_len = 8; /* maximum payload length */
#endif

    osMutexId_t mutex_id_; /* CMSIS-RTOS2 mutex handle */
};

using CanCallback = HAL_StatusTypeDef (*)(const CANFrame& msg, void* ctx);

struct IdEntry
{
    uint32_t id;
    CanCallback cb;
};
struct RangeEntry
{
    uint32_t start, end;
    CanCallback cb;
};

/**
 * @brief CAN device controller
 *
 */
class CANDevice
{
   public:
    /*!
     * @brief Construct a new CANDevice object.
     *
     * @details Creates a CANDevice wrapper around a specific CAN peripheral handle.
     *          The handle is stored internally and used for all subsequent operations.
     *
     * @param hcan Pointer to the HAL CAN/FDCAN handle for this device.
     *             This handle must remain valid for the lifetime of the CANDevice,
     *             and cannot be reused to construct another CANDevice.
     */
    explicit CANDevice(CanHandle_t* hcan);

    /*!
     * @brief Loads configured filters and starts the physical CAN device.
     *
     * @details Initializes the underlying CAN peripheral using the stored handle,
     *          applies any configured filters, and transitions the device into
     *          the operational state so that it can transmit and receive messages.
     *
     * @return HAL_OK on success, or an appropriate HAL error/status code if startup fails.
     */
    HAL_StatusTypeDef StartCANDevice();

    /*!
     * @brief Adds a hardware filter to accept a single CAN identifier.
     *
     * @details Configures the CAN hardware filter bank to accept frames with the
     *          given identifier and characteristics. The frame is then routed into
     *          the specified FIFO for reception.
     *
     * @param can_id    CAN identifier to filter on (11-bit or 29-bit depending on @p id_type).
     * @param id_type   Identifier type: use CAN_ID_STD for standard (11-bit) or
     *                  CAN_ID_EXT for extended (29-bit) frames.
     * @param rtr_mode  Frame type: data frame or remote transmission request (RTR).
     *                  Note: in FDCAN, RTR matching is configured globally, not per-filter.
     * @param priority  FIFO assignment: typically 0 = high priority (FIFO0),
     *                  1 = low priority (FIFO1).
     *
     * @return HAL_OK if the filter was successfully added, or an error/status code if not.
     */
    HAL_StatusTypeDef AddFilterId(uint32_t can_id,
                                  uint32_t id_type,
                                  uint32_t rtr_mode,
                                  uint32_t priority);

    /*!
     * @brief Adds a hardware filter to accept a range of CAN identifiers.
     *
     * @details Configures the CAN hardware filter bank to accept all identifiers
     *          from @p can_id up to (@p can_id + @p range). Matching frames are
     *          routed into the specified FIFO for reception.
     *
     * @param can_id    Starting CAN identifier of the accepted range (inclusive).
     * @param range     Range size. The filter accepts all IDs in
     *                  [can_id, can_id + range].
     * @param id_type   Identifier type: use CAN_ID_STD for standard (11-bit) or
     *                  CAN_ID_EXT for extended (29-bit) frames.
     * @param rtr_mode  Frame type: data frame or remote transmission request (RTR).
     *                  Note: in FDCAN, RTR matching is configured globally, not per-filter.
     * @param priority  FIFO assignment: typically 0 = high priority (FIFO0),
     *                  1 = low priority (FIFO1).
     *
     * @return HAL_OK if the filter was successfully added, or an error/status code if not.
     */
    HAL_StatusTypeDef AddFilterRange(uint32_t can_id,
                                     uint32_t range,
                                     uint32_t id_type,
                                     uint32_t rtr_mode,
                                     uint32_t priority);

    /*!
     * @brief Adds a callback function for a single CAN identifier.
     *
     * @details Registers a callback that will be invoked whenever a CAN frame with
     *          the specified identifier and type is received.
     *
     * @param can_id   The CAN identifier to match against (11-bit or 29-bit depending on @p id_type).
     * @param id_type  The type of identifier. Typically CAN_STD_ID for standard (11-bit) or
     *                 CAN_EXT_ID for extended (29-bit). Used to distinguish how @p can_id is interpreted.
     * @param cb       The callback function to be invoked when a matching frame is received.
     *                 Must conform to the CanCallback signature.
     * @param ctx      Optional user context pointer. Passed back to the callback when invoked
     *                 to allow per-registration state or user data.
     *
     * @return true  If the callback was successfully registered.
     * @return false If registration failed (e.g., maximum number of callbacks reached).
     */
    bool addCallbackId(uint32_t can_id, uint32_t id_type, CanCallback cb, void* ctx = nullptr);

    /*!
     * @brief Adds a callback function for a range of CAN identifiers.
     *
     * @details Registers a callback that will be invoked whenever a CAN frame falls
     *          within the specified identifier range and type.
     *
     * @param start_id The starting CAN identifier for the range (inclusive).
     * @param range    The size of the identifier range. The effective range will be
     *                 [start_id, start_id + range].
     * @param id_type  The type of identifiers in this range. Typically CAN_STD_ID for standard (11-bit)
     *                 or CAN_EXT_ID for extended (29-bit).
     * @param cb       The callback function to be invoked when a frame within the range is received.
     *                 Must conform to the CanCallback signature.
     * @param ctx      Optional user context pointer. Passed back to the callback when invoked
     *                 to allow per-registration state or user data.
     *
     * @return true  If the callback was successfully registered.
     * @return false If registration failed (e.g., maximum number of callbacks reached).
     */
    bool addCallbackRange(uint32_t start_id,
                          uint32_t range,
                          uint32_t id_type,
                          CanCallback cb,
                          void* ctx = nullptr);

    /*!
     * @brief Finds callback connected to a single id
     *
     * @details Finds registered callback for single CAN Id
     * 
     * @param id    The CAN identifier to match against (11-bit or 29-bit depending on @p id_type).
     * @return const CanCallback* if found, nullptr if no connected callback
     */
    const CanCallback* find_by_id(uint32_t id);

    /*!
     * @brief Finds callback connected to id within range filter
     *
     * @details Finds registered callback for id within range of CAN Identifiers
     * 
     * @param id    The CAN identifier to match against (11-bit or 29-bit depending on @p id_type).
     * @return const CanCallback* if found, nullptr if no connected callback
     */
    const CanCallback* find_by_range(uint32_t id);

    HAL_StatusTypeDef Send(CANFrame* msg);

    static HAL_StatusTypeDef RxCallback(CanHandle_t* hcan);

   private:
    CanHandle_t* hcan_ = nullptr;

    std::vector<CanFilter_t> filters_;
    std::vector<IdEntry> idCallbacks_;
    std::vector<RangeEntry> rangeCallbacks_;

    osMessageQueueId_t tx_queue_ = osMessageQueueNew(TX_QUEUE_SIZE, sizeof(CANFrame*), NULL);

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

};  // namespace CANDriver
