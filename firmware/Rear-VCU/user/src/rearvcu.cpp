//
// Created by justin on 3/11/26.
//

#include "can.hpp"
#include "can_protocol.h"
#include "main.h"
#include "rearvcu_state.h"

#include <atomic>

extern ADC_HandleTypeDef hadc1;  // supp batt
extern DAC_HandleTypeDef hdac1;

namespace
{
constexpr uint32_t array_precharge_hold_time_ms = 500;

// dacs
DAC_HandleTypeDef& dac = hdac1;
constexpr uint32_t dac_channel_throttle = DAC_CHANNEL_1;
constexpr uint32_t dac_channel_regen = DAC_CHANNEL_2;
}  // namespace

namespace rearvcu
{

void init()
{
    // assert atomics work
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

    // array contactors should start open
    HAL_GPIO_WritePin(PRE_ARRAY_CTRL_GPIO_Port, PRE_ARRAY_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MAIN_ARRAY_CTRL_GPIO_Port, MAIN_ARRAY_CTRL_Pin, GPIO_PIN_RESET);

    // turn on mc
    HAL_GPIO_WritePin(MC_MAIN_CTRL_GPIO_Port, MC_MAIN_CTRL_Pin, GPIO_PIN_SET);

    // init the regen and throttle dacs
    HAL_DAC_Start(&dac, dac_channel_regen);
    HAL_DAC_Start(&dac, dac_channel_throttle);

    // TODO: initialize ina chip here

    // can
    can_init();
}

void sendSuppBattFrame()
{
    sg::CANFrame supp_batt_frame{0x021,
                                 sg::CANFrameIDType::STANDARD,
                                 sg::CANFrameRTRMode::DATA,
                                 sg::CANFrameLen::BYTES_4,
                                 0,
                                 {}};

    // supp batt voltage can be read and sent in this thread as its not as urgent/important
    uint16_t supp_batt_voltage_mv = 0xFFFF;  // TODO: could get voltage of supp batt here
    uint16_t supp_batt_current = 0xFFFF;     // TODO: could get current draw of supp batt here

    supp_batt_frame.data[0] = static_cast<uint8_t>(supp_batt_voltage_mv);       // lsb
    supp_batt_frame.data[1] = static_cast<uint8_t>(supp_batt_voltage_mv >> 8);  // msb
    supp_batt_frame.data[2] = static_cast<uint8_t>(supp_batt_current);          // lsb
    supp_batt_frame.data[3] = static_cast<uint8_t>(supp_batt_current >> 8);     // msb
    can_device.send(supp_batt_frame);
}

void processRegenThrottleOutputs()
{
    // TODO: Use PID loop and add cruise control after verifying this works on car

    uint16_t throttle_vol = state.throttle_requested.load(std::memory_order_relaxed);

    uint8_t regen_percent = state.regen_percent_requested.load(std::memory_order_relaxed);
    auto regen_vol =
        static_cast<uint16_t>((static_cast<float>(regen_percent) / 100.0f) * 0xFFF);  // 12 bit dac

    // if physical brake is pressed, set throttle to 0 and regen to max
    if (state.brake_pressed.load(std::memory_order_relaxed))
    {
        HAL_DAC_SetValue(&dac, dac_channel_throttle, DAC_ALIGN_12B_R, 0);
        HAL_DAC_SetValue(&dac, dac_channel_regen, DAC_ALIGN_12B_R, 0xFFF);
        return;
    }

    // if throttle is 0, turn on regen at driver's requested strength
    if (throttle_vol <
        10)  // less than 10 here in case of transcience or something in front vcus adc?
    {
        HAL_DAC_SetValue(&dac, dac_channel_throttle, DAC_ALIGN_12B_R, 0);
        HAL_DAC_SetValue(&dac, dac_channel_regen, DAC_ALIGN_12B_R, regen_vol);
        return;
    }

    // otherwise actually use the throttle requested by driver
    HAL_DAC_SetValue(&dac, dac_channel_regen, DAC_ALIGN_12B_R, 0);
    HAL_DAC_SetValue(&dac, dac_channel_throttle, DAC_ALIGN_12B_R, throttle_vol);
}

void processMCOutputs()
{
    // TODO: can make these writes less frequent using flag, only call writepin on change yk

    // power/eco
    flare_can::MCPowerMode mc_power_mode_requested = state.mc_power_mode_requested.load();
    HAL_GPIO_WritePin(MC_PWR_ECO_CTRL_GPIO_Port,
                      MC_PWR_ECO_CTRL_Pin,
                      static_cast<GPIO_PinState>(mc_power_mode_requested));

    // direction pin
    flare_can::Direction direction_requested = state.direction_requested.load();
    HAL_GPIO_WritePin(MC_FWD_REV_CTRL_GPIO_Port,
                      MC_FWD_REV_CTRL_Pin,
                      static_cast<GPIO_PinState>(direction_requested));
}

void processArrayContactors()
{
    static uint32_t precharge_closed_timestamp{};

    // TODO: user timer peripheral for consistent timer logic
    auto array_contactors = state.array_contactors.load(std::memory_order_relaxed);
    if (state.array_contactors_requested_closed.load())
    {
        if (array_contactors == flare_can::ArrayContactors::BOTH_OPEN)
        {
            // close pre
            HAL_GPIO_WritePin(PRE_ARRAY_CTRL_GPIO_Port, PRE_ARRAY_CTRL_Pin, GPIO_PIN_SET);
            precharge_closed_timestamp = HAL_GetTick();
            state.array_contactors.store(flare_can::ArrayContactors::PRECHARGE_CLOSED,
                                         std::memory_order_relaxed);
        }
        else if (array_contactors == flare_can::ArrayContactors::PRECHARGE_CLOSED &&
                 HAL_GetTick() - precharge_closed_timestamp > array_precharge_hold_time_ms)
        {
            // close main
            HAL_GPIO_WritePin(MAIN_ARRAY_CTRL_GPIO_Port, MAIN_ARRAY_CTRL_Pin, GPIO_PIN_SET);
            state.array_contactors.store(flare_can::ArrayContactors::MAIN_CLOSED,
                                         std::memory_order_relaxed);
        }
    }
    else
    {
        // open both contactors
        HAL_GPIO_WritePin(PRE_ARRAY_CTRL_GPIO_Port, PRE_ARRAY_CTRL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MAIN_ARRAY_CTRL_GPIO_Port, MAIN_ARRAY_CTRL_Pin, GPIO_PIN_RESET);
        state.array_contactors.store(flare_can::ArrayContactors::BOTH_OPEN,
                                     std::memory_order_relaxed);
    }
}

void sendStatusMessage()
{
    sg::CANFrame rearvcu_statuses_frame{0x020,
                                        sg::CANFrameIDType::STANDARD,
                                        sg::CANFrameRTRMode::DATA,
                                        sg::CANFrameLen::BYTES_8,
                                        0,
                                        {}};
    // mc should always be high written to at startup
    rearvcu_statuses_frame.data[0] = 1;

    // setup and send diagnostic can message
    auto mc_power_mode_requested = state.mc_power_mode_requested.load();
    auto direction_requested = state.direction_requested.load();
    auto array_contactors = state.array_contactors.load();
    uint8_t car_speed = state.car_speed.load();

    rearvcu_statuses_frame.data[1] = static_cast<uint8_t>(direction_requested);
    rearvcu_statuses_frame.data[2] = static_cast<uint8_t>(mc_power_mode_requested);
    rearvcu_statuses_frame.data[3] = static_cast<uint8_t>(array_contactors);
    rearvcu_statuses_frame.data[4] = car_speed;

    can_device.send(rearvcu_statuses_frame);
}

}  // namespace rearvcu
