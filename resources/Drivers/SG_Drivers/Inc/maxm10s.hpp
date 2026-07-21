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
    explicit MaxM10S(I2C_HandleTypeDef* hi2c)
        : I2CDevice(hi2c, I2C_ADDRESS),
          position{},
          ground_speed_knots{},
          course_deg{},
          fix_valid{},
          fix_mode{},
          num_satellites{},
          quality{},
          date{},
          time{},
          gps_buffer{}
    {
    }
    void init();
    void readOutputBuffer();
    void parseNMEA();

    struct Position
    {
        double latitude_deg;
        double longitude_deg;
    };

    Position getPosition() const
    {
        osMutexAcquire(long_lat_read_mutex, osWaitForever);
        Position temp = position;
        osMutexRelease(long_lat_read_mutex);
        return temp;
    }
    float getSpeed() const
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        float temp = ground_speed_knots;
        osMutexRelease(fix_data_mutex);
        return temp;
    }
    uint8_t getNumSatellites() const
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        uint8_t temp = num_satellites;
        osMutexRelease(fix_data_mutex);
        return temp;
    }
    uint8_t getQuality() const
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        uint8_t temp = quality;
        osMutexRelease(fix_data_mutex);
        return temp;
    }
    uint16_t getDate() const
    {
        osMutexAcquire(fix_data_mutex, osWaitForever);
        uint16_t temp = date;
        osMutexRelease(fix_data_mutex);
        return temp;
    }

   private:
    static constexpr uint32_t CFG_RATE_MEAS_KEY = 0x30210001U;
    static constexpr uint16_t NAV_RATE_4HZ_MS = 250U;

    static constexpr uint8_t UBX_SYNC_CHAR_1 = 0xB5;
    static constexpr uint8_t UBX_SYNC_CHAR_2 = 0x62;
    static constexpr uint8_t UBX_CLASS_CFG = 0x06;
    static constexpr uint8_t UBX_ID_CFG_VALSET = 0x8A;
    static constexpr uint8_t UBX_CFG_LAYER_RAM = 0x01;

    uint16_t getDataLength();
    bool configureNavigationRate();
    void appendUbxChecksum(uint8_t* message, size_t length);
    bool extractSentence(char* sentence, size_t sentence_capacity);
    void processSentence(char* sentence);
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
    SemaphoreHandle_t fix_data_mutex = nullptr;
    SemaphoreHandle_t long_lat_read_mutex = nullptr;
#endif

    static constexpr size_t GPS_BUFFER_SIZE = 1024;
    static constexpr size_t MAX_NMEA_SENTENCE_SIZE = 256;
    volatile uint8_t gps_buffer[GPS_BUFFER_SIZE];
    volatile uint16_t gps_head = 0;
    volatile uint16_t gps_tail = 0;

    static constexpr uint8_t I2C_ADDRESS = 0x42;
    static constexpr uint8_t LEN_REG_HIGH = 0xFD;
    static constexpr uint8_t DATA_REG = 0xFF;
};
