#include "watchdog.hpp"

//#include "stm32u5xx.h"   // CMSIS device header

namespace sg
{

    IWDG_HandleTypeDef Watchdog::hiwdg = {0};
    bool Watchdog::threadReady[16] = {false};
    uint8_t Watchdog::totalThreads = 0;

    /**
    * @brief  This function is executed in case of error occurrence.
    * @retval None
    */
    void Watchdog::Error_Handler(void)
    {
        /* USER CODE BEGIN Error_Handler_Debug */
        __disable_irq();
        while (1)
        {
            // User can add his own implementation to report the HAL error return state
        }
        /* USER CODE END Error_Handler_Debug */
    }

    /**
    * @brief STM32U575xx IWDG Initialization Function
    * @param None
    */
    HAL_StatusTypeDef Watchdog::MX_IWDG_Init(void)
    {
        // This watchdog is meant for the STM32U575xx MCU

        // give each thread an id
        if (totalThreads < 16) {
            id = totalThreads++;
        }


        // only run watchdog init once
        if (id == 0) {
            hiwdg.Instance = IWDG;
            hiwdg.Init.Prescaler = (IWDG_PR_PR_1 | IWDG_PR_PR_2); // TODO: change this to IWDG_PR_PR_3
            hiwdg.Init.Window = 4095;
            hiwdg.Init.Reload = 4095;
            hiwdg.Init.EWI = 0;
            HAL_IWDG_Init(&hiwdg);
        }

        /*if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
        {
            __HAL_RCC_LSI_ENABLE();
            HAL_Delay(1);
        }*/

        if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
        {
            Error_Handler();
        }

        return HAL_OK;
    }

    /**
    * @brief STM32U575xx IWDG Kick Function
    * @param None
    * @retval None
    */
    void Watchdog::Kick(void)
    {
        // set id connect to specific thread to true/ran
        threadReady[id] = true;

        // check if all threads have been run, if so, set all_ready flag to true
        bool all_ready = true;
        for (uint8_t i = 0; i < totalThreads; i++) {
            if (threadReady[i] == false) {
                all_ready = false;
                break;
            }
        }

        // When all threads are ready (have been run), kick the watchdog
        if (all_ready) {
            HAL_IWDG_Refresh(&hiwdg);
            for (uint8_t i = 0; i < totalThreads; i++) {
                threadReady[i] = false;
            }
        }
    }


} // namespace sg