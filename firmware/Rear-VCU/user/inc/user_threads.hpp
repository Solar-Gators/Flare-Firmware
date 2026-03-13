#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void init_user();

    [[noreturn]] void startHeartbeatTask_user(void* argument);
    [[noreturn]] void startOutputsTask_user(void* argument);
    [[noreturn]] void startSendStatusTask_user(void* argument);

#ifdef __cplusplus
}
#endif

#endif
