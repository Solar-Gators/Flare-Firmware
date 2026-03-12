#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    < < < < < < < Updated upstream void init_user();

    [[noreturn]] void startHeartbeatTask_user(void* argument);
    [[noreturn]] void startScreenTask_user(void* argument);
    [[noreturn]] void startPollButtons_user(void* argument);
=======
[[noreturn]] void startHeartbeatTask_user(void* argument);
[[noreturn]] void startScreenTask_user(void* argument);
>>>>>>> Stashed changes

#ifdef __cplusplus
}
#endif

#endif
