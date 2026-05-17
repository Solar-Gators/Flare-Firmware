#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void StartDefaultTask_user(void* argument);
    void StartStatusRedPWM_user(void *argument);
    void StartStatusFlash_user(void *argument);

#ifdef __cplusplus
}
#endif

#endif
