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
#define CAN_LED_Pin GPIO_PIN_0
#define CAN_LED_GPIO_Port GPIOC
#define OK_LED_Pin GPIO_PIN_1
#define OK_LED_GPIO_Port GPIOC
#define BRAKE_LIGHT_CTRL_Pin GPIO_PIN_6
#define BRAKE_LIGHT_CTRL_GPIO_Port GPIOA
#define REAR_LEFT_LIGHT_CTRL_Pin GPIO_PIN_7
#define REAR_LEFT_LIGHT_CTRL_GPIO_Port GPIOA
#define STROBE_LIGHT_CTRL_Pin GPIO_PIN_4
#define STROBE_LIGHT_CTRL_GPIO_Port GPIOC
#define REAR_RIGHT_LIGHT_CTRL_Pin GPIO_PIN_5
#define REAR_RIGHT_LIGHT_CTRL_GPIO_Port GPIOC
#define KILL_SW_INPUT_Pin GPIO_PIN_12
#define KILL_SW_INPUT_GPIO_Port GPIOB
#define KILL_SW_INPUT_EXTI_IRQn EXTI12_IRQn
#define RADIO_CTS_Pin GPIO_PIN_14
#define RADIO_CTS_GPIO_Port GPIOB
#define SENSOR_MCU_UART_TX_Pin GPIO_PIN_9
#define SENSOR_MCU_UART_TX_GPIO_Port GPIOA
#define SENSOR_MCU_UART_RX_Pin GPIO_PIN_10
#define SENSOR_MCU_UART_RX_GPIO_Port GPIOA
#define EEPROM_CS_Pin GPIO_PIN_15
#define EEPROM_CS_GPIO_Port GPIOA
#define RADIO_LED_Pin GPIO_PIN_4
#define RADIO_LED_GPIO_Port GPIOB
#define GPS_LED_Pin GPIO_PIN_5
#define GPS_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
