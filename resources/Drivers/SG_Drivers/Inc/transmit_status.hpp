#pragma once

#include <cstdint>

namespace sg
{

enum class Status : uint8_t
{
    Ok,
    Error,
    Busy,
    Timeout,
    InvalidParam
};

}