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
typedef StaticTask_t osStaticThreadDef_t;
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
/* Definitions for hearbeatTask */
osThreadId_t hearbeatTaskHandle;
uint32_t heartbeatTaskBuffer[ 64 ];
osStaticThreadDef_t heartbeatTaskCB;
const osThreadAttr_t hearbeatTask_attributes = {
  .name = "hearbeatTask",
  .stack_mem = &heartbeatTaskBuffer[0],
  .stack_size = sizeof(heartbeatTaskBuffer),
  .cb_mem = &heartbeatTaskCB,
  .cb_size = sizeof(heartbeatTaskCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for regenThrottleTask */
osThreadId_t regenThrottleTaskHandle;
uint32_t regenThrottleTaskBuffer[ 128 ];
osStaticThreadDef_t regenThrottleTaskCB;
const osThreadAttr_t regenThrottleTask_attributes = {
  .name = "regenThrottleTask",
  .stack_mem = &regenThrottleTaskBuffer[0],
  .stack_size = sizeof(regenThrottleTaskBuffer),
  .cb_mem = &regenThrottleTaskCB,
  .cb_size = sizeof(regenThrottleTaskCB),
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for outputsTask */
osThreadId_t outputsTaskHandle;
uint32_t outputsTaskBuffer[ 128 ];
osStaticThreadDef_t outputsTaskCB;
const osThreadAttr_t outputsTask_attributes = {
  .name = "outputsTask",
  .stack_mem = &outputsTaskBuffer[0],
  .stack_size = sizeof(outputsTaskBuffer),
  .cb_mem = &outputsTaskCB,
  .cb_size = sizeof(outputsTaskCB),
  .priority = (osPriority_t) osPriorityAboveNormal,
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
  /* creation of hearbeatTask */
  hearbeatTaskHandle = osThreadNew(startHeartbeatTask, NULL, &hearbeatTask_attributes);

  /* creation of regenThrottleTask */
  regenThrottleTaskHandle = osThreadNew(startRegenThrottleTask, NULL, &regenThrottleTask_attributes);

  /* creation of outputsTask */
  outputsTaskHandle = osThreadNew(startOutputsTask, NULL, &outputsTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_startHeartbeatTask */
/**
* @brief Function implementing the hearbeatTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startHeartbeatTask */
void startHeartbeatTask(void *argument)
{
  /* USER CODE BEGIN hearbeatTask */
    startHeartbeatTask_user(argument);
  /* USER CODE END hearbeatTask */
}

/* USER CODE BEGIN Header_startRegenThrottleTask */
/**
* @brief Function implementing the regenThrottleTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startRegenThrottleTask */
void startRegenThrottleTask(void *argument)
{
  /* USER CODE BEGIN regenThrottleTask */
    startRegenThrottleTask_user(argument);
  /* USER CODE END regenThrottleTask */
}

/* USER CODE BEGIN Header_startOutputsTask */
/**
* @brief Function implementing the outputsTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startOutputsTask */
void startOutputsTask(void *argument)
{
  /* USER CODE BEGIN outputsTask */
    startOutputsTask_user(argument);
  /* USER CODE END outputsTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

