/*
Nathan Achinger
Ublox MAX M10S Driver
*/
#pragma once

#include <cstdio>
#include <cstring>

#include <stdlib.h>

#if defined(STM32U575xx)
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_i2c.h"
#elif defined(STM32L467xx)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"
#endif

#include <string>

#ifdef USING_FREERTOS
#include "cmsis_os2.h"

#include "FreeRTOS.h"
#include "semphr.h"
#endif

#define GPS_BUFFER_SIZE 1024

class MaxM10S
{
   public:
    MaxM10S(I2C_HandleTypeDef* hi2c);
    void init();
    void readOutputBuffer();
    void parseNMEA();

    struct Position
    {
        double latitude_deg;
        double longitude_deg;
        ;
    };

    Position getPosition() { return position; }
    float getSpeed() { return ground_speed_knots; }
    uint8_t getNumSatellites() { return num_satellites; }
    uint8_t getQuality() { return quality; }
    uint16_t getDate() { return date; }

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
#endif

    volatile uint8_t gps_buffer[GPS_BUFFER_SIZE];
    volatile uint16_t gps_head = 0;
    volatile uint16_t gps_tail = 0;

    I2C_HandleTypeDef* i2c_handle;
    static constexpr uint8_t I2C_ADDRESS = 0x42;
    static constexpr uint8_t LEN_REG_HIGH = 0xFD;
    static constexpr uint8_t DATA_REG = 0xFF;
};
