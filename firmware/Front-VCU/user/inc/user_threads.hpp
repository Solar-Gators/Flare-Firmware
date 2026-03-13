#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void StartHeartbeat_user(void *argument);
    void StartThrottleBrakeRead_user(void *argument);
    void StartDefaultTask_user(void* argument);
    void StartCANMessagesTX_user(void* argument);
    void StartLightsControl_user(void* argument);
    void init_user();

#ifdef __cplusplus
}
#endif

#endif
