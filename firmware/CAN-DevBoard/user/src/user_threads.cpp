#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "eeprom_93aa46.hpp"
#include "main.h"

extern "C"
{
    SPI_HandleTypeDef hspi3;
}

void StartDefaultTask_user(void* argument)
{
    Eeprom93AA46 eeprom(&hspi3, EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);

    for (;;)
    {
        osDelay(500);
    }
}
