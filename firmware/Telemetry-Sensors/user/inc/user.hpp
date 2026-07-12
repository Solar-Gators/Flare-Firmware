#ifndef FLAREFIRMWARE_USER_THREADS_HPP
#define FLAREFIRMWARE_USER_THREADS_HPP

#if __cplusplus
extern "C"
{
#endif

    [[noreturn]] void startDefaultTask_user(void* argument);
    [[noreturn]] void startMsgTask_user(void* argument);

#ifdef __cplusplus
}
#endif

#endif  //FLAREFIRMWARE_USER_THREADS_HPP
