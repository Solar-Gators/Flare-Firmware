#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void StartHeartbeat_user(void *argument);
    void StartThrottleRead_user(void *argument);


#ifdef __cplusplus
}
#endif

#endif
