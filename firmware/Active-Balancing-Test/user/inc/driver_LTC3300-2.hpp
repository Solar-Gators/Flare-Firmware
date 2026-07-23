
#ifndef FLAREFIRMWARE_DRIVER_LTC3300_2_H
#define FLAREFIRMWARE_DRIVER_LTC3300_2_H
#include <cstdint>

#include "stm32u5xx_hal_spi.h"
#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

//CRC constants
#define CRC4LTCBM 0b10011

//LTC3300-2 command constants
//Command formatted as A4, A3, A2, A1, A0 (All 0s for test board)
#define testAddrBM (0b00000 << 3)
//Commands
#define readBalStatusBM (0b10 << 1)
#define writeBalCmdBM (0b00 << 1)
#define readbackBalCmdBM (0b01 << 1)
//Even parity bit
#define parityOneBM 0b1
#define parityZeroBM 0b0

    namespace LTC33002Driver
    {

    /**
     * @brief Generates a LUT holding the CRC4 remainder values based on the passed generator ploynomial.
     * Helps reduce compute time.
     * @param LUT A pass to the LUT that will store the values.
     * @param polynomial The required CRC-4 polynomial for the CRC system. This should be a 5-bit value.
     */
    void genCRC4LUT(uint8_t LUT[256], uint8_t polynomial);


    /**
    * @brief Generates the 4 bit CRC remainder value. By default, it uses a CRC-4 characteristic polynomial.
    * This function only supports generating CRCs of the 4th degree but the exact polynomial can be changed
    * @param message The given message to calculate the CRC remainder. This should be a 2 byte value.
    * @param msgSize The size of the input message in bytes.
    * @param CRC4LUT Pass the generated LUT from genCRC4LUT() as a calculation shortcut.
    */
    uint8_t calcCRC4Remainder(uint8_t const message[], uint8_t msgSize, uint8_t const CRC4LUT[256]);

    /** @brief Generates the 16-bit value to be sent to the LTC3300-2 for a balance command.
     *  All BM are organized so the nth bit of the value corresponds to the n+1 cell in the stack, starting at 1. (ex: 0bxx100101 would affect cells 1, 3, and 6).
     *  @param cmdArray The 2-byte array that will store the generated balance command.
     *  @param CRC4LUT Pass the generated LUT from genCRC4LUT() as a calculation shortcut.
     *  @param chargeBM Bitmask for the cells to charge up.
     *  @param dischargeSyncBM Bitmask for the cells to discharge with synchronous secondary rectification. Use this for high efficiency rectification when FETs are used on both the primary and secondary side.
     *  @param dischargeNonBM Bitmask for the cells to discharge non-synchronously. This disables the secondary side synchronous rectification, using either the body diode of the FET or a physical diode if designed around.
     */
    bool generateBalCmd(uint8_t cmdArray[2], uint8_t const CRC4LUT[256], uint8_t chargeBM, uint8_t dischargeSyncBM, uint8_t dischargeNonBM= 0);


    class LTC3300_2
    {

    private:
        //Hardware properties
        uint8_t Address;
        uint8_t CRCPolynomial;
        SPI_HandleTypeDef* Balhspi;
        GPIO_TypeDef* CSPinGPIOx;
        uint16_t CSPin;

        //Data Handling
        uint8_t CRCLUT[256];
        uint8_t TxBuff[3];
        uint8_t RxBuff[3];
        uint8_t executeTx[1];

    public:
        /** @brief Constructor for a LTC3300-2 device
         *  @param deviceAddress The physical device address for a given IC. Set by dedicated hardware pins. (Define as 0xXXAAAAA where A is an address bit)
         *  @param CRC4Poly The 5 bit CRC-4 polynomial for the device. (Define as 0bXXPPPPP where P are the values for the polynomial)
         *  @param hspi The SPI handler for the given configuration
         *  @param CSPinGPIOx The GPIO port for the device CS
         *  @param CSPin The GPIO pin for the device CS
         */
        LTC3300_2(uint8_t deviceAddress, uint8_t CRC4Poly, SPI_HandleTypeDef* hspi, GPIO_TypeDef* CSPinGPIOx, uint16_t CSPin);

        /** @brief Writes the balancing command to the LTC.
        *  All BM are organized so the nth bit of the value corresponds to the n+1 cell in the stack, starting at 1. (ex: 0bxx100101 would affect cells 1, 3, and 6).
        *  @return Returns a boolean noting if the command was successfully written.
        *  @param chargeBM Bitmask for the cells to charge up.
        *  @param dischargeSyncBM Bitmask for the cells to discharge with synchronous secondary rectification. Use this for high efficiency rectification when FETs are used on both the primary and secondary side.
        *  @param dischargeNonBM Bitmask for the cells to discharge non-synchronously. This disables the secondary side synchronous rectification, using either the body diode of the FET or a physical diode if hardware permits.
        */
        bool writeBalCommand(uint8_t chargeBM, uint8_t dischargeSyncBM, uint8_t dischargeNonBM=0);

        /** @brief Sends
         *  @return
         */
        bool readbackBalCmd();

        /** @brief Getter function to see the value stored in the Rx Buffer of the object.
         *  @return Returns a pointer to the first byte of the Rx buffer. This value is always a 3 byte sequence where [0] is junk data, [1] and [2] refer to the returned data.
         */
        uint8_t* readRxBuffer();







    };

    }

#ifdef __cplusplus
}
#endif

#endif  //FLAREFIRMWARE_DRIVER_LTC3300_2_H
