#include "CanDriver.hpp"

#define TRY(x)                \
    do                        \
    {                         \
        if ((x) != HAL_OK)    \
            return HAL_ERROR; \
    } while (0)

CANDevice::CANDevice(CanHandle_t* hcan) : hcan(hcan) {}

HAL_StatusTypeDef CANDevice::StartCANDevice()
{
    if (!registerHandle(hcan, this))
    {
        return HAL_ERROR;
    }

    rx_task_handle = osThreadNew(&CANDevice::HandleRxTrampoline, this, &rx_task_attributes_);
    if (!rx_task_handle)
        return HAL_ERROR;

#if defined(HAL_CAN_MODULE_ENABLED)
    // ===================== bxCAN =====================
    if (filters_.empty())
    {
        CanFilter_t filter = {};

        filter.FilterActivation = ENABLE;
        filter.FilterBank = 0;
        filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
        filter.FilterMode = CAN_FILTERMODE_IDMASK;
        filter.FilterScale = CAN_FILTERSCALE_32BIT;

        // Accept ALL messages (for debug/testing)
        filter.FilterIdHigh = 0x0000;
        filter.FilterIdLow = 0x0000;
        filter.FilterMaskIdHigh = 0x0000;
        filter.FilterMaskIdLow = 0x0000;

        TRY(HAL_CAN_ConfigFilter(hcan, &filter));
    }
    else
    {
        for (const auto& filter : filters_)
        {
            TRY(HAL_CAN_ConfigFilter(hcan, &filter));
        }
    }

    TRY(HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING));

    TRY(HAL_CAN_Start(hcan));

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
        filter.FilterID2 = 0x7FF;

        TRY(HAL_FDCAN_ConfigFilter(hcan, &filter));
    }
    else
    {
        for (const auto& filter : filters_)
        {
            TRY(HAL_FDCAN_ConfigFilter(hcan, &filter));
        }
    }

    TRY(HAL_FDCAN_ConfigGlobalFilter(
        hcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE));

    TRY(HAL_FDCAN_ActivateNotification(hcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0));

    TRY(HAL_FDCAN_Start(hcan));

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif

    return HAL_OK;
}

HAL_StatusTypeDef CANDevice::AddFilterId(uint32_t can_id,
                                         uint32_t id_type,
                                         uint32_t rtr_mode,
                                         uint32_t priority)
{
    if (filters_.size() >= NUM_FILTER_BANKS)
        return HAL_ERROR;

#if defined(HAL_CAN_MODULE_ENABLED)
    // ------------------------ bxCAN (classic) ------------------------
    // Validate & map RTR for bxCAN
    const uint32_t hal_rtr = (rtr_mode == SG_CAN_RTR_REMOTE) ? CAN_RTR_REMOTE : CAN_RTR_DATA;

    if (id_type == SG_CAN_ID_STD)
    {
        if (can_id > 0x7FFu)
            return HAL_ERROR;

        // Exact match: use IDMASK with all 11 ID bits compared (mask 0x7FF)
        const uint32_t filter_id = ((can_id & 0x7FFu) << 21) | CAN_ID_STD | hal_rtr;
        const uint32_t filter_mask = ((0x7FFu) << 21) | 0b110;  // also match IDE & RTR

        CAN_FilterTypeDef f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == SG_CAN_PRIO_HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
        f.FilterBank = filters_.size();
        f.FilterMode = CAN_FILTERMODE_IDMASK;
        f.FilterScale = CAN_FILTERSCALE_32BIT;
        f.FilterActivation = ENABLE;

        filters_.push_back(f);
        return HAL_OK;
    }
    else if (id_type == SG_CAN_ID_EXT)
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
            (priority == SG_CAN_PRIO_HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
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
    return HAL_OK;

#elif defined(HAL_FDCAN_MODULE_ENABLED)
    // ------------------------ FDCAN (M_CAN) ------------------------

    FDCAN_FilterTypeDef f = {};
    if (id_type == SG_CAN_ID_STD)
    {
        if (can_id > 0x7FFu)
            return HAL_ERROR;

        // Easiest exact match on FDCAN: RANGE with start==end
        f.IdType = FDCAN_STANDARD_ID;
        f.FilterType = FDCAN_FILTER_RANGE;  // inclusive
        f.FilterID1 = can_id;
        f.FilterID2 = can_id;
    }
    else if (id_type == SG_CAN_ID_EXT)
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
    f.FilterConfig =
        (priority == SG_CAN_PRIORITY_HIGH) ? FDCAN_FILTER_TO_RXFIFO0 : FDCAN_FILTER_TO_RXFIFO1;

    filters_.push_back(f);
    return HAL_OK;

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif
}

HAL_StatusTypeDef CANDevice::AddFilterRange(uint32_t can_id,
                                            uint32_t range,
                                            uint32_t id_type,
                                            uint32_t rtr_mode,
                                            uint32_t priority)
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

    if (id_type == SG_CAN_ID_STD)
    {
        if (can_id > 0x7FFu)
            return HAL_ERROR;
        if (end_inc > 0x7FFu)
            end_inc = 0x7FFu;

        // bxCAN 32-bit IDMASK packing (STD: ID at bits 31..21)
        uint32_t filter_id = ((base & 0x7FFu) << 21) | id_type | rtr_mode;
        uint32_t filter_mask = ((id_mask & 0x7FFu) << 21) | 0b110;  // match IDE & RTR

        CanFilter_t f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == SG_CAN_PRIORITY_HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
        f.FilterBank = filters_.size();  // ensure unique index upstream or here
        f.FilterMode = CAN_FILTERMODE_IDMASK;
        f.FilterScale = CAN_FILTERSCALE_32BIT;
        f.FilterActivation = ENABLE;

        filters_.push_back(f);
        return HAL_OK;
    }
    else if (id_type == SG_CAN_ID_EXT)
    {
        if (can_id > 0x1FFFFFFFu)
            return HAL_ERROR;
        if (end_inc > 0x1FFFFFFFu)
            end_inc = 0x1FFFFFFFu;

        // bxCAN 32-bit IDMASK packing (EXT: ID at bits 31..3)
        uint32_t filter_id = ((base & 0x1FFFFFFFu) << 3) | id_type | rtr_mode;
        uint32_t filter_mask = ((id_mask & 0x1FFFFFFFu) << 3) | 0b110;  // match IDE & RTR

        CanFilter_t f = {};
        f.FilterIdHigh = (filter_id >> 16) & 0xFFFFu;
        f.FilterIdLow = filter_id & 0xFFFFu;
        f.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFu;
        f.FilterMaskIdLow = filter_mask & 0xFFFFu;
        f.FilterFIFOAssignment =
            (priority == SG_CAN_PRIORITY_HIGH) ? CAN_FILTER_FIFO0 : CAN_FILTER_FIFO1;
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
    return HAL_OK;

#elif defined(HAL_FDCAN_MODULE_ENABLED)
    // ===================== FDCAN (M_CAN) =====================

    CanFilter_t f = {};
    if (id_type == SG_CAN_ID_STD)
    {
        if (can_id > 0x7FFu)
            return HAL_ERROR;
        if (end_inc > 0x7FFu)
            end_inc = 0x7FFu;

        f.IdType = FDCAN_STANDARD_ID;
        f.FilterType = FDCAN_FILTER_RANGE;  // inclusive [ID1..ID2]
        f.FilterID1 = base;
        f.FilterID2 = end_inc;
    }
    else if (id_type == SG_CAN_ID_EXT)
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
    f.FilterConfig =
        (priority == SG_CAN_PRIORITY_HIGH) ? FDCAN_FILTER_TO_RXFIFO0 : FDCAN_FILTER_TO_RXFIFO1;

    filters_.push_back(f);
    return HAL_OK;

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif
}

// ---- ISR entry (called from HAL callback) ----
HAL_StatusTypeDef CANDevice::RxCallback(CanHandle_t* hcan)
{
    CANDevice* self = findByHandle(hcan);
    if (!self)
        return HAL_ERROR;

    osThreadFlagsSet(self->rx_task_handle, 1u << 0);
    return HAL_OK;
}

// Turn static task function into Device specific Call
void CANDevice::HandleRxTrampoline(void* arg)
{
    static_cast<CANDevice*>(arg)->HandleRxLoop();
}

void CANDevice::HandleRxLoop()
{
    for (;;)
    {
        osThreadFlagsWait(1u << 0, osFlagsWaitAny, osWaitForever);

#if defined(HAL_FDCAN_MODULE_ENABLED)
        while (HAL_FDCAN_GetRxFifoFillLevel(hcan, FDCAN_RX_FIFO0) > 0)
        {
            CANMsg msg{};
            msg.hcan = hcan;
            FDCAN_RxHeaderTypeDef hdr{};
            if (HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &hdr, msg.data) != HAL_OK)
                break;
            msg.id = (hdr.IdType == FDCAN_EXTENDED_ID) ? hdr.Identifier : (hdr.Identifier & 0x7FF);
            msg.id_type = (hdr.IdType == FDCAN_EXTENDED_ID) ? SG_CAN_ID_EXT : SG_CAN_ID_STD;
            msg.rtr = (hdr.RxFrameType == FDCAN_REMOTE_FRAME);
            msg.dlc = hdr.DataLength;
            msg.timestamp = 0;
            // TODO: fill with timestamping
            // TODO: lookup and call handlers with (msg, ctx)
        }
#elif defined(HAL_CAN_MODULE_ENABLED)
        while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0)
        {
            CANMsg msg{};
            msg.hcan = hcan;
            CAN_RxHeaderTypeDef hdr{};
            if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hdr, msg.data) != HAL_OK)
                break;
            msg.id = (hdr.IDE == CAN_ID_EXT) ? hdr.ExtId : hdr.StdId;
            msg.id_type = (hdr.IDE == CAN_ID_EXT) ? SG_CAN_ID_EXT : SG_CAN_ID_STD;
            msg.rtr = (hdr.RTR == CAN_RTR_REMOTE);
            msg.dlc = hdr.DLC;
            msg.timestamp = 0;
            // TODO: fill with timestamping
            // TODO: lookup and call handlers with (msg, ctx)
        }
#endif
    }
}

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

#if defined(HAL_CAN_MODULE_ENABLED)
/**
 * @brief  CAN Rx interrupt callback.
 * @param  hcan Pointer to CAN_HandleTypeDef object
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    CANDevice::RxCallback(hcan);
}
#elif defined(HAL_FDCAN_MODULE_ENABLED)
/**
 * @brief  CAN Rx interrupt callback
 * @param  hcan Pointer to CAN_HandleTypeDef object
 * @param  RxFifo0ITs Reason for callback
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hcan, uint32_t RxFifo0ITs)
{
    HAL_FDCAN_DeactivateNotification(hcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
    CANDevice::RxCallback(hcan);
}
#endif