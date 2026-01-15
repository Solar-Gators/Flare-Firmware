/**
 * @file CanDriver.hpp
 * @author Jonathon Brown (jonathonb18b@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-08 
 */

#pragma once
#include <string.h>

#include "CanDriverApi.hpp"
#include "FreeRTOS.h"

#include <vector>

namespace sg
{

#define THREAD_PRIORITY osPriorityAboveNormal /* Priority of Rx and Tx threads */

#define NUM_FILTER_BANKS 14
#define NUM_CAN_CALLBACKS 16

#define THREAD_STACK_SIZE_WORDS 512

#define TX_QUEUE_SIZE 5 /* Size of Tx message queue */
#define RX_QUEUE_SIZE 5 /* Size of Tx message queue */
#define TX_TIMEOUT 10   /* Timeout for tx thread in ms */

#define MAX_CAN_ID 0x7FFu

#ifndef CANDEVICE_MAX_BUSES
#define CANDEVICE_MAX_BUSES 2
#endif

class CANFrame
{
   public:
    // no constructor allows aggregate initialization

    void LoadData(uint8_t data[], uint32_t len)
    {
        uint8_t copy_len = max_len < len ? max_len : len;
        memcpy(this->data, data, copy_len);
    }

    uint32_t can_id;          /* CAN ID, can be either 11 bits for standard or 29 for extended */
    CANFrameIDType id_type;   /* CAN ID type */
    CANFrameRTRMode rtr_mode; /* RTR (remote transmission request) mode */
    CANFrameLen len;          /* payload data length */
    uint32_t timestamp;       /* timestamp of last message received */

#if defined(HAL_FDCAN_MODULE_ENABLED)
    uint8_t data[64];           /* payload data array, maximum of 64 bytes */
    const uint8_t max_len = 64; /* maximum payload length */
#else
    uint8_t data[8];           /* payload data array, maximum of 8 bytes */
    const uint8_t max_len = 8; /* maximum payload length */
#endif
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
     * @param id_type   Identifier type: use sg::CANFrameIDType::STANDARD for standard (11-bit) or
     *                  sg::CANFrameIDType::EXTENDED for extended (29-bit) frames.
     * @param rtr_mode  Frame type: data frame or remote transmission request (RTR). Use sg::CANFrameRTRMode::...
     *                  Note: in FDCAN, RTR matching is configured globally, not per-filter.
     * @param priority  FIFO assignment: sg::CANFramePriority::High or sg::CANFramePriority::Low. Chooses hardware FIFO to be routed into.
     *
     * @return HAL_OK if the filter was successfully added, or an error/status code if not.
     */
    HAL_StatusTypeDef AddFilterId(uint32_t can_id,
                                  CANFrameIDType id_type,
                                  CANFrameRTRMode rtr_mode,
                                  CANFramePriority priority);

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
                                     sg::CANFrameIDType id_type,
                                     sg::CANFrameRTRMode rtr_mode,
                                     sg::CANFramePriority priority);

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
    bool addCallbackId(uint32_t can_id,
                       sg::CANFrameIDType id_type,
                       CanCallback cb,
                       void* ctx = nullptr);

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
                          sg::CANFrameIDType id_type,
                          CanCallback cb,
                          void* ctx = nullptr);

    void addCallbackAll(CanCallback cb);

    /**
     * @brief Sends a CANFrame message.
     * @details Adds a CANFrame to a FreeRTOS queue object to be later consumed and sent by the background tx thread.
     * @param msg message to be sent, CANDevice stores a copy of it in a buffer
     * @return HAL error code, HAL_OK if pushed to queue successfully
     */
    HAL_StatusTypeDef Send(const CANFrame& msg);

    // should be unused by user
    static HAL_StatusTypeDef RxCallback(CanHandle_t* hcan);

    // one instance should be tied to each can peripheral
    CANDevice(const CANDevice&) = delete;
    CANDevice& operator=(const CANDevice&) = delete;
    CANDevice(CANDevice&&) = delete;
    CANDevice& operator=(CANDevice&&) = delete;

   private:
    CanHandle_t* hcan_ = nullptr;

    std::vector<CanFilter_t> filters_;
    std::vector<IdEntry> idCallbacks_;
    std::vector<RangeEntry> rangeCallbacks_;
    CanCallback allCallback_ = nullptr;

    osMessageQueueId_t tx_queue_;  // = osMessageQueueNew(TX_QUEUE_SIZE, sizeof(CANFrame*), NULL);
    osMessageQueueId_t rx_queue_;

    struct Entry
    {
        CanHandle_t* h;
        CANDevice* dev;
    };
    inline static Entry s_registry_[CANDEVICE_MAX_BUSES] = {};

    static CANDevice* findByHandle(CanHandle_t* h);
    static bool registerHandle(CanHandle_t* h, CANDevice* d);
    static void unregisterHandle(CanHandle_t* h);
    const CanCallback* find_by_id(uint32_t id);
    const CanCallback* find_by_range(uint32_t id);

    // ====== Tx and Rx Methods ======

    static void HandleRxTrampoline(void* arg);
    [[noreturn]] void HandleRx();

    static void HandleTxTrampoline(void* arg);
    [[noreturn]] void HandleTx();

    osThreadId_t rx_task_handle;
    alignas(8) uint32_t rx_task_stack[THREAD_STACK_SIZE_WORDS];

    StaticTask_t tx_tcb;
    StaticTask_t rx_tcb;

    const osThreadAttr_t rx_task_attributes_ = {
        .name = "CAN Rx Task",
        .attr_bits = osThreadDetached,
        .cb_mem = &rx_tcb,
        .cb_size = sizeof(rx_tcb),
        .stack_mem = rx_task_stack,
        .stack_size = sizeof(rx_task_stack),
        .priority = osPriorityHigh,
    };

    osThreadId_t tx_task_handle;
    alignas(8) uint32_t tx_task_stack[THREAD_STACK_SIZE_WORDS];

    const osThreadAttr_t tx_task_attributes_ = {
        .name = "CAN Tx Task",
        .attr_bits = osThreadDetached,
        .cb_mem = &tx_tcb,
        .cb_size = sizeof(tx_tcb),
        .stack_mem = tx_task_stack,
        .stack_size = sizeof(tx_task_stack),
        .priority = osPriorityAboveNormal,
    };
};

};  // namespace sg
