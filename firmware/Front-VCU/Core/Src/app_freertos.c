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
/* Definitions for ThrottleBrakeRead */
osThreadId_t ThrottleBrakeReadHandle;
const osThreadAttr_t ThrottleBrakeRead_attributes = {
  .name = "ThrottleBrakeRead",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 128 * 4
};
/* Definitions for LoadsControl */
osThreadId_t LoadsControlHandle;
const osThreadAttr_t LoadsControl_attributes = {
  .name = "LoadsControl",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for CANMessagesTX */
osThreadId_t CANMessagesTXHandle;
const osThreadAttr_t CANMessagesTX_attributes = {
  .name = "CANMessagesTX",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for CurrentSense */
osThreadId_t CurrentSenseHandle;
const osThreadAttr_t CurrentSense_attributes = {
  .name = "CurrentSense",
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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ThrottleBrakeRead */
  ThrottleBrakeReadHandle = osThreadNew(StartThrottleBrakeRead, NULL, &ThrottleBrakeRead_attributes);

  /* creation of LoadsControl */
  LoadsControlHandle = osThreadNew(StartLightsControl, NULL, &LoadsControl_attributes);

  /* creation of CANMessagesTX */
  CANMessagesTXHandle = osThreadNew(StartCANMessagesTX, NULL, &CANMessagesTX_attributes);

  /* creation of CurrentSense */
  CurrentSenseHandle = osThreadNew(StartCurrentSense, NULL, &CurrentSense_attributes);

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
    StartHeartbeat_user(argument);
    for(;;) {
        osDelay(1);
    }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_StartThrottleBrakeRead */
/**
* @brief Function implementing the ThrottleBrakeRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartThrottleBrakeRead */
void StartThrottleBrakeRead(void *argument)
{
  /* USER CODE BEGIN ThrottleBrakeRead */
  /* Infinite loop */

    StartThrottleBrakeRead_user(argument);
    for(;;)
    {
        osDelay(1);
    }
  /* USER CODE END ThrottleBrakeRead */
}

/* USER CODE BEGIN Header_StartLightsControl */
/**
* @brief Function implementing the LoadsControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLightsControl */
void StartLightsControl(void *argument)
{
  /* USER CODE BEGIN LoadsControl */
  /* Infinite loop */
  StartLoadsControl_user(argument);
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LoadsControl */
}

/* USER CODE BEGIN Header_StartCANMessagesTX */
/**
* @brief Function implementing the CANMessagesTX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCANMessagesTX */
void StartCANMessagesTX(void *argument)
{
  /* USER CODE BEGIN CANMessagesTX */
  /* Infinite loop */
  StartCANMessagesTX_user(argument);
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CANMessagesTX */
}

/* USER CODE BEGIN Header_StartCurrentSense */
/**
* @brief Function implementing the CurrentSense thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCurrentSense */
void StartCurrentSense(void *argument)
{
  /* USER CODE BEGIN CurrentSense */
    StartCurrentSense_user(argument);
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CurrentSense */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

