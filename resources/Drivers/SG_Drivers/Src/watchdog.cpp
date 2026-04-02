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

    void Watchdog::Kick(void)
    {
        // Reload counter
        // hiwdg.Instance.Reload = 4095;
        HAL_IWDG_Refresh(&hiwdg);
    }

    /*
     isreset
     // more info here https://github.com/STMicroelectronics/STM32CubeU5/blob/main/Projects/NUCLEO-U575ZI-Q/Examples/IWDG/IWDG_Reset/Src/main.c
    -1- Check if the system has resumed from IWDG reset // (pulled from stm repo)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != 0x00u)
    {
        /* IWDGRST flag set: Turn LED1 on and set IwdgStatus
        IwdgStatus = 1;

        /* Insert 4s delay
        HAL_Delay(4000);

    }
     */


} // namespace sg