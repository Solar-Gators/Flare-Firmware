#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    [[noreturn]] void startHeartbeatTask_user(void* argument);
    [[noreturn]] void startScreenTask_user(void* argument);
    [[noreturn]] void startPollButtons_user(void* argument);

#ifdef __cplusplus
}
#endif

#endif
