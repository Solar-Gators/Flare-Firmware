#include "../Include/BQ7692000.h"

#include <cstdint>

#define TRY(x)                \
    do                        \
    {                         \
        if ((x) != HAL_OK)    \
            return HAL_ERROR; \
    } while (0)

HAL_StatusTypeDef BQ7692000PW::getCC(uint16_t *data)
{
    TRY(checkCC());

    *data = (dataCC_[0] << 8) | dataCC_[1];
    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::getVC(std::vector<uint16_t> &vc_values)
{
    TRY(checkVC());

    vc_values.reserve(dataVC_.size() / 2);

    for (size_t i = 0; i < dataVC_.size(); i += 2)
    {
        vc_values.push_back((uint16_t(dataVC_[i]) << 8) | dataVC_[i + 1]);
    }

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::checkStatus()
{
    TRY(readN(static_cast<uint8_t>(registers::status), &status_, 1));

    if ((status_ & STATUS_ERR_MASK) != 0)
    {
        // TODO: log/handle error bits
    }

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::checkVC()
{
    for (int i = 0; i < 5; i++)
    {
        TRY(readN(static_cast<uint8_t>(registers::vc1_hi) + static_cast<uint8_t>(i * TWO_BYTES),
                  dataVC_.data() + (i * TWO_BYTES),
                  TWO_BYTES));
    }

    return HAL_OK;
}

HAL_StatusTypeDef BQ7692000PW::checkCC()
{
    TRY(checkStatus());

    if (!(status_ & STATUS_CC_READY_MASK))
    {
        return HAL_BUSY;  // Maybe HAL_ERROR?
    }
    return readN(static_cast<uint8_t>(registers::cc_hi), dataCC_, static_cast<size_t>(TWO_BYTES));
}