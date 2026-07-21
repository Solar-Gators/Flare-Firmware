#include "maxm10s.hpp"

#include "main.h"

void MaxM10S::init()
{
#ifdef USING_FREERTOS
    fix_data_mutex = xSemaphoreCreateMutex();
    long_lat_read_mutex = xSemaphoreCreateMutex();

    if (fix_data_mutex == nullptr)
    {
        Error_Handler();
    }
    if (long_lat_read_mutex == nullptr)
    {
        Error_Handler();
    }
#endif

    if (!configureNavigationRate())
    {
        Error_Handler();
    }
}

void MaxM10S::appendUbxChecksum(uint8_t* message, size_t length)
{
    uint8_t ck_a = 0;
    uint8_t ck_b = 0;

    for (size_t i = 2; i < length; i++)
    {
        ck_a = static_cast<uint8_t>(ck_a + message[i]);
        ck_b = static_cast<uint8_t>(ck_b + ck_a);
    }

    message[length] = ck_a;
    message[length + 1] = ck_b;
}

bool MaxM10S::configureNavigationRate()
{
    uint8_t message[] = {
        UBX_SYNC_CHAR_1,
        UBX_SYNC_CHAR_2,
        UBX_CLASS_CFG,
        UBX_ID_CFG_VALSET,
        0x0A,
        0x00,  // payload length
        0x00,  // version
        UBX_CFG_LAYER_RAM,
        0x00,
        0x00,  // reserved
        static_cast<uint8_t>(CFG_RATE_MEAS_KEY),
        static_cast<uint8_t>(CFG_RATE_MEAS_KEY >> 8),
        static_cast<uint8_t>(CFG_RATE_MEAS_KEY >> 16),
        static_cast<uint8_t>(CFG_RATE_MEAS_KEY >> 24),
        static_cast<uint8_t>(NAV_RATE_4HZ_MS),
        static_cast<uint8_t>(NAV_RATE_4HZ_MS >> 8),
    };

    appendUbxChecksum(message, sizeof(message));

    return writeN(DATA_REG, message, sizeof(message) + 2) == HAL_OK;
}

void MaxM10S::readOutputBuffer()
{
    uint16_t bytes_available = getDataLength();
    uint8_t rx_buff[256];

    if (bytes_available > 0)
    {
        if (bytes_available > 256)
        {
            bytes_available = 256;
        }

        if (readN(DATA_REG, rx_buff, bytes_available) != HAL_OK)
        {
            return;
        }

        for (uint16_t i = 0; i < bytes_available; i++)
        {
            uint16_t next_head = (gps_head + 1) % GPS_BUFFER_SIZE;

            if (next_head == gps_tail)
            {
                // Buffer full: drop the oldest byte to make room for newer data.
                gps_tail = (gps_tail + 1) % GPS_BUFFER_SIZE;
                next_head = (gps_head + 1) % GPS_BUFFER_SIZE;
            }

            gps_buffer[gps_head] = rx_buff[i];
            gps_head = next_head;
        }
    }
}

bool MaxM10S::extractSentence(char* sentence, size_t sentence_capacity)
{
    while (gps_tail != gps_head)
    {
        if (gps_buffer[gps_tail] != '$')
        {
            gps_tail = (gps_tail + 1) % GPS_BUFFER_SIZE;
            continue;
        }

        const uint16_t start = gps_tail;
        uint16_t end = start;
        bool found_newline = false;

        uint16_t i = (start + 1) % GPS_BUFFER_SIZE;
        while (i != gps_head)
        {
            if (gps_buffer[i] == '\n')
            {
                end = i;
                found_newline = true;
                break;
            }
            i = (i + 1) % GPS_BUFFER_SIZE;
        }

        if (!found_newline)
        {
            return false;
        }

        size_t sentence_length = 0;
        uint16_t j = start;
        while (j != (end + 1) % GPS_BUFFER_SIZE)
        {
            sentence_length++;
            j = (j + 1) % GPS_BUFFER_SIZE;
        }

        gps_tail = (end + 1) % GPS_BUFFER_SIZE;

        if (sentence_length == 0 || sentence_length >= sentence_capacity)
        {
            continue;
        }

        j = start;
        size_t idx = 0;
        while (j != (end + 1) % GPS_BUFFER_SIZE)
        {
            sentence[idx++] = static_cast<char>(gps_buffer[j]);
            j = (j + 1) % GPS_BUFFER_SIZE;
        }
        sentence[idx] = '\0';
        return true;
    }

    return false;
}

void MaxM10S::processSentence(char* sentence)
{
    if (!NMEAchecksumValid(sentence))
    {
        return;
    }

    if (strncmp(sentence, "$GNRMC", 6) == 0)
    {
        parseGNRMC(sentence);
    }
    else if (strncmp(sentence, "$GNGGA", 6) == 0)
    {
        parseGNGGA(sentence);
    }
}

void MaxM10S::parseNMEA()
{
    char sentence[MAX_NMEA_SENTENCE_SIZE];

    while (extractSentence(sentence, sizeof(sentence)))
    {
        processSentence(sentence);
    }
}

uint16_t MaxM10S::getDataLength()
{
    uint8_t rx[2];

    if (readN(LEN_REG_HIGH, rx, 2) != HAL_OK)
    {
        return 0;
    }

    return (static_cast<uint16_t>(rx[0]) << 8) | rx[1];
}

// Parses a GNRMC sentence to extract time, latitude, and longitude
void MaxM10S::parseGNRMC(char* sentence)
{
    char* saveptr = nullptr;
    char* token = strtok_r(sentence, ",", &saveptr);  // Tokenize the string by commas

    int index = 0;  // Keeps track of field

    while (token != nullptr)
    {
        switch (index)
        {
            case 1:
                // Field 1: UTC time in hhmmss.sss format
                // TODO: finish code to get time
                //fix_data.time = token;
                break;

            case 3:
            {
                // Field 3: Latitude (in NMEA format)
                const char* latStr = token;
                const char* latDir = strtok_r(nullptr, ",", &saveptr);  // Field 4: N/S
                index++;

                if (latDir != nullptr && latStr[0] != '\0')
                {
                    osMutexAcquire(long_lat_read_mutex, osWaitForever);
                    this->position.latitude_deg = nmeaToDecimal(latStr, *latDir);
                    osMutexRelease(long_lat_read_mutex);
                }
                break;
            }

            case 5:
            {
                // Field 5: Longitude (in NMEA format)
                const char* lonStr = token;
                const char* lonDir = strtok_r(nullptr, ",", &saveptr);  // Field 6: E/W
                index++;

                if (lonDir != nullptr && lonStr[0] != '\0')
                {
                    osMutexAcquire(long_lat_read_mutex, osWaitForever);
                    this->position.longitude_deg = nmeaToDecimal(lonStr, *lonDir);
                    osMutexRelease(long_lat_read_mutex);
                }
                break;
            }

            case 7:
                // Field 7: Speed over ground in knots
                osMutexAcquire(fix_data_mutex, osWaitForever);
                this->ground_speed_knots = atof(token);
                osMutexRelease(fix_data_mutex);
                break;

            default:
                break;
        }

        // Move to next comma-separated token
        token = strtok_r(nullptr, ",", &saveptr);
        index++;
    }
}

// Parses a GNGGA sentence to extract time, position, fix status, satellites, HDOP, and altitude
void MaxM10S::parseGNGGA(char* sentence)
{
    char* saveptr;

    char* token = strtok_r(sentence, ",", &saveptr);  // Tokenize by commas
    int index = 0;                                    // Field index

    while (token != nullptr)
    {
        switch (index)
        {
            case 1:
                // Field 1: UTC time
                //fix_data.time = token;
                break;

            case 2:
            {
                // Field 2: Latitude
                const char* latStr = token;
                const char* latDir = strtok_r(nullptr, ",", &saveptr);  // Field 3: N/S
                index++;

                if (latDir != nullptr && latStr[0] != '\0')
                {
                    osMutexAcquire(long_lat_read_mutex, osWaitForever);
                    this->position.latitude_deg = nmeaToDecimal(latStr, *latDir);
                    osMutexRelease(long_lat_read_mutex);
                }
                break;
            }

            case 4:
            {
                // Field 4: Longitude
                const char* lonStr = token;
                const char* lonDir = strtok_r(nullptr, ",", &saveptr);  // Field 5: E/W
                index++;

                if (lonDir != nullptr && lonStr[0] != '\0')
                {
                    osMutexAcquire(long_lat_read_mutex, osWaitForever);
                    this->position.longitude_deg = nmeaToDecimal(lonStr, *lonDir);
                    osMutexRelease(long_lat_read_mutex);
                }
                break;
            }

            case 6:
                // Field 6: Fix quality (0 = invalid, 1 = GPS, 2 = DGPS)
                osMutexAcquire(fix_data_mutex, osWaitForever);
                this->quality = atoi(token);
                osMutexRelease(fix_data_mutex);
                break;

            case 7:
                // Field 7: Number of satellites in use
                osMutexAcquire(fix_data_mutex, osWaitForever);
                this->num_satellites = atoi(token);
                osMutexRelease(fix_data_mutex);
                break;

            default:
                break;
        }

        // Next token
        token = strtok_r(nullptr, ",", &saveptr);
        index++;
    }
}

double MaxM10S::nmeaToDecimal(const char* nmeaCoord, const char direction)
{
    double raw = atof(nmeaCoord);  // Convert string to double

    // Separate degrees and minutes
    int degrees = (int) (raw / 100);
    double minutes = raw - (degrees * 100);

    // Convert to decimal degrees
    double decimal = degrees + (minutes / 60.0);

    // Apply sign based on direction
    if (direction == 'S' || direction == 'W')
    {
        decimal = -decimal;
    }

    return decimal;
}

bool MaxM10S::NMEAchecksumValid(const char* sentence)
{
    if (sentence[0] != '$')
        return false;

    uint8_t checksum = 0;
    const char* p = sentence + 1;

    // XOR all chars until '*'
    while (*p && *p != '*')
    {
        checksum ^= static_cast<uint8_t>(*p);
        p++;
    }

    if (*p != '*')
        return false;  // '*' not found

    // Parse the checksum after '*'
    auto received_checksum = static_cast<uint8_t>(strtol(p + 1, NULL, 16));

    return (checksum == received_checksum);
}
