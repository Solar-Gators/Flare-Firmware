//
// Created by justin on 3/11/26.
//

#ifndef FLAREFIRMWARE_REARVCU_H
#define FLAREFIRMWARE_REARVCU_H

namespace rearvcu
{

void init();
void sendSuppBattFrame();
void processRegenThrottleOutputs();
void processMCOutputs();
void processArrayContactors();
void sendStatusMessage();
void startSupBatMonitoring ();

}  // namespace rearvcu

#endif  //FLAREFIRMWARE_REARVCU_H
