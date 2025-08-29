#include "test.hpp"
#include "cmsis_os2.h" // this works cause cmsis_os includes cmsis_os2 so u could write either
#include "stm32l4xx_hal.h"
#include "main.h"

#include <atomic>

class fart {

};

void StartDefaultTask_user(void *argument) {
    std::atomic<int> t;
    for (;;) {
        volatile fart f;
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        osDelay(500);
    }
}
