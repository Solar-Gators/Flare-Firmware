#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void init_user();

    void startHeartbeatTask_user(void *argument);
    void startRegenThrottleTask_user(void *argument);
    void startOutputsTask_user(void *argument);

#ifdef __cplusplus
}
#endif

#endif
