#ifndef FLAREFIRMWARE_FRONTVCU_H
#define FLAREFIRMWARE_FRONTVCU_H

namespace frontvcu
{

void init();
void sendCANMessagesTX();
void writeLeft(bool on);
void writeRight(bool on);
void writeLoadsControl();
void readCurrentSense();

}  // namespace frontvcu

#endif
