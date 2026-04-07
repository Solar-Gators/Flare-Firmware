#include "watchdog.hpp"

//#include "stm32u5xx.h"   // CMSIS device header

namespace sg
{

    /**
    * @brief  This function is executed in case of error occurrence.
    * @retval None
    */
    void Watchdog::Error_Handler(void)
    {
        /* USER CODE BEGIN Error_Handler_Debug */
        /* User can add his own implementation to report the HAL error return state */
        __disable_irq();
        while (1)
        {
        }
        /* USER CODE END Error_Handler_Debug */
    }

    /**
    * @brief STM32U575xx IWDG Initialization Function
    * @param None
    * @retval None
    */
    HAL_StatusTypeDef Watchdog::MX_IWDG_Init(void)
    {
        // This watchdog is meant for the STM32U575xx MCU
        hiwdg.Instance = IWDG;
        hiwdg.Init.Prescaler = (IWDG_PR_PR_1 | IWDG_PR_PR_0);
        hiwdg.Init.Window = 4095;
        hiwdg.Init.Reload = 4095;
        hiwdg.Init.EWI = 0;

        if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
        {
            __HAL_RCC_LSI_ENABLE();
            HAL_Delay(1);
        }

        if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
        {
            Error_Handler();
        }

        return HAL_OK;
    }

    void Watchdog::Kick(void)
    {
        // Reload counter
        // hiwdg.Instance.Reload = 4095;
        HAL_IWDG_Refresh(&hiwdg);
    }


} // namespace sg