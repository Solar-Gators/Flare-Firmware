//
// Created by justin on 3/11/26.
//

#ifndef FLAREFIRMWARE_REARVCU_H
#define FLAREFIRMWARE_REARVCU_H

namespace rearvcu
{

constexpr uint32_t throttle_output_loop_rate_ms =
    10;  // it should always be this so we don't have to pass in delta time for pid loop

void init();
void sendSuppBattFrame();
void processRegenThrottleOutputs();
void processMCOutputs();
void processArrayContactors();
void sendStatusMessage();
void startSupBatMonitoring();

}  // namespace rearvcu

#endif  //FLAREFIRMWARE_REARVCU_H
