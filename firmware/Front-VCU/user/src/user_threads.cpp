#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <main.h>
#include <stm32u5xx_hal.h>

extern ADC_HandleTypeDef hadc1;

void StartHeartbeat_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
    	osDelay(500);
        while ()

    }
}

void StartThrottleRead_user(void* argument)
{
    for (;;)
    {
        uint32_t adc_buffer[20];
        HAL_ADC_Start_DMA(&hadc1, adc_buffer, 20);

    	osDelay(500);
    }
}


