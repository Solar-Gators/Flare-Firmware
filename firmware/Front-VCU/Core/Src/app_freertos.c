/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user_threads.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Heartbeat */
osThreadId_t HeartbeatHandle;
const osThreadAttr_t Heartbeat_attributes = {
  .name = "Heartbeat",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for ThrottleRead */
osThreadId_t ThrottleReadHandle;
const osThreadAttr_t ThrottleRead_attributes = {
  .name = "ThrottleRead",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of Heartbeat */
  HeartbeatHandle = osThreadNew(StartHeartbeat, NULL, &Heartbeat_attributes);

  /* creation of ThrottleRead */
  ThrottleReadHandle = osThreadNew(StartThrottleRead, NULL, &ThrottleRead_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartHeartbeat */
/**
* @brief Function implementing the Heartbeat thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHeartbeat */
void StartHeartbeat(void *argument)
{
  /* USER CODE BEGIN Heartbeat */
  /* Infinite loop */
  StartHeartbeat_user(argument);
  /* USER CODE END Heartbeat */
}

/* USER CODE BEGIN Header_StartThrottleRead */
/**
* @brief Function implementing the ThrottleRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartThrottleRead */
void StartThrottleRead(void *argument)
{
  /* USER CODE BEGIN ThrottleRead */
  /* Infinite loop */
  StartThrottleRead_user(argument);
  /* USER CODE END ThrottleRead */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

