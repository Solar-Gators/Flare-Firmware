/**
 * @file CanDriver.cpp
 * @author Jonathon Brown (jonathonb18b@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-08 
 */

#include "CanDriver.hpp"

namespace sg
{

#define TRY(x)                \
    do                        \
    {                         \
        if ((x) != HAL_OK)    \
            return HAL_ERROR; \
    } while (0)

static inline int CAN_RxFifoLevel(CanHandle_t* h)
{
#if defined(HAL_CAN_MODULE_ENABLED)
    return HAL_CAN_GetRxFifoFillLevel(h, CAN_RX_FIFO0);
#elif defined(HAL_FDCAN_MODULE_ENABLED)
    return static_cast<int>(HAL_FDCAN_GetRxFifoFillLevel(h, FDCAN_RX_FIFO0));
#else
    return 0;
#endif
}

static inline CANFrameLen CAN_DlcToBytes(uint32_t dlc)
{
#if defined(HAL_FDCAN_MODULE_ENABLED)
    switch (dlc)
    {
        case FDCAN_DLC_BYTES_0:
            return sg::CANFrameLen::BYTES_0;
        case FDCAN_DLC_BYTES_1:
            return sg::CANFrameLen::BYTES_1;
        case FDCAN_DLC_BYTES_2:
            return sg::CANFrameLen::BYTES_2;
        case FDCAN_DLC_BYTES_3:
            return sg::CANFrameLen::BYTES_3;
        case FDCAN_DLC_BYTES_4:
            return sg::CANFrameLen::BYTES_4;
        case FDCAN_DLC_BYTES_5:
            return sg::CANFrameLen::BYTES_5;
        case FDCAN_DLC_BYTES_6:
            return sg::CANFrameLen::BYTES_6;
        case FDCAN_DLC_BYTES_7:
            return sg::CANFrameLen::BYTES_7;
        case FDCAN_DLC_BYTES_8:
            return sg::CANFrameLen::BYTES_8;
        case FDCAN_DLC_BYTES_12:
            return sg::CANFrameLen::BYTES_12;
        case FDCAN_DLC_BYTES_16:
            return sg::CANFrameLen::BYTES_16;
        case FDCAN_DLC_BYTES_20:
            return sg::CANFrameLen::BYTES_20;
        case FDCAN_DLC_BYTES_24:
            return sg::CANFrameLen::BYTES_24;
        case FDCAN_DLC_BYTES_32:
            return sg::CANFrameLen::BYTES_32;
        case FDCAN_DLC_BYTES_48:
            return sg::CANFrameLen::BYTES_48;
        case FDCAN_DLC_BYTES_64:
            return sg::CANFrameLen::BYTES_64;
        default:
            return sg::CANFrameLen::BYTES_0;
    }
#else
    if (dlc > 8)
        dlc = 0;
    return static_cast<CANFrameLen>(dlc);  // bxCAN: DLC equals byte length (0..8)
#endif
}

static inline uint32_t CAN_BytesToDlc(CANFrameLen len)
{
#if defined(HAL_FDCAN_MODULE_ENABLED)
    switch (len)
    {
        case sg::CANFrameLen::BYTES_0:
            return FDCAN_DLC_BYTES_0;
        case sg::CANFrameLen::BYTES_1:
            return FDCAN_DLC_BYTES_1;
        case sg::CANFrameLen::BYTES_2:
            return FDCAN_DLC_BYTES_2;
        case sg::CANFrameLen::BYTES_3:
            return FDCAN_DLC_BYTES_3;
        case sg::CANFrameLen::BYTES_4:
            return FDCAN_DLC_BYTES_4;
        case sg::CANFrameLen::BYTES_5:
            return FDCAN_DLC_BYTES_5;
        case sg::CANFrameLen::BYTES_6:
            return FDCAN_DLC_BYTES_6;
        case sg::CANFrameLen::BYTES_7:
            return FDCAN_DLC_BYTES_7;
        case sg::CANFrameLen::BYTES_8:
            return FDCAN_DLC_BYTES_8;
        case sg::CANFrameLen::BYTES_12:
            return FDCAN_DLC_BYTES_12;
        case sg::CANFrameLen::BYTES_16:
            return FDCAN_DLC_BYTES_16;
        case sg::CANFrameLen::BYTES_20:
            return FDCAN_DLC_BYTES_20;
        case sg::CANFrameLen::BYTES_24:
            return FDCAN_DLC_BYTES_24;
        case sg::CANFrameLen::BYTES_32:
            return FDCAN_DLC_BYTES_32;
        case sg::CANFrameLen::BYTES_48:
            return FDCAN_DLC_BYTES_48;
        case sg::CANFrameLen::BYTES_64:
            return FDCAN_DLC_BYTES_64;
        default:
            return FDCAN_DLC_BYTES_0;
    }
#else
    return static_cast<uint32_t>(len);  // bxCAN: DLC equals byte length (0..8)
#endif
}

static inline bool CAN_ReadOne(CanHandle_t* h, CANFrame& out)
{
#if defined(HAL_CAN_MODULE_ENABLED)
    CAN_RxHeaderTypeDef hdr{};
    if (HAL_CAN_GetRxMessage(h, CAN_RX_FIFO0, &hdr, out.data) != HAL_OK)
        return false;
    // out.hcan = h;
    out.can_id = (hdr.IDE == CAN_ID_EXT) ? hdr.ExtId : hdr.StdId;
    out.id_type =
        (hdr.IDE == CAN_ID_EXT) ? sg::CANFrameIDType::EXTENDED : sg::CANFrameIDType::STANDARD;
    out.rtr_mode =
        (hdr.RTR == CAN_RTR_REMOTE) ? sg::CANFrameRTRMode::REMOTE : sg::CANFrameRTRMode::DATA;
    out.len = CAN_DlcToBytes(hdr.DLC);
    out.timestamp = 0;  // bxCAN timestamping not filled here (optional: use TIM if needed)

    return true;

#elif defined(HAL_FDCAN_MODULE_ENABLED)
    FDCAN_RxHeaderTypeDef hdr{};
    if (HAL_FDCAN_GetRxMessage(h, FDCAN_RX_FIFO0, &hdr, out.data) != HAL_OK)
        return false;
    //out.hcan = h;
    out.can_id = hdr.Identifier & ((hdr.IdType == FDCAN_EXTENDED_ID) ? 0x1FFFFFFF : 0x7FF);
    out.id_type = (hdr.IdType == FDCAN_EXTENDED_ID) ? sg::CANFrameIDType::EXTENDED
                                                    : sg::CANFrameIDType::STANDARD;
    out.rtr_mode = (hdr.RxFrameType == FDCAN_REMOTE_FRAME) ? sg::CANFrameRTRMode::REMOTE
                                                           : sg::CANFrameRTRMode::DATA;
    out.len = CAN_DlcToBytes(hdr.DataLength);
    out.timestamp =
        0;  // If timestamping enabled, you can capture from peripheral or a systick here
    return true;
#else
    (void) h;
    (void) out;
    return false;
#endif
}

CANDevice::CANDevice(CanHandle_t* hcan) : hcan_(hcan), tx_queue_(nullptr), rx_queue_(nullptr)
{
    filters_.reserve(NUM_FILTER_BANKS);
    idCallbacks_.reserve(NUM_CAN_CALLBACKS);
    rangeCallbacks_.reserve(NUM_CAN_CALLBACKS);
}

HAL_StatusTypeDef CANDevice::StartCANDevice()
{
    if (!registerHandle(hcan_, this))
    {
        return HAL_ERROR;
    }

    if (!tx_queue_)
    {
        tx_queue_ = osMessageQueueNew(TX_QUEUE_SIZE, sizeof(CANFrame), nullptr);
        if (!tx_queue_)
            return HAL_ERROR;
    }

    if (!rx_queue_)
    {
        rx_queue_ = osMessageQueueNew(RX_QUEUE_SIZE, sizeof(CANFrame), nullptr);
        if (!rx_queue_)
            return HAL_ERROR;
    }

    rx_task_handle = osThreadNew(&CANDevice::HandleRxTrampoline, this, &rx_task_attributes_);
    if (!rx_task_handle)
        return HAL_ERROR;

    tx_task_handle = osThreadNew(&CANDevice::HandleTxTrampoline, this, &tx_task_attributes_);
    if (!tx_task_handle)
        return HAL_ERROR;

#if defined(HAL_CAN_MODULE_ENABLED)
    // ===================== bxCAN =====================
    if (filters_.empty())
    {
        CanFilter_t filter = {};

        filter.FilterActivation = ENABLE;
        filter.FilterBank = 0;
        filter.SlaveStartFilterBank = 14;
        filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
        filter.FilterMode = CAN_FILTERMODE_IDMASK;
        filter.FilterScale = CAN_FILTERSCALE_32BIT;

        // Accept ALL messages (for debug/testing)
        filter.FilterIdHigh = 0x0000;
        filter.FilterIdLow = 0x0000;
        filter.FilterMaskIdHigh = 0x0000;
        filter.FilterMaskIdLow = 0x0000;

        TRY(HAL_CAN_ConfigFilter(hcan_, &filter));
    }
    else
    {
        for (const auto& filter : filters_)
        {
            TRY(HAL_CAN_ConfigFilter(hcan_, &filter));
        }
    }

    TRY(HAL_CAN_ActivateNotification(hcan_, CAN_IT_RX_FIFO0_MSG_PENDING));

    TRY(HAL_CAN_Start(hcan_));

#elif defined(HAL_FDCAN_MODULE_ENABLED)
    // ===================== FDCAN (M_CAN) =====================
    // If there are no filters to add, then accept all messages
    if (filters_.empty())
    {
        CanFilter_t filter = {};

        filter.IdType = FDCAN_STANDARD_ID;
        filter.FilterIndex = 0;
        filter.FilterType = FDCAN_FILTER_RANGE;
        filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
        filter.FilterID1 = 0x0000;
        filter.FilterID2 = MAX_CAN_ID;

        TRY(HAL_FDCAN_ConfigFilter(hcan_, &filter));
    }
    else
    {
        for (const auto& filter : filters_)
        {
            TRY(HAL_FDCAN_ConfigFilter(hcan_, &filter));
        }
    }

    TRY(HAL_FDCAN_ConfigGlobalFilter(
        hcan_, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE));

    TRY(HAL_FDCAN_ActivateNotification(hcan_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0));

    TRY(HAL_FDCAN_Start(hcan_));

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif

    return HAL_OK;
}

HAL_StatusTypeDef CANDevice::AddFilterId(uint32_t can_id,
                                         CANFrameIDType id_type,
                                         CANFrameRTRMode rtr_mode,
                                         CANFramePriority priority)
{
    if (filters_.size() >= NUM_FILTER_BANKS)
        return HAL_ERROR;

#if defined(HAL_CAN_MODULE_ENABLED)
    // ===================== bxCAN =====================
    // Validate & map RTR for bxCAN
    const uint32_t hal_rtr =
        (rtr_mode == sg::CANFrameRTRMode::REMOTE) ? CAN_RTR_REMOTE : CAN_RTR_DATA;

    if (id_type == sg::CANFrameIDType::STANDARD)
    {
        if (can_id > MAX_CAN_ID)
            return HAL_ERROR;

        // Exact match: use IDMASK with all 11 ID bits compared (mask 0x7FF)
        const uint32_t filter_id = ((can_id & MAX_CAN_ID) << 21) | CAN_ID_STD | hal_rtr;
        const uint32_t filter_mask = ((MAX_CAN_ID) << 21) | 0b110;  // also match IDE & RTR

        CAN_FilterTypeDef f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == sg::CANFramePriority::HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
        f.FilterBank = filters_.size();
        f.FilterMode = CAN_FILTERMODE_IDMASK;
        f.FilterScale = CAN_FILTERSCALE_32BIT;
        f.FilterActivation = ENABLE;

        filters_.push_back(f);
        return HAL_OK;
    }
    else if (id_type == sg::CANFrameIDType::EXTENDED)
    {
        if (can_id > 0x1FFFFFFFu)
            return HAL_ERROR;

        // Exact match: use IDMASK with all 29 ID bits compared
        const uint32_t filter_id =
            ((can_id & 0x1FFFFFFFu) << 3) | CAN_ID_EXT | hal_rtr;  // EXT at bit 3
        const uint32_t filter_mask = ((0x1FFFFFFFu) << 3) | 0b110;

        CAN_FilterTypeDef f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == sg::CANFramePriority::HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
        f.FilterBank = filters_.size();
        f.FilterMode = CAN_FILTERMODE_IDMASK;
        f.FilterScale = CAN_FILTERSCALE_32BIT;
        f.FilterActivation = ENABLE;

        filters_.push_back(f);
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }

#elif defined(HAL_FDCAN_MODULE_ENABLED)
    // ===================== FDCAN (M_CAN) =====================

    FDCAN_FilterTypeDef f = {};
    if (id_type == sg::CANFrameIDType::STANDARD)
    {
        if (can_id > MAX_CAN_ID)
            return HAL_ERROR;

        // Easiest exact match on FDCAN: RANGE with start==end
        f.IdType = FDCAN_STANDARD_ID;
        f.FilterType = FDCAN_FILTER_RANGE;  // inclusive
        f.FilterID1 = can_id;
        f.FilterID2 = can_id;
    }
    else if (id_type == sg::CANFrameIDType::EXTENDED)
    {
        if (can_id > 0x1FFFFFFFu)
            return HAL_ERROR;

        f.IdType = FDCAN_EXTENDED_ID;
        f.FilterType = FDCAN_FILTER_RANGE;  // inclusive
        f.FilterID1 = can_id;
        f.FilterID2 = can_id;
    }
    else
    {
        return HAL_ERROR;
    }

    f.FilterIndex = filters_.size();
    f.FilterConfig = (priority == sg::CANFramePriority::HIGH) ? FDCAN_FILTER_TO_RXFIFO0
                                                              : FDCAN_FILTER_TO_RXFIFO1;

    filters_.push_back(f);
    return HAL_OK;

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif
}

HAL_StatusTypeDef CANDevice::AddFilterRange(uint32_t can_id,
                                            uint32_t range,
                                            sg::CANFrameIDType id_type,
                                            sg::CANFrameRTRMode rtr_mode,
                                            sg::CANFramePriority priority)
{
    if (filters_.size() >= NUM_FILTER_BANKS || range == 0)
        return HAL_ERROR;

    // ---------- Compute window alignment ----------
    // n = ceil(log2(range)), id_mask = ~((1<<n)-1) => zero out n LSBs
    uint32_t counter = range;
    int32_t n = -1;
    while (counter)
    {
        counter >>= 1;
        n++;
    }
    if (range > static_cast<uint32_t>(1u << n))
        n++;
    uint32_t id_mask = (n >= 32) ? 0u : ((0xFFFFFFFFu >> n) << n);
    uint32_t base = can_id & id_mask;      // aligned start
    uint32_t end_inc = base + range - 1u;  // inclusive end

#if defined(HAL_CAN_MODULE_ENABLED)
    // ===================== bxCAN =====================

    const uint32_t hal_rtr =
        (rtr_mode == sg::CANFrameRTRMode::REMOTE) ? CAN_RTR_REMOTE : CAN_RTR_DATA;

    if (id_type == sg::CANFrameIDType::STANDARD)
    {
        if (can_id > MAX_CAN_ID)
            return HAL_ERROR;
        if (end_inc > MAX_CAN_ID)
            end_inc = MAX_CAN_ID;

        // bxCAN 32-bit IDMASK packing (STD: ID at bits 31..21)
        uint32_t filter_id = ((base & MAX_CAN_ID) << 21) | CAN_ID_STD | hal_rtr;
        uint32_t filter_mask = ((id_mask & MAX_CAN_ID) << 21) | 0b110;  // match IDE & RTR

        CanFilter_t f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == sg::CANFramePriority::HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
        f.FilterBank = filters_.size();  // ensure unique index upstream or here
        f.FilterMode = CAN_FILTERMODE_IDMASK;
        f.FilterScale = CAN_FILTERSCALE_32BIT;
        f.FilterActivation = ENABLE;

        filters_.push_back(f);
        return HAL_OK;
    }
    else if (id_type == sg::CANFrameIDType::EXTENDED)
    {
        if (can_id > 0x1FFFFFFFu)
            return HAL_ERROR;
        if (end_inc > 0x1FFFFFFFu)
            end_inc = 0x1FFFFFFFu;

        // bxCAN 32-bit IDMASK packing (EXT: ID at bits 31..3)
        uint32_t filter_id = ((base & 0x1FFFFFFFu) << 3) | CAN_ID_EXT | hal_rtr;
        uint32_t filter_mask = ((id_mask & 0x1FFFFFFFu) << 3) | 0b110;  // match IDE & RTR

        CanFilter_t f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == sg::CANFramePriority::HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
        f.FilterBank = filters_.size();
        f.FilterMode = CAN_FILTERMODE_IDMASK;
        f.FilterScale = CAN_FILTERSCALE_32BIT;
        f.FilterActivation = ENABLE;

        filters_.push_back(f);
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }

#elif defined(HAL_FDCAN_MODULE_ENABLED)
    // ===================== FDCAN (M_CAN) =====================

    CanFilter_t f = {};
    if (id_type == sg::CANFrameIDType::STANDARD)
    {
        if (can_id > MAX_CAN_ID)
            return HAL_ERROR;
        if (end_inc > MAX_CAN_ID)
            end_inc = MAX_CAN_ID;

        f.IdType = FDCAN_STANDARD_ID;
        f.FilterType = FDCAN_FILTER_RANGE;  // inclusive [ID1..ID2]
        f.FilterID1 = base;
        f.FilterID2 = end_inc;
    }
    else if (id_type == sg::CANFrameIDType::EXTENDED)
    {
        if (can_id > 0x1FFFFFFFu)
            return HAL_ERROR;
        if (end_inc > 0x1FFFFFFFu)
            end_inc = 0x1FFFFFFFu;

        f.IdType = FDCAN_EXTENDED_ID;
        f.FilterType = FDCAN_FILTER_RANGE;  // inclusive [ID1..ID2]
        f.FilterID1 = base;
        f.FilterID2 = end_inc;
    }
    else
    {
        return HAL_ERROR;
    }

    f.FilterIndex = filters_.size();  // ensure unique index
    f.FilterConfig = (priority == sg::CANFramePriority::HIGH) ? FDCAN_FILTER_TO_RXFIFO0
                                                              : FDCAN_FILTER_TO_RXFIFO1;

    filters_.push_back(f);
    return HAL_OK;

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif
}

bool CANDevice::addCallbackId(uint32_t can_id,
                              sg::CANFrameIDType id_type,
                              CanCallback cb,
                              void* ctx)
{
    if (idCallbacks_.size() >= NUM_CAN_CALLBACKS)
        return false;
    IdEntry entry{
        .id = can_id,
        .cb = cb,
    };
    idCallbacks_.push_back(entry);
    return true;
    // TODO: Add id_type filtering & context
}

bool CANDevice::addCallbackRange(uint32_t start_id,
                                 uint32_t range,
                                 sg::CANFrameIDType id_type,
                                 CanCallback cb,
                                 void* ctx)
{
    if (rangeCallbacks_.size() >= NUM_CAN_CALLBACKS)
        return false;
    RangeEntry entry{
        .start = start_id,
        .end = start_id + range,
        .cb = cb,
    };
    rangeCallbacks_.push_back(entry);
    return true;
    // TODO: Add id_type filtering & context
}

void CANDevice::addCallbackAll(CanCallback cb)
{
    allCallback_ = cb;
}

/*!
 * @brief Finds callback connected to a single id
 *
 * @details Finds registered callback for single CAN Id
 *
 * @param id    The CAN identifier to match against (11-bit or 29-bit depending on @p id_type).
 * @return const CanCallback* if found, nullptr if no connected callback
 */
const CanCallback* CANDevice::find_by_id(uint32_t id)
{
    for (const auto& idEntry : idCallbacks_)
    {
        if (id == idEntry.id)
        {
            return &idEntry.cb;
        }
    }
    return nullptr;
}

/*!
 * @brief Finds callback connected to id within range filter
 *
 * @details Finds registered callback for id within range of CAN Identifiers
 *
 * @param id    The CAN identifier to match against (11-bit or 29-bit depending on @p id_type).
 * @return const CanCallback* if found, nullptr if no connected callback
 */
const CanCallback* CANDevice::find_by_range(uint32_t id)
{
    for (const auto& rangeEntry : rangeCallbacks_)
    {
        if (id <= rangeEntry.start && id <= rangeEntry.end)
        {
            return &rangeEntry.cb;
        }
    }
    return nullptr;
}

// ISR entry (called from HAL callback)
HAL_StatusTypeDef CANDevice::RxCallback(CanHandle_t* hcan)
{
    CANDevice* self = findByHandle(hcan);
    if (!self)
        return HAL_ERROR;

    // Read ALL messages from FIFO in the ISR
    while (CAN_RxFifoLevel(hcan) > 0)
    {
        CANFrame msg;  // Simple struct, no mutex

        CAN_ReadOne(hcan, msg);

        // Queue the simple struct (safe to copy)
        osMessageQueuePut(self->rx_queue_, &msg, 0, 0);
    }

    // Now signal the task that messages are available
    osThreadFlagsSet(self->rx_task_handle, 1u << 0);

    return HAL_OK;
}

// ====== TX AND RX FUNCTIONS ======

// Turn static task function into Device specific Call
void CANDevice::HandleRxTrampoline(void* arg)
{
    static_cast<CANDevice*>(arg)->HandleRx();
}

[[noreturn]] void CANDevice::HandleRx()
{
    CANFrame msg{};

    for (;;)
    {
        // Wait for message in queue (blocking)
        if (osMessageQueueGet(rx_queue_, &msg, nullptr, osWaitForever) == osOK)
        {
            // Process the message
            const CanCallback* cb = find_by_id(msg.can_id);
            if (cb)
            {
                (*cb)(msg, this);
                continue;
            }

            cb = find_by_range(msg.can_id);
            if (cb)
            {
                (*cb)(msg, this);
                continue;
            }

            if (allCallback_)
            {
                allCallback_(msg, this);
            }
        }
    }
}

void CANDevice::HandleTxTrampoline(void* arg)
{
    static_cast<CANDevice*>(arg)->HandleTx();
}

[[noreturn]] void CANDevice::HandleTx()
{
    CANFrame tx_msg;
    for (;;)
    {
        osMessageQueueGet(tx_queue_, &tx_msg, nullptr, osWaitForever);

        // Spinlock until a tx mailbox is empty
#if defined(HAL_FDCAN_MODULE_ENABLED)
        while (!HAL_FDCAN_GetTxFifoFreeLevel(hcan_))
            ;

        FDCAN_TxHeaderTypeDef txHeader = {
            .Identifier = tx_msg.can_id,
            .IdType = (tx_msg.id_type == sg::CANFrameIDType::STANDARD) ? FDCAN_STANDARD_ID
                                                                       : FDCAN_EXTENDED_ID,
            .TxFrameType = (tx_msg.rtr_mode == sg::CANFrameRTRMode::REMOTE) ? FDCAN_REMOTE_FRAME
                                                                            : FDCAN_DATA_FRAME,
            .DataLength = CAN_BytesToDlc(tx_msg.len),
            .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
            .BitRateSwitch = FDCAN_BRS_ON,
            .FDFormat = FDCAN_FD_CAN,
            .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
            .MessageMarker = 0};

        // Request HAL message send
        HAL_FDCAN_AddMessageToTxFifoQ(hcan_, &txHeader, tx_msg.data);
#else
        while (!HAL_CAN_GetTxMailboxesFreeLevel(hcan_))
            ;

        CAN_TxHeaderTypeDef txHeader = {
            .StdId = tx_msg.can_id,
            .ExtId = tx_msg.can_id,
            .IDE = (tx_msg.id_type == sg::CANFrameIDType::STANDARD) ? CAN_ID_STD : CAN_ID_EXT,
            .RTR = (tx_msg.rtr_mode == sg::CANFrameRTRMode::REMOTE) ? CAN_RTR_REMOTE : CAN_RTR_DATA,
            .DLC = CAN_BytesToDlc(tx_msg.len),
            .TransmitGlobalTime = DISABLE,
        };

        uint32_t txMailbox;

        // Request HAL message send
        HAL_CAN_AddTxMessage(hcan_, &txHeader, tx_msg.data, &txMailbox);
#endif
    }
}

HAL_StatusTypeDef CANDevice::Send(const CANFrame& msg)
{
    if (osMessageQueuePut(tx_queue_, &msg, 0, TX_TIMEOUT) != osOK)
    {
        //HandleTxTimeout();
        return HAL_ERROR;
    }

    return HAL_OK;
}

// ====== HANDLE FUNCTIONS ======
CANDevice* CANDevice::findByHandle(CanHandle_t* h)
{
    for (auto& e : s_registry_)
        if (e.h == h)
            return e.dev;
    return nullptr;
}

bool CANDevice::registerHandle(CanHandle_t* h, CANDevice* d)
{
    for (auto& e : s_registry_)
    {
        if (e.h == h)
        {
            // already registered -> fail
            return false;
        }
    }

    for (auto& e : s_registry_)
    {
        // first free slot
        if (e.h == nullptr)
        {
            e.h = h;
            e.dev = d;
            return true;
        }
    }
    // 3) No space
    return false;
}

void CANDevice::unregisterHandle(CanHandle_t* h)
{
    for (auto& e : s_registry_)
        if (e.h == h)
        {
            e.h = nullptr;
            e.dev = nullptr;
            return;
        }
}

};  // namespace sg

#if defined(HAL_CAN_MODULE_ENABLED)
/**
 * @brief  CAN Rx interrupt callback.
 * @param  hcan Pointer to CAN_HandleTypeDef object
 */
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    CANDriver::CANDevice::RxCallback(hcan);
}
#elif defined(HAL_FDCAN_MODULE_ENABLED)
/**
 * @brief  CAN Rx interrupt callback
 * @param  hcan Pointer to CAN_HandleTypeDef object
 * @param  RxFifo0ITs Reason for callback
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hcan, uint32_t RxFifo0ITs)
{
    sg::CANDevice::RxCallback(hcan);
}
#endif
