#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
#include "ina226.hpp"
extern "C"
{
#endif

    void StartHeartbeat_user(void *argument);
    void StartThrottleBrakeRead_user(void *argument);
    void StartDefaultTask_user(void* argument);
    void StartCANMessagesTX_user(void* argument);
    void StartLoadsControl_user(void* argument);
    void StartCurrentSense_user(void* argument);
    void init_user();

#ifdef __cplusplus
}
#endif

#endif
