#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#include "stm32u5xx_hal.h"

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t radioTXQueue;

#ifdef __cplusplus
extern "C"
{
#endif

    void init_user();

    [[noreturn]] void startHeartbeatTask_user(void* argument);
    [[noreturn]] void startGPSReadBufferTask_user(void* argument);
    [[noreturn]] void startGPSParseNMEATask_user(void* argument);
    [[noreturn]] void startTXRadioTask_user(void* argument);
    [[noreturn]] void startKillSwitchMessageTask_user(void* argument);
    [[noreturn]] void startLightsOutputsTask_user(void* argument);

#ifdef __cplusplus
}
#endif

#endif
