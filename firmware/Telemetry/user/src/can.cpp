#include "can.h"

#include "CanDriver.hpp"

void can_init()
{
    can_device.addCallbackId() can_device.StartCANDevice();
}

void steeringRequestsCallback() {}
