
#ifndef FRONTVCU_STATE_H
#define FRONTVCU_STATE_H

#include "can_protocol.h"

#include <atomic>
#include <cstdint>

namespace frontvcu
{

enum class LIGHTS_STATES : uint8_t
{
  OFF = 0,
  ON = 1,
  TURN = 2,
  HAZARDS = 3
};

struct FrontVCUState
{
  static constexpr uint8_t ADC_BUF_LEN  = 20;
  std::atomic<uint16_t> throttle_data{};
  std::atomic<uint8_t> brake_state{};
  std::atomic<LIGHTS_STATES>  lights_req[2];
  std::atomic<LIGHTS_STATES> lights[2];
  std::atomic<uint8_t> horn_state{};
  std::atomic<uint8_t> fan_state{};
};

inline FrontVCUState state;

}  // namespace frontvcu


#endif //FRONTVCU_STATE_H
