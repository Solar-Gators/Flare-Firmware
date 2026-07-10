#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
#include <stm32u5xx_hal.h>
extern "C"
{
#endif

    void init_user();

    [[noreturn]] void startHeartbeatTask_user(void* argument);
    [[noreturn]] void startScreenTask_user(void* argument);
    [[noreturn]] void startPollButtons_user(void* argument);
    [[noreturn]] void startHeartbeatTask_user(void* argument);
    [[noreturn]] void startScreenTask_user(void* argument);
    [[noreturn]] void startDancingFlareScreenTask_user(void* argument);
    void HAL_TIM_PeriodElapsedCallback_user(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif
