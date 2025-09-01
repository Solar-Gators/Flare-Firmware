#include "CanDriver.hpp"

HAL_StatusTypeDef CANDevice::StartCANDevice(CanHandle_t *hcan)
{
    this->hcan = hcan;

    CanFilter_t filter = {0};

#ifdef HAL_CAN_MODULE_ENABLE
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

    if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK)
    {
        Error_Handler();  // Debug message -> Write to eeprom?
    }

    if (HAL_CAN_Start(&hcan) != HAL_OK)
    {
        Error_Handler();  // Debug message -> Write to eeprom?
    }
#endif

    return HAL_OK;
}