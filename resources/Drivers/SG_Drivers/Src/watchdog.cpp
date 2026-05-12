#include "watchdog.hpp"

// Look at hpp file for use case

namespace sg
{
bool Watchdog::threadReady[16] = {false};
uint8_t Watchdog::totalThreads = 0;

/**
    * @brief STM32U575xx IWDG Initialization Function
    * @param None
    */
HAL_StatusTypeDef Watchdog::IWDG_Init(void)
{
    taskENTER_CRITICAL();
    // give each thread an id
    if (totalThreads < 16)
    {
        id = totalThreads++;
    }
    taskEXIT_CRITICAL();
    return HAL_OK;
}

/**
    * @brief STM32U575xx IWDG Kick Function
    * @param None
    * @retval None
    */
void Watchdog::Kick(void)
{
    taskENTER_CRITICAL();

    threadReady[id] = true;

    bool all_ready = true;
    for (uint8_t i = 0; i < totalThreads; i++)
    {
        if (threadReady[i] == false)
        {
            all_ready = false;
            break;
        }
    }

    if (all_ready)
    {
        HAL_IWDG_Refresh(&hiwdg);

        for (uint8_t i = 0; i < totalThreads; i++)
        {
            threadReady[i] = false;
        }
    }

    taskEXIT_CRITICAL();
}
}  // namespace sg
