#include "../Include/NRF24L01P.hpp"

HAL_StatusTypeDef NRF24L01P::readRegister(uint8_t reg, uint8_t *data, uint8_t data_len)
{
    const uint8_t cmd = 0x00 | (reg & 0x1F);
    return writeThenRead(&cmd, 1, data, data_len);
}

HAL_StatusTypeDef NRF24L01P::writeSingle(uint8_t reg, const uint8_t data)
{
    uint8_t buf[2] = {static_cast<uint8_t>(0x20 | (reg & 0x1F)), data};
    return write(buf, 1);
}

HAL_StatusTypeDef NRF24L01P::writeMultiple(uint8_t reg, const uint8_t *data, uint8_t data_len)
{
    uint8_t *buf = new uint8_t[data_len + 1];
    buf[0] = reg;
    memcpy(buf + 1, data, data_len);

    return write(buf, data_len + 1);
}

HAL_StatusTypeDef NRF24L01P::init()
{
    // Power off / 1 bit CRC
    TRY(writeSingle(0x00, 0x00));

    // Address width = 5 bytes
    TRY(writeSingle(0x03, 0x03));

    // Auto retransmit: 750us delay (0101), 15 retries (1111) => 0x5F (example 0x2F is fine too)
    TRY(writeSingle(0x04, 0x5F));

    // Channel 90
    TRY(writeSingle(0x05, 90));

    // RF: 2Mbps (RF_DR_HIGH=1), 0dBm (RF_PWR=11) -> 0x0E
    TRY(writeSingle(0x06, 0x0E));

    // Enable Auto-ACK on pipe 0
    TRY(writeSingle(0x01, 0x01));

    // Enable only pipe 0
    TRY(writeSingle(0x02, 0x01));

    TRY(writeMultiple(0x10, ADDR, sizeof(ADDR)));

    return HAL_OK;
}