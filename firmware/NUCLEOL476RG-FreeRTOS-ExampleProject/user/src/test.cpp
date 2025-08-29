#include "cmsis_os2.h"  // this works cause cmsis_os includes cmsis_os2 so u could write either
#include "main.h"
#include "stm32l4xx_hal.h"

#include "test.hpp"
#include "CanDriver.hpp"

void StartDefaultTask_user(void *argument) {

    CANDevice& device = CANDevice::getInstance();

    for (;;) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); // blink led
        osDelay(500);
    }
}
