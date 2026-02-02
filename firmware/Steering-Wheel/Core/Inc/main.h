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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EEPROM_CS_Pin GPIO_PIN_13
#define EEPROM_CS_GPIO_Port GPIOC
#define BUTTON8_LED_Pin GPIO_PIN_0
#define BUTTON8_LED_GPIO_Port GPIOC
#define BUTTON2_LED_Pin GPIO_PIN_1
#define BUTTON2_LED_GPIO_Port GPIOC
#define REGEN_INCREASE_Pin GPIO_PIN_2
#define REGEN_INCREASE_GPIO_Port GPIOC
#define SCREEN_D0_Pin GPIO_PIN_0
#define SCREEN_D0_GPIO_Port GPIOA
#define SCREEN_D1_Pin GPIO_PIN_1
#define SCREEN_D1_GPIO_Port GPIOA
#define SCREEN_D2_Pin GPIO_PIN_2
#define SCREEN_D2_GPIO_Port GPIOA
#define SCREEN_D3_Pin GPIO_PIN_3
#define SCREEN_D3_GPIO_Port GPIOA
#define SCREEN_D4_Pin GPIO_PIN_4
#define SCREEN_D4_GPIO_Port GPIOA
#define SCREEN_D5_Pin GPIO_PIN_5
#define SCREEN_D5_GPIO_Port GPIOA
#define SCREEN_D6_Pin GPIO_PIN_6
#define SCREEN_D6_GPIO_Port GPIOA
#define SCREEN_D7_Pin GPIO_PIN_7
#define SCREEN_D7_GPIO_Port GPIOA
#define SCREEN_WRITE_READY_Pin GPIO_PIN_4
#define SCREEN_WRITE_READY_GPIO_Port GPIOC
#define SCREEN_CMD_DATA_SEL_Pin GPIO_PIN_5
#define SCREEN_CMD_DATA_SEL_GPIO_Port GPIOC
#define BUTTON1_Pin GPIO_PIN_0
#define BUTTON1_GPIO_Port GPIOB
#define BUTTON1_EXTI_IRQn EXTI0_IRQn
#define BUTTON1_LED_Pin GPIO_PIN_1
#define BUTTON1_LED_GPIO_Port GPIOB
#define BUTTON2_Pin GPIO_PIN_2
#define BUTTON2_GPIO_Port GPIOB
#define BUTTON2_EXTI_IRQn EXTI2_IRQn
#define BUTTON6_Pin GPIO_PIN_10
#define BUTTON6_GPIO_Port GPIOB
#define BUTTON6_EXTI_IRQn EXTI10_IRQn
#define BUTTON6_LED_Pin GPIO_PIN_12
#define BUTTON6_LED_GPIO_Port GPIOB
#define BUTTON7_Pin GPIO_PIN_13
#define BUTTON7_GPIO_Port GPIOB
#define BUTTON7_EXTI_IRQn EXTI13_IRQn
#define BUTTON7_LED_Pin GPIO_PIN_14
#define BUTTON7_LED_GPIO_Port GPIOB
#define BUTTON8_Pin GPIO_PIN_15
#define BUTTON8_GPIO_Port GPIOB
#define BUTTON8_EXTI_IRQn EXTI15_IRQn
#define SCREEN_PARALLEL_CS_Pin GPIO_PIN_6
#define SCREEN_PARALLEL_CS_GPIO_Port GPIOC
#define OK_LED_Pin GPIO_PIN_7
#define OK_LED_GPIO_Port GPIOC
#define ERROR_LED_Pin GPIO_PIN_8
#define ERROR_LED_GPIO_Port GPIOC
#define REGEN_DECREASE_Pin GPIO_PIN_9
#define REGEN_DECREASE_GPIO_Port GPIOC
#define SCREEN_BRIGHTNESS_PWM_Pin GPIO_PIN_8
#define SCREEN_BRIGHTNESS_PWM_GPIO_Port GPIOA
#define SCREEN_nRST_Pin GPIO_PIN_9
#define SCREEN_nRST_GPIO_Port GPIOA
#define SCREEN_DC_SEL_Pin GPIO_PIN_15
#define SCREEN_DC_SEL_GPIO_Port GPIOA
#define BUTTON4_Pin GPIO_PIN_4
#define BUTTON4_GPIO_Port GPIOB
#define BUTTON4_EXTI_IRQn EXTI4_IRQn
#define BUTTON4_LED_Pin GPIO_PIN_5
#define BUTTON4_LED_GPIO_Port GPIOB
#define BUTTON3_Pin GPIO_PIN_6
#define BUTTON3_GPIO_Port GPIOB
#define BUTTON3_EXTI_IRQn EXTI6_IRQn
#define BUTTON3_LED_Pin GPIO_PIN_7
#define BUTTON3_LED_GPIO_Port GPIOB
#define BUTTON5_Pin GPIO_PIN_8
#define BUTTON5_GPIO_Port GPIOB
#define BUTTON5_EXTI_IRQn EXTI8_IRQn
#define BUTTON5_LED_Pin GPIO_PIN_9
#define BUTTON5_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
