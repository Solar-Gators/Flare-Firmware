/*
Nathan Achinger
Ublox MAX M10S Driver
*/
#pragma once

#define USING_FREERTOS 1

#include <cstdio>
#include <cstring>

#include <stdlib.h>

#include "i2c_api.hpp"
#include "main.h"

#include <string>

#ifdef USING_FREERTOS
#include "cmsis_os2.h"

#include "FreeRTOS.h"
#include "semphr.h"
#endif

class MaxM10S : public I2CDevice
{
   public:
    MaxM10S(I2C_HandleTypeDef* hi2c) : I2CDevice(hi2c, I2C_ADDRESS){};
    void init();
    void readOutputBuffer();
    void parseNMEA();

    struct Position
    {
        double latitude_deg;
        double longitude_deg;
    };

    Position getPosition()
    {
        osMutexAcquire(long_lat_read_mutex, osWaitForever);
        Position temp = position;
        osMutexRelease(long_lat_read_mutex);
        return temp;
    }
    float getSpeed()
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        float temp = ground_speed_knots;
        osMutexRelease(fix_data_mutex);
        return temp;
    }
    uint8_t getNumSatellites()
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        uint8_t temp = num_satellites;
        osMutexRelease(fix_data_mutex);
        return temp;
    }
    uint8_t getQuality()
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        uint8_t temp = quality;
        osMutexRelease(fix_data_mutex);
        return temp;
    }
    uint16_t getDate()
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        uint16_t temp = date;
        osMutexRelease(fix_data_mutex);
        return temp;
    }

   private:
    uint16_t getDataLength();
    void parseGNRMC(char* sentence);
    void parseGNGGA(char* sentence);
    double nmeaToDecimal(const char* nmeaCoord, const char direction);
    bool NMEAchecksumValid(const char* sentence);

    Position position;
    float ground_speed_knots;
    float course_deg;
    bool fix_valid;
    uint8_t fix_mode;
    uint8_t num_satellites;
    uint8_t quality;
    uint16_t date;
    char time[16];

#ifdef USING_FREERTOS
    SemaphoreHandle_t buffer_mutex = nullptr;
    SemaphoreHandle_t fix_data_mutex = nullptr;
    SemaphoreHandle_t long_lat_read_mutex = nullptr;
#endif

    static constexpr size_t GPS_BUFFER_SIZE = 1024;
    volatile uint8_t gps_buffer[GPS_BUFFER_SIZE];
    volatile uint16_t gps_head = 0;
    volatile uint16_t gps_tail = 0;

    I2C_HandleTypeDef* i2c_handle;
    static constexpr uint8_t I2C_ADDRESS = 0x42;
    static constexpr uint8_t LEN_REG_HIGH = 0xFD;
    static constexpr uint8_t DATA_REG = 0xFF;
};