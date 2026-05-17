
#ifndef FLAREFIRMWARE_DRIVER_LTC3300_2_H
#define FLAREFIRMWARE_DRIVER_LTC3300_2_H
#include <cstdint>

#ifdef __cplusplus
extern "C"
{
#endif

//CRC constants
#define CRC4LTC 0b10011

//LTC3300-2 command constants
//Command formatted as A4, A3, A2, A1, A0 (All 0s for this board)
#define testAddr (0b00000 << 3)
//Commands
#define readBalStatus (0b10 << 1)
#define writeBalCmd (0b00 << 1)
#define readbackBalCmd (0b01 << 1)
//Even parity bit
#define parityOne 0b1
#define parityZero 0b0

    namespace LTC33002
    {

    /**
     * @brief Generates a LUT holding the CRC4 remainder values based on the passed generator ploynomial.
     * Helps reduce compute time.
     * @param LUT A pass to the LUT that will store the values.
     * @param polynomial The required CRC-4 polynomial for the CRC system. This should be a 5-bit value.
     */
    void genCRC4LUT(uint8_t LUT[256], uint8_t polynomial = CRC4LTC);


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

    }

#ifdef __cplusplus
}
#endif

#endif  //FLAREFIRMWARE_DRIVER_LTC3300_2_H
