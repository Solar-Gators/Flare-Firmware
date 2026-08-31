#include "main.h"

#include "stm32u5xx_hal.h"

void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddA();
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void NMI_Handler(void)
{
    while (1)
    {
    }
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
