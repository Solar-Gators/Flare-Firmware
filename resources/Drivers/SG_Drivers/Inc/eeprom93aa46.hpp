#pragma once

#include "eeprom.hpp"
#include "spi_api.hpp"

#include <array>

namespace sg
{

class Eeprom93AA46 final : public Eeprom
{
   public:
    /**
     *
     * @param hspi Stm32Hal Spi Handle
     * @param cs_port Stm32Hal Port that holds the ss for this target
     * @param cs_pin Stm32Hal Pin that holds teh ss for this target
     * @param al ActivationLevel Enum denoting the activation level for the devices ss. ActiveLow means low ss activates the device
     */
    Eeprom93AA46(SPI_HandleTypeDef* hspi,
                 GPIO_TypeDef* cs_port,
                 uint16_t cs_pin,
                 ActivationLevel al)
        : spi_(hspi, cs_port, cs_pin, al)
    {
    }

    /**
     *
     * @param addr Address inside the eeprom to read from
     * @param buf Pointer to byte buffer that data at addr will be placed in
     * @param len The number of bytes read
     * @return HAL status denoting sucess of transmit. HAL_OK on success
     */
    HAL_StatusTypeDef read(uint32_t addr, uint8_t* buf, size_t len) override;

    /**
     *
     * @param addr Address inside the eeprom to write to
     * @param buf Pointer to const byte buffer where data resides that will be written
     * @param len The number of bytes to write
     * @return HAL status denoting sucess of transmit. HAL_OK on success
     */
    HAL_StatusTypeDef write(uint32_t addr, const uint8_t* buf, size_t len) override;

    /**
     *
     * @return The number of pages in the eeprom
     */
    uint32_t size() const override { return 128; }

    /**
     *
     * @return The size of a single page, in bytes
     */
    uint16_t pageSize() const override { return 1; }

    /**
     *
     * @return The number of bytes that can be read/written in one command
     */
    uint16_t programGranularity() const override { return 1; }

   private:
    SpiDevice spi_;

    // EWEN  -> 1 00 1 1 X X X X X
    static inline constexpr std::array<uint8_t, 2> kEwen = {0b00000010, 0b01100000};
    // EWDS  -> 1 00 0 0 XXXXX
    static inline constexpr std::array<uint8_t, 2> kEwds = {0b00000010, 0b00000000};

    // start bit and opcode for read, length is byte length of instruction
    static inline constexpr uint32_t kRdMask = (0b110 << 7);
    static inline constexpr size_t kRLen = 2;

    // start bit and opcode for write, length in byte length of instruction
    static inline constexpr uint32_t kWrMask = (0b101 << 15);
    static inline constexpr size_t kWLen = 3;

    // address is 7 bits
    static inline constexpr uint8_t kAddrMask = 0b01111111;

    // helper functions that send one of the commands to the eeprom
    HAL_StatusTypeDef sendRead(uint32_t addr, uint8_t& out);
    HAL_StatusTypeDef sendWrite(uint32_t addr, const uint8_t& byte);
    HAL_StatusTypeDef sendEWEN();
    HAL_StatusTypeDef sendEWDS();

    // internal helpers
    HAL_StatusTypeDef sendWriteFromBitInstruction(uint32_t instr);
    HAL_StatusTypeDef sendReadFromBitInstruction(uint32_t instr, uint8_t& out);
};

}  // namespace sg