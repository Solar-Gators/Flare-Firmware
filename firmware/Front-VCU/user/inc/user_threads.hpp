#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#include <stdbool.h>

#ifdef __cplusplus
#include "ina226.hpp"

extern "C"
{
#endif

    [[noreturn]] void StartHeartbeat_user(void* argument);
    [[noreturn]] void StartCANMessagesTX_user(void* argument);

    void writeLeft(bool on);
    void writeRight(bool on);

    [[noreturn]] void StartLoadsControl_user(void* argument);
    [[noreturn]] void StartCurrentSense_user(void* argument);
    void init_user();

#ifdef __cplusplus
}
#endif

#endif
