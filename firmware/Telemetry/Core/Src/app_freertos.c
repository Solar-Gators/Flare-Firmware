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
/* Definitions for HeartbeatTask */
osThreadId_t HeartbeatTaskHandle;
uint32_t HeartbeatTaskBuffer[ 128 ];
osStaticThreadDef_t HeartbeatTaskCB;
const osThreadAttr_t HeartbeatTask_attributes = {
  .name = "HeartbeatTask",
  .stack_mem = &HeartbeatTaskBuffer[0],
  .stack_size = sizeof(HeartbeatTaskBuffer),
  .cb_mem = &HeartbeatTaskCB,
  .cb_size = sizeof(HeartbeatTaskCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GPSReadBufferTask */
osThreadId_t GPSReadBufferTaskHandle;
uint32_t GPSReadBufferBuffer[ 512 ];
osStaticThreadDef_t GPSReadBufferCB;
const osThreadAttr_t GPSReadBufferTask_attributes = {
  .name = "GPSReadBufferTask",
  .stack_mem = &GPSReadBufferBuffer[0],
  .stack_size = sizeof(GPSReadBufferBuffer),
  .cb_mem = &GPSReadBufferCB,
  .cb_size = sizeof(GPSReadBufferCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GPSParseNMEATask */
osThreadId_t GPSParseNMEATaskHandle;
uint32_t GPSParseNMEABuffer[ 256 ];
osStaticThreadDef_t GPSParseNMEACB;
const osThreadAttr_t GPSParseNMEATask_attributes = {
  .name = "GPSParseNMEATask",
  .stack_mem = &GPSParseNMEABuffer[0],
  .stack_size = sizeof(GPSParseNMEABuffer),
  .cb_mem = &GPSParseNMEACB,
  .cb_size = sizeof(GPSParseNMEACB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TXRadioTask */
osThreadId_t TXRadioTaskHandle;
uint32_t TXRadioBuffer[ 128 ];
osStaticThreadDef_t TXRadioCB;
const osThreadAttr_t TXRadioTask_attributes = {
  .name = "TXRadioTask",
  .stack_mem = &TXRadioBuffer[0],
  .stack_size = sizeof(TXRadioBuffer),
  .cb_mem = &TXRadioCB,
  .cb_size = sizeof(TXRadioCB),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for KillSwitchTask */
osThreadId_t KillSwitchTaskHandle;
uint32_t KillSwitchTaskBuffer[ 128 ];
osStaticThreadDef_t KillSwitchTaskCB;
const osThreadAttr_t KillSwitchTask_attributes = {
  .name = "KillSwitchTask",
  .stack_mem = &KillSwitchTaskBuffer[0],
  .stack_size = sizeof(KillSwitchTaskBuffer),
  .cb_mem = &KillSwitchTaskCB,
  .cb_size = sizeof(KillSwitchTaskCB),
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
  /* creation of HeartbeatTask */
  HeartbeatTaskHandle = osThreadNew(StartHeartbeatTask, NULL, &HeartbeatTask_attributes);

  /* creation of GPSReadBufferTask */
  GPSReadBufferTaskHandle = osThreadNew(StartGPSReadBufferTask, NULL, &GPSReadBufferTask_attributes);

  /* creation of GPSParseNMEATask */
  GPSParseNMEATaskHandle = osThreadNew(StartGPSParseNMEATask, NULL, &GPSParseNMEATask_attributes);

  /* creation of TXRadioTask */
  TXRadioTaskHandle = osThreadNew(StartTXRadioTask, NULL, &TXRadioTask_attributes);

  /* creation of KillSwitchTask */
  KillSwitchTaskHandle = osThreadNew(StartKillSwitchTask, NULL, &KillSwitchTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartHeartbeatTask */
/**
* @brief Function implementing the HeartbeatTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHeartbeatTask */
void StartHeartbeatTask(void *argument)
{
  /* USER CODE BEGIN HeartbeatTask */
    startHeartbeatTask_user(argument);
  /* USER CODE END HeartbeatTask */
}

/* USER CODE BEGIN Header_StartGPSReadBufferTask */
/**
* @brief Function implementing the GPSReadBufferTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGPSReadBufferTask */
void StartGPSReadBufferTask(void *argument)
{
  /* USER CODE BEGIN GPSReadBufferTask */
    startGPSReadBufferTask_user(argument);
  /* USER CODE END GPSReadBufferTask */
}

/* USER CODE BEGIN Header_StartGPSParseNMEATask */
/**
* @brief Function implementing the GPSParseNMEATask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGPSParseNMEATask */
void StartGPSParseNMEATask(void *argument)
{
  /* USER CODE BEGIN GPSParseNMEATask */
    startGPSParseNMEATask_user(argument);
  /* USER CODE END GPSParseNMEATask */
}

/* USER CODE BEGIN Header_StartTXRadioTask */
/**
* @brief Function implementing the TXRadioTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTXRadioTask */
void StartTXRadioTask(void *argument)
{
  /* USER CODE BEGIN TXRadioTask */
    startTXRadioTask_user(argument);
  /* USER CODE END TXRadioTask */
}

/* USER CODE BEGIN Header_StartKillSwitchTask */
/**
* @brief Function implementing the KillSwitchTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKillSwitchTask */
void StartKillSwitchTask(void *argument)
{
  /* USER CODE BEGIN KillSwitchTask */
    startKillSwitchTask_user(argument);
  /* USER CODE END KillSwitchTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

