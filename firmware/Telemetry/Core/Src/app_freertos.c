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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for GPSReadBuffer */
osThreadId_t GPSReadBufferHandle;
const osThreadAttr_t GPSReadBuffer_attributes = {
  .name = "GPSReadBuffer",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for GPSParseNMEA */
osThreadId_t GPSParseNMEAHandle;
const osThreadAttr_t GPSParseNMEA_attributes = {
  .name = "GPSParseNMEA",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for TXRadio */
osThreadId_t TXRadioHandle;
const osThreadAttr_t TXRadio_attributes = {
  .name = "TXRadio",
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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of GPSReadBuffer */
  GPSReadBufferHandle = osThreadNew(StartGPSReadBuffer, NULL, &GPSReadBuffer_attributes);

  /* creation of GPSParseNMEA */
  GPSParseNMEAHandle = osThreadNew(StartGPSParseNMEA, NULL, &GPSParseNMEA_attributes);

  /* creation of TXRadio */
  TXRadioHandle = osThreadNew(StartTXRadio, NULL, &TXRadio_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  StartDefaultTask_user(argument);
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_StartGPSReadBuffer */
/**
* @brief Function implementing the GPSReadBuffer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGPSReadBuffer */
void StartGPSReadBuffer(void *argument)
{
  /* USER CODE BEGIN GPSReadBuffer */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END GPSReadBuffer */
}

/* USER CODE BEGIN Header_StartGPSParseNMEA */
/**
* @brief Function implementing the GPSParseNMEA thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGPSParseNMEA */
void StartGPSParseNMEA(void *argument)
{
  /* USER CODE BEGIN GPSParseNMEA */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END GPSParseNMEA */
}

/* USER CODE BEGIN Header_StartTXRadio */
/**
* @brief Function implementing the TXRadio thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTXRadio */
void StartTXRadio(void *argument)
{
  /* USER CODE BEGIN TXRadio */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TXRadio */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

