#include "driver_LTC3300-2.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"

#include <functional>

inline bool checkParity(uint8_t byte) {
    byte ^= byte >> 4;
    byte ^= byte >> 2;
    byte ^= byte >> 1;

    // Returns 1 if odd (needs a 1 to become even), 0 if even
    return (byte & 1);
}

void genCRC4LUT(uint8_t LUT[256],uint8_t const polynomial)
{

    #define TOPBITBM 1 << 7 //Bitmask to hold to the top of the nibble

    uint8_t const truncPoly = polynomial << 4;

    //Repeats for all the possible dividends
    for (int dividend = 0; dividend < 256; ++dividend)
    {
        uint8_t remainder = dividend;

        //Repeats to step through 7 bits
        for (uint8_t i = 8; i > 0; --i)
        {

            if (remainder & TOPBITBM)
            {
                remainder = (remainder  << 1) ^ truncPoly;
            }
            else
            {
                remainder = (remainder << 1);
            }

        }

        //Sets the value in the LUT adjusted to the lower nibble of the data
        LUT[dividend] = (remainder>>4) & 0x0F;

    }
}


uint8_t calcCRC4Remainder(uint8_t const message[], uint8_t msgSize, uint8_t const CRC4LUT[256])
{
    uint8_t remainder = 0;
    uint8_t data = 0;

    //Shifts in the highest bit at the start of the next byte
    for (uint8_t byteInd = 0; byteInd < msgSize; ++byteInd)
    {
        //Calculating CRC is distributive so finding the CRC of the 16 bit message is the same as XORing the CRCs of the two bytes
        data = message[byteInd] ^ (remainder << 4);
        remainder = CRC4LUT[data];

    }

    return remainder & 0x0F;
}


bool generateBalCmd(uint8_t cmdArray[2], uint8_t const CRC4LUT[256], uint8_t chargeBM, uint8_t dischargeSyncBM, uint8_t dischargeNonBM)
{

#define chargeCMD 0b11
#define dischargeSyncCMD 0b10
#define dischargeNonCMD 0b01

    cmdArray[0] = 0;
    cmdArray[1] = 0;

    //Checks to ensure a cell is not set in multiple bitmasks
    if ((chargeBM & dischargeSyncBM) & (chargeBM & dischargeNonBM) & (dischargeSyncBM & dischargeNonBM) != 0){
        //A complete 0 BM will be returned.
        //Not only will this set all cells to do nothing, but the CRC would be incorrect, meaning that the command cannot change the operation)
        cmdArray[0] = 0;
        cmdArray[1] = 0;
        return false;
    }

    //Defines two arrays to store the BM and commands to allow for iteration
    uint8_t const balBMArray[3] = {chargeBM, dischargeSyncBM, dischargeNonBM};
    uint8_t const balCmdArray[3] = {chargeCMD, dischargeSyncCMD, dischargeNonCMD};

    //Saved as a 16-bit value to make processing easier
    uint16_t longBalCmd = 0;

    //Iterates through the three command operations
    for (uint8_t operation = 0; operation < 3; ++operation)
    {
        for (uint8_t i = 0; i < 6; ++i)
        {
            //Tests for the given cell within the given
            if (balBMArray[operation] & 1 << (5-i))
            {
                longBalCmd |= balCmdArray[operation] << (i * 2);
            }

        }
    }

    //Converts the long 16-bit command into a byte array
    //For proper CRC calculation the value must be passed as a 12-bit value
    cmdArray[0] = longBalCmd >> 8;
    cmdArray[1] = longBalCmd & 0xFF;

    //Generates the inverted CRC data
    uint8_t crcAppend = ~ calcCRC4Remainder(cmdArray, 2, CRC4LUT);

    //Shifts the data to add the 4 CRC bits
    cmdArray[0] = (cmdArray[0] << 4) | (cmdArray[1] >> 4); //Upper nibble is bits 11-8 Lower is bits 7-4 of 12 bit message
    cmdArray[1] = (cmdArray[1] << 4) | (crcAppend & 0x0F); //Upper nibble is bits 3-0 of message and the 4 CRC bits



    return true;
}

LTC3300_2::LTC3300_2(uint8_t deviceAddress, uint8_t CRC4Poly, SPI_HandleTypeDef hspi, GPIO_TypeDef* CSPinGPIOx, uint16_t CSPin)
{

    Address = deviceAddress;
    CRCPolynomial = CRC4Poly;
    Balhspi = hspi;
    BalCSPinGPIOx = CSPinGPIOx;
    BalCSPin = CSPin;

    //Data Handling
    genCRC4LUT(CRCLUT, CRCPolynomial);

}


bool LTC3300_2::writeBalCommand(uint8_t chargeBM, uint8_t dischargeSyncBM, uint8_t dischargeNonBM)
{
    //Generates and saves the balance command to the second 2 bytes of the Tx buffer
    if (!generateBalCmd(&TxBuff[1], CRCLUT, chargeBM, dischargeSyncBM, dischargeNonBM)) return false;

    //Combines the address with the balance command
    TxBuff[0] = (Address << 3) | writeBalCmdBM;
    //Adds the parity bit
    TxBuff[0] |= checkParity(TxBuff[0]);

    //Writes to the SPI device
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&Balhspi, TxBuff, 3, 1000) != HAL_OK) return false;
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);
    return true;
}

uint8_t* LTC3300_2::readbackBalCmd()
{
    //Adds the readback command to the Tx buffer with junk bytes
    //Junk bytes are needed for the SPI device to return data
    TxBuff[0] = (Address << 3) | readbackBalCmdBM;
    TxBuff[0] |= checkParity(TxBuff[0]);
    TxBuff[1] = 0x01;
    TxBuff[2] = 0x01;


    //Writes to the SPI device
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&Balhspi, TxBuff, RxBuff, 3, 1000);
    // while (HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY);
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);

    return &RxBuff[1];

}

uint8_t* LTC3300_2::readBalanceStatus()
{
    //Adds the balance status command to the Tx buffer with junk bytes
    //Junk bytes are needed for the SPI device to return data
    TxBuff[0] = (Address << 3) | readBalStatusBM;
    TxBuff[0] |= checkParity(TxBuff[0]);
    TxBuff[1] = 0x02;
    TxBuff[2] = 0x02;


    //Writes to the SPI device
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&Balhspi, TxBuff, RxBuff, 3, 1000);
    // while (HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY);
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);

    return &RxBuff[1];

}

void LTC3300_2::executeBalanceCommand()
{
    //Combines the address with the execute balance command
    TxBuff[0] = (Address << 3) | executeBalCmdBM;
    //Adds the parity bit
    TxBuff[0] |= checkParity(TxBuff[0]);
    TxBuff[1] = 0x03;
    TxBuff[2] = 0x03;

    //Writes to the SPI device
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&Balhspi, TxBuff, 3, 1000);
    // if (HAL_SPI_Transmit(Balhspi, TxBuff, 3, 1000) != HAL_OK) return false;
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);
}

void LTC3300_2::haltBalanceCommand()
{
    //Combines the address with the execute balance command
    TxBuff[0] = (Address << 3) | executeBalCmdBM;
    //Adds the opposite of the parity bit so that it is odd parity (pauses)
    TxBuff[0] |= (!checkParity(TxBuff[0]));
    TxBuff[1] = 0x04;
    TxBuff[2] = 0x04;

    //Writes to the SPI device
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&Balhspi, TxBuff, 3, 1000);
    // if (HAL_SPI_Transmit(Balhspi, TxBuff, 3, 1000) != HAL_OK) return false;
    HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);
}

uint8_t* LTC3300_2::readRxBuffer()
{
    return &RxBuff[0];
}

uint8_t LTC3300_2::readAddress()
{
    return Address;
}