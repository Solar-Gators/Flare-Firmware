#pragma once

#include <cstring>

#include "spi_api.h"

#define TRY(x)                \
    do                        \
    {                         \
        if ((x) != HAL_OK)    \
            return HAL_ERROR; \
    } while (0)

static const uint8_t ADDR[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};  // LSByte first
class NRF24L01P : public SpiDevice
{
   public:
    NRF24L01P(SPI_HandleTypeDef* hspi,
              GPIO_TypeDef* cs_port,
              uint16_t cs_pin,
              GPIO_TypeDef* ce_port,
              uint16_t ce_pin)
        : SpiDevice(hspi, cs_port, cs_pin), ce_port_(ce_port), ce_pin_(ce_pin)
    {
    }

    HAL_StatusTypeDef readRegister(uint8_t reg, uint8_t* data, uint8_t data_len);
    HAL_StatusTypeDef writeSingle(uint8_t reg, const uint8_t data);
    HAL_StatusTypeDef writeMultiple(uint8_t reg, const uint8_t* data, uint8_t data_len);

    HAL_StatusTypeDef init();

   private:
    const GPIO_TypeDef* ce_port_;
    const uint16_t ce_pin_;
};

/* Thanks to Chat for formatting

                           +--------------------+
                           |     POWER DOWN     |
                           |  CONFIG.PWR_UP=0   |
                           +----------+---------+
                                      |
                        write CONFIG.PWR_UP=1
                                      v
                           +--------------------+
                           |     STANDBY-I      |
                           |  CE=0, PWR_UP=1    |
                           +----+-----------+---+
                                |           |
                      PRIM_RX=1 |           | PRIM_RX=0
                                v           v
                      +----------------+   +----------------+
                      |   PRX (RX IDLE)|   |   PTX (TX IDLE)|
                      | CE=1, listen   |   | CE=0           |
                      +-------+--------+   +--------+-------+
                              |                         |
                IRQ: RX_DR=1  |                         |  load payload:
                              v                         |  W_TX_PAYLOAD
                    +--------------------+             |
                    |  RX: PACKET READY  |             |
                    |  (RX FIFO not empty)            |
                    +--------------------+             |
                              |                        |
                 R_RX_PAYLOAD, clear RX_DR            |
                              |                        |
             more data?  yes -+                        |
                              | no                     v
                              |                +------------------+
                              +--------------> |    TX TRIGGER    |
                                               | CE pulse ≥10 µs  |
                                               +---------+--------+
                                                         |
                                     auto-retry/ACK window occurs here
                                                         |
                                           +-------------+-------------+
                                           |                           |
                                  IRQ: TX_DS=1                 IRQ: MAX_RT=1
                                  (success)                    (retries exhausted)
                                           |                           |
                                           v                           v
                                  +------------------+        +------------------+
                                  |  TX SUCCESS      |        |   TX FAILED      |
                                  | clear TX_DS      |        | clear MAX_RT     |
                                  +---------+--------+        +--------+---------+
                                            \                       /
                                             \  (optional FLUSH_TX)/
                                              \                   /
                                               +-------+---------+
                                                       |
                                        go back to receive (typical)
                                                       |
                                                set PRIM_RX=1
                                                       v
                                              +----------------+
                                              |  PRX (RX IDLE) |
                                              |  CE=1          |
                                              +----------------+

 */