#ifndef USER_THREADS_HPP
#define USER_THREADS_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    void StartDefaultTask_user(void* argument);
    void StartGPSReadBuffer_user(void* argument);
    void StartGPSParseNMEA_user(void* argument);
    void StartStartTXRadio_user(void* argument);

#ifdef __cplusplus
}
#endif

#endif
