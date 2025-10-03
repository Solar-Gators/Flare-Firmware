#include "user_threads.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "../../../../resources/Drivers/SG_Drivers/Inc/spi_target.hpp"
#include "eeprom93aa46.hpp"
#include "main.h"

extern SPI_HandleTypeDef hspi3;

void StartDefaultTask_user(void* argument)
{
    // intended way to make eeprom
    sg::Eeprom93AA46 eeprom(&hspi3, EEPROM_CS_GPIO_Port, EEPROM_CS_Pin, true);

    eeprom.writeByte(0, 0);
    eeprom.write(0, nullptr, 0);
    eeprom.read(0, nullptr, 0);

    for (;;)
    {
        osDelay(500);
    }
}
