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
#define FR_LIGHT_CTRL_Pin GPIO_PIN_0
#define FR_LIGHT_CTRL_GPIO_Port GPIOC
#define R_HEADLIGHT_CTRL_Pin GPIO_PIN_1
#define R_HEADLIGHT_CTRL_GPIO_Port GPIOC
#define L_HEADLIGHT_CTRL_Pin GPIO_PIN_2
#define L_HEADLIGHT_CTRL_GPIO_Port GPIOC
#define FL_LIGHT_CTRL_Pin GPIO_PIN_3
#define FL_LIGHT_CTRL_GPIO_Port GPIOC
#define EXTRA1_CTRL_Pin GPIO_PIN_0
#define EXTRA1_CTRL_GPIO_Port GPIOA
#define EXTRA2_CTRL_Pin GPIO_PIN_1
#define EXTRA2_CTRL_GPIO_Port GPIOA
#define FAN_CTRL_Pin GPIO_PIN_2
#define FAN_CTRL_GPIO_Port GPIOA
#define HORN_CTRL_Pin GPIO_PIN_3
#define HORN_CTRL_GPIO_Port GPIOA
#define THROTTLE_Pin GPIO_PIN_6
#define THROTTLE_GPIO_Port GPIOA
#define BRAKE_Pin GPIO_PIN_7
#define BRAKE_GPIO_Port GPIOA
#define OK_LED_Pin GPIO_PIN_15
#define OK_LED_GPIO_Port GPIOB
#define STROBE_LED_Pin GPIO_PIN_6
#define STROBE_LED_GPIO_Port GPIOC
#define ERROR_LED_Pin GPIO_PIN_7
#define ERROR_LED_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
