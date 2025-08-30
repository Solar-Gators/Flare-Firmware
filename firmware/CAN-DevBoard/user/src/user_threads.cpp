#include "cmsis_os2.h"  // this works cause cmsis_os includes cmsis_os2 so u could write either
#include "main.h"
#include "stm32u5xx_hal.h"

#include "user_threads.hpp"
#include "CanDriver.hpp"

void StartDefaultTask_user(void *argument) {

    CANDevice& device = CANDevice::getInstance();

    for (;;) {
        osDelay(500);
    }
}
