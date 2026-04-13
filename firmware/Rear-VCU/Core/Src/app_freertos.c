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
uint32_t heartbeatTaskBuffer[ 256 ];
osStaticThreadDef_t heartbeatTaskCB;
const osThreadAttr_t hearbeatTask_attributes = {
  .name = "hearbeatTask",
  .stack_mem = &heartbeatTaskBuffer[0],
  .stack_size = sizeof(heartbeatTaskBuffer),
  .cb_mem = &heartbeatTaskCB,
  .cb_size = sizeof(heartbeatTaskCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sendStatusTask */
osThreadId_t sendStatusTaskHandle;
uint32_t sendStatusTaskBuffer[ 256 ];
osStaticThreadDef_t sendStatusTaskCB;
const osThreadAttr_t sendStatusTask_attributes = {
  .name = "sendStatusTask",
  .stack_mem = &sendStatusTaskBuffer[0],
  .stack_size = sizeof(sendStatusTaskBuffer),
  .cb_mem = &sendStatusTaskCB,
  .cb_size = sizeof(sendStatusTaskCB),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for outputsTask */
osThreadId_t outputsTaskHandle;
uint32_t outputsTaskBuffer[ 512 ];
osStaticThreadDef_t outputsTaskCB;
const osThreadAttr_t outputsTask_attributes = {
  .name = "outputsTask",
  .stack_mem = &outputsTaskBuffer[0],
  .stack_size = sizeof(outputsTaskBuffer),
  .cb_mem = &outputsTaskCB,
  .cb_size = sizeof(outputsTaskCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SupBatMonitoring */
osThreadId_t SupBatMonitoringHandle;
const osThreadAttr_t SupBatMonitoring_attributes = {
  .name = "SupBatMonitoring",
  .priority = (osPriority_t) osPriorityLow,
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
  /* creation of hearbeatTask */
  hearbeatTaskHandle = osThreadNew(startHeartbeatTask, NULL, &hearbeatTask_attributes);

  /* creation of sendStatusTask */
  sendStatusTaskHandle = osThreadNew(startSendStatusTask, NULL, &sendStatusTask_attributes);

  /* creation of outputsTask */
  outputsTaskHandle = osThreadNew(startOutputsTask, NULL, &outputsTask_attributes);

  /* creation of SupBatMonitoring */
  SupBatMonitoringHandle = osThreadNew(startSupBatMonitoring , NULL, &SupBatMonitoring_attributes);

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

/* USER CODE BEGIN Header_startSendStatusTask */
/**
* @brief Function implementing the sendStatusTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startSendStatusTask */
void startSendStatusTask(void *argument)
{
  /* USER CODE BEGIN sendStatusTask */
    startSendStatusTask_user(argument);
  /* USER CODE END sendStatusTask */
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

/* USER CODE BEGIN Header_startSupBatMonitoring */
/**
* @brief Function implementing the SupBatMonitoring thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startSupBatMonitoring */
void startSupBatMonitoring (void *argument)
{
  /* USER CODE BEGIN SupBatMonitoring */
  /* Infinite loop */
  for(;;)
  {
      startSupBatMonitoring_user(argument);
  }
  /* USER CODE END SupBatMonitoring */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

