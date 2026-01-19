/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SUPP_BATT_V_Pin GPIO_PIN_2
#define SUPP_BATT_V_GPIO_Port GPIOC
#define DAC_SPI_SS_Pin GPIO_PIN_3
#define DAC_SPI_SS_GPIO_Port GPIOC
#define MAIN_ARRAY_CTRL_Pin GPIO_PIN_0
#define MAIN_ARRAY_CTRL_GPIO_Port GPIOA
#define PRE_ARRAY_CTRL_Pin GPIO_PIN_2
#define PRE_ARRAY_CTRL_GPIO_Port GPIOA
#define THROTTLE_SRC_SEL_Pin GPIO_PIN_3
#define THROTTLE_SRC_SEL_GPIO_Port GPIOA
#define MC_FWD_REV_CTRL_Pin GPIO_PIN_4
#define MC_FWD_REV_CTRL_GPIO_Port GPIOA
#define MC_MAIN_CTRL_Pin GPIO_PIN_6
#define MC_MAIN_CTRL_GPIO_Port GPIOA
#define THROTTLE_V_Pin GPIO_PIN_4
#define THROTTLE_V_GPIO_Port GPIOC
#define REGEN_V_Pin GPIO_PIN_5
#define REGEN_V_GPIO_Port GPIOC
#define WATCHDOG_INPUT_Pin GPIO_PIN_0
#define WATCHDOG_INPUT_GPIO_Port GPIOB
#define WATCHDOG_SET0_Pin GPIO_PIN_1
#define WATCHDOG_SET0_GPIO_Port GPIOB
#define WATCHDOG_SET1_Pin GPIO_PIN_2
#define WATCHDOG_SET1_GPIO_Port GPIOB
#define WATCHDOG_ENABLE_Pin GPIO_PIN_10
#define WATCHDOG_ENABLE_GPIO_Port GPIOB
#define OK_LED_Pin GPIO_PIN_13
#define OK_LED_GPIO_Port GPIOB
#define THROTTLE_MODE_INPUT_Pin GPIO_PIN_15
#define THROTTLE_MODE_INPUT_GPIO_Port GPIOB
#define EEPROM_SPI_SS_Pin GPIO_PIN_15
#define EEPROM_SPI_SS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
