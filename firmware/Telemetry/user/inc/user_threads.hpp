#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void init_user();

    void startDefaultTask_user(void* argument);
    void startGPSReadBuffer_user(void* argument);
    void startGPSParseNMEA_user(void* argument);
    void startTXRadio_user(void* argument);

#ifdef __cplusplus
}
#endif

#endif
