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
/* Definitions for heartbeatTask */
osThreadId_t heartbeatTaskHandle;
uint32_t heartbeatTaskBuffer[ 128 ];
osStaticThreadDef_t heartbeatTaskCB;
const osThreadAttr_t heartbeatTask_attributes = {
  .name = "heartbeatTask",
  .stack_mem = &heartbeatTaskBuffer[0],
  .stack_size = sizeof(heartbeatTaskBuffer),
  .cb_mem = &heartbeatTaskCB,
  .cb_size = sizeof(heartbeatTaskCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for screenTask */
osThreadId_t screenTaskHandle;
uint32_t screenTaskBuffer[ 128 ];
osStaticThreadDef_t screenTaskCB;
const osThreadAttr_t screenTask_attributes = {
  .name = "screenTask",
  .stack_mem = &screenTaskBuffer[0],
  .stack_size = sizeof(screenTaskBuffer),
  .cb_mem = &screenTaskCB,
  .cb_size = sizeof(screenTaskCB),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for PollButtons */
osThreadId_t PollButtonsHandle;
const osThreadAttr_t PollButtons_attributes = {
  .name = "PollButtons",
  .priority = (osPriority_t) osPriorityNormal,
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
  /* creation of heartbeatTask */
  heartbeatTaskHandle = osThreadNew(startHeartbeatTask, NULL, &heartbeatTask_attributes);

  /* creation of screenTask */
  screenTaskHandle = osThreadNew(startScreenTask, NULL, &screenTask_attributes);

  /* creation of PollButtons */
  PollButtonsHandle = osThreadNew(StartPollButtons, NULL, &PollButtons_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_startHeartbeatTask */
/**
* @brief Function implementing the heartbeatTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startHeartbeatTask */
void startHeartbeatTask(void *argument)
{
  /* USER CODE BEGIN heartbeatTask */
    startHeartbeatTask_user(argument);
  /* USER CODE END heartbeatTask */
}

/* USER CODE BEGIN Header_startScreenTask */
/**
* @brief Function implementing the screenTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startScreenTask */
void startScreenTask(void *argument)
{
  /* USER CODE BEGIN screenTask */
    startScreenTask_user(argument);
  /* USER CODE END screenTask */
}

/* USER CODE BEGIN Header_StartPollButtons */
/**
* @brief Function implementing the PollButtons thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPollButtons */
void StartPollButtons(void *argument)
{
  /* USER CODE BEGIN PollButtons */
    startPollButtons_user(argument);
  /* USER CODE END PollButtons */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

