#include "user_threads.hpp"
#include "driver_LTC3300-2.hpp"

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "main.h"

extern SPI_HandleTypeDef hspi3;

void StartDefaultTask_user(void* argument)
{
    for (;;)
    {
        osDelay(500);
    }
}
void StartStatusRedPWM_user(void *argument)
{
    /* Infinite loop */
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOC, OK_LED_Pin);

        osDelay(500);
    }

}


void StartStatusFlash_user(void *argument)
{
    // uint8_t txByte[1];
    uint8_t rxBuff[3];
    uint8_t txWriteBalCmd[1] = {testAddr | writeBalCmd | parityZero};
    //Charge 1 and 3 | Discharge (sync) 6
    //11, 00, 11, 00, 00, 10
    //Hex value of 0xCC20
    //CRC int is 10011 -> CRC bits of 0x7(0b0111) !Inverted!
    //uint8_t txBalDataBitmapBinary[2] = {0b11001100, 0b00101000};
    uint8_t txBalDataBitmapHex[2] = {0xCC, 0x28};

    uint8_t txReadbackBalCmd[1] = {testAddr | readbackBalCmd | parityOne};


    uint8_t testRxCMD[3] = {txReadbackBalCmd[0], 0xFC, 0xFC};

    //Define the CRC LUT
    uint8_t balCRCLUT[256];
    LTC33002::genCRC4LUT(balCRCLUT, CRC4LTC);
    HAL_GPIO_WritePin(GPIOC, nBalCS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, nBalCS_Pin, GPIO_PIN_SET);

#define testMSG {0xC, 0xC2}
    uint8_t const tester[2] = testMSG;
    uint8_t volatile CRCTest = LTC33002::calcCRC4Remainder(tester, 2, balCRCLUT);

    uint8_t testCmdGen[2];
    bool volatile success = LTC33002::generateBalCmd(testCmdGen, balCRCLUT, 0b00000101, 0b00100000);

    uint8_t testTx[3] = {txWriteBalCmd[0], testCmdGen[0], testCmdGen[1]};




    /* Infinite loop */
    for(;;)
    {

        //Transmit command
        HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi3, testTx, 3, 1000);
        HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);
        //
        // //osDelay(500);
        HAL_Delay(1);
        // //Recieve Data
        //
        HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_RESET);

        HAL_SPI_TransmitReceive(&hspi3, testRxCMD, rxBuff, 3, 1000);
        while (HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_BUSY);
        //osDelay(2000);
        HAL_GPIO_WritePin(nBalCS_GPIO_Port, nBalCS_Pin, GPIO_PIN_SET);
        osDelay(500);
    }
}