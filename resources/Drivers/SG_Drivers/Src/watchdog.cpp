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
    * @brief IWDG Initialization Function
    * @param None
    * @retval None
    */
    HAL_StatusTypeDef Watchdog::MX_IWDG_Init(void)
    {
        hiwdg.Instance = IWDG;
        hiwdg.Init.Prescaler = (IWDG_PR_PR_1 | IWDG_PR_PR_0);
        hiwdg.Init.Window = 4095;
        hiwdg.Init.Reload = 4095;
        hiwdg.Init.EWI = 0;

        if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
        {
            __HAL_RCC_LSI_ENABLE();
            HAL_Delay(1);
            //delay
            //while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET){} // This line breaks the code (infinite loop)
        }

        if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
        {
            Error_Handler();
        }

        return HAL_OK;
    }

    void Watchdog::Kick()
    {
        // Reload counter
        // hiwdg.Instance.Reload = 4095;
        HAL_IWDG_Refresh(&hiwdg);
    }

    /*
    void Watchdog::EnableLSI()
    {
        // Enable LSI clock if not already enabled
        if ((RCC->BDCR & RCC_BDCR_LSION) == 0)
        {
            RCC->BDCR |= RCC_BDCR_LSION;

            // Wait until LSI is ready
            while ((RCC->BDCR & RCC_BDCR_LSIRDY) == 0)
            {
                // spin
            }
        }
    }*/

} // namespace sg