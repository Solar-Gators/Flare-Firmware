#include "CanDriver.hpp"

HAL_StatusTypeDef CANDevice::StartCANDevice(CanHandle_t *hcan)
{
    this->hcan = hcan;

    CanFilter_t filter;

#if defined(HAL_CAN_MODULE_ENABLE)
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

    HAL_StatusTypeDef res = HAL_CAN_ConfigFilter(&hcan, &filter);
    if (HAL_OK != res)
        return res;

    res = HAL_FDCAN_ConfigGlobalFilter(
        hcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
    if (HAL_OK != res)
        return res;

    res = HAL_CAN_Start(&hcan);
    if (HAL_OK != res)
        return res;

#elif defined(HAL_FDCAN_MODULE_ENABLED)

    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = 0;
    filter.FilterType = FDCAN_FILTER_RANGE;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = 0x0000;
    filter.FilterID2 = 0x0000;

    HAL_StatusTypeDef res = HAL_FDCAN_ConfigFilter(hcan, &filter);
    if (HAL_OK != res)
        return res;

    res = HAL_FDCAN_ConfigGlobalFilter(
        hcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
    if (HAL_OK != res)
        return res;

    res = HAL_FDCAN_Start(hcan);
    if (HAL_OK != res)
        return res;

#else
#error "Enable either HAL_CAN_MODULE_ENABLED or HAL_FDCAN_MODULE_ENABLED"
#endif

    return HAL_OK;
}