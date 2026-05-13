#include "user_threads.hpp"

#include <cstdint>

#include <cmsis_os2.h>
#include <stm32u5xx_hal.h>

#include "can.hpp"
#include "frontvcu_state.hpp"
#include "ina226.hpp"
#include "main.h"

extern ADC_HandleTypeDef hadc1;
uint16_t adc_buffer[frontvcu::FrontVCUState::ADC_BUF_LEN];
extern I2C_HandleTypeDef hi2c2;

INA226 fh_cs(&hi2c2, 0x41);
INA226 lights_cs(&hi2c2, 0x40);

void init_user()
{
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

    HAL_GPIO_WritePin(FAN_CTRL_GPIO_Port, FAN_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(HORN_CTRL_GPIO_Port, HORN_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FL_LIGHT_CTRL_GPIO_Port, FL_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FR_LIGHT_CTRL_GPIO_Port, FR_LIGHT_CTRL_Pin, GPIO_PIN_RESET);

    memset(adc_buffer, 0, sizeof(adc_buffer));

    // start throttle adc and dma (continuous mode)
    if (HAL_ADC_Start_DMA(&hadc1,
                          reinterpret_cast<uint32_t*>(adc_buffer),
                          frontvcu::FrontVCUState::ADC_BUF_LEN) != HAL_OK)
    {
        Error_Handler();
    }

    frontvcu::can_init();
}

void StartHeartbeat_user(void* argument)
{
    for (;;)
    {
        HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
        osDelay(500);
    }
}

// full transfer
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        uint32_t total{};
        for (size_t i = frontvcu::FrontVCUState::ADC_BUF_LEN / 2;
             i < frontvcu::FrontVCUState::ADC_BUF_LEN;
             ++i)
        {
            total += adc_buffer[i];
        }
        frontvcu::state.throttle_data.store(total / (frontvcu::FrontVCUState::ADC_BUF_LEN / 2));
    }
}

// half transfer
extern "C" void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        uint32_t total{};
        for (size_t i{}; i < frontvcu::FrontVCUState::ADC_BUF_LEN / 2; ++i)
        {
            total += adc_buffer[i];
        }
        frontvcu::state.throttle_data.store(total / (frontvcu::FrontVCUState::ADC_BUF_LEN / 2));
    }
}

void StartCANMessagesTX_user(void* argument)
{
    sg::CANFrame tb_frame = {.can_id = 0x080,
                             .id_type = sg::CANFrameIDType::STANDARD,
                             .rtr_mode = sg::CANFrameRTRMode::DATA,
                             .len = sg::CANFrameLen::BYTES_8,
                             .timestamp = {},
                             .data = {}};

    for (;;)
    {
        uint16_t throttle_data = frontvcu::state.throttle_data.load(std::memory_order_relaxed);
        tb_frame.data[0] = static_cast<uint8_t>(throttle_data);
        tb_frame.data[1] = static_cast<uint8_t>(throttle_data >> 8);
        tb_frame.data[2] = frontvcu::state.fh_power_lsb.load();
        tb_frame.data[3] = frontvcu::state.fh_power_msb.load();
        tb_frame.data[4] = frontvcu::state.lights_power_lsb.load();
        tb_frame.data[5] = frontvcu::state.lights_power_msb.load();
        tb_frame.data[7] = static_cast<uint8_t>(HAL_GPIO_ReadPin(BRAKE_GPIO_Port, BRAKE_Pin));
        frontvcu::can_device.send(tb_frame);
        osDelay(20);
    }
}

void StartLoadsControl_user(void* argument)
{
    uint8_t blinkCounter = 0;
    for (;;)
    {
        //driver fan and horn (driver fan is always on for now)
        HAL_GPIO_WritePin(FAN_CTRL_GPIO_Port, FAN_CTRL_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(HORN_CTRL_GPIO_Port,
                          HORN_CTRL_Pin,
                          static_cast<GPIO_PinState>(frontvcu::state.horn_state.load()));

        // reset blink counter if we enter hazards / turn signal condition from some other state
        if ((frontvcu::state.lights_req[0] == frontvcu::LIGHTS_STATES::HAZARDS ||
             frontvcu::state.lights_req[1] == frontvcu::LIGHTS_STATES::HAZARDS) &&
            ((frontvcu::state.lights_req[1] != frontvcu::state.lights[1]) ||
             (frontvcu::state.lights_req[0] != frontvcu::state.lights[0])))
        {
            blinkCounter = 0;
        }
        else if ((frontvcu::state.lights_req[0] == frontvcu::LIGHTS_STATES::TURN ||
                  frontvcu::state.lights_req[1] == frontvcu::LIGHTS_STATES::TURN) &&
                 (frontvcu::state.lights[0] != frontvcu::state.lights_req[0] ||
                  frontvcu::state.lights[1] != frontvcu::state.lights_req[1]))
        {
            blinkCounter = 0;
        }

        frontvcu::state.lights[0].store(frontvcu::state.lights_req[0].load());
        frontvcu::state.lights[1].store(frontvcu::state.lights_req[1].load());

        if (blinkCounter % 25 == 0)
        {
            blinkCounter = blinkCounter % 25;
            if (frontvcu::state.lights[0] == frontvcu::LIGHTS_STATES::TURN ||
                frontvcu::state.lights[0] == frontvcu::LIGHTS_STATES::HAZARDS)
            {
                HAL_GPIO_TogglePin(FL_LIGHT_CTRL_GPIO_Port, FL_LIGHT_CTRL_Pin);
            }
            if (frontvcu::state.lights[1] == frontvcu::LIGHTS_STATES::TURN ||
                frontvcu::state.lights[1] == frontvcu::LIGHTS_STATES::HAZARDS)
            {
                HAL_GPIO_TogglePin(FR_LIGHT_CTRL_GPIO_Port, FR_LIGHT_CTRL_Pin);
            }
        }

        if (frontvcu::state.lights[0].load() == frontvcu::LIGHTS_STATES::OFF)
        {
            blinkCounter = 0;
            HAL_GPIO_WritePin(FL_LIGHT_CTRL_GPIO_Port, FL_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
        }

        if (frontvcu::state.lights[1].load() == frontvcu::LIGHTS_STATES::OFF)
        {
            blinkCounter = 0;
            HAL_GPIO_WritePin(FR_LIGHT_CTRL_GPIO_Port, FR_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
        }

        //blink counter always increments, but it only has effect if it reaches 25 and one or more of the lights are in hazards/turn mode
        blinkCounter++;
        osDelay(20);
    }
}

void StartCurrentSense_user(void* argument)
{
    fh_cs.init(0.002, 2, 0x4123);
    lights_cs.init(0.002, 2, 0x4123);
    INA226::Measurement fh_cs_m{};
    INA226::Measurement lights_cs_m{};

    struct CS_DATA
    {
        uint16_t power;
        uint16_t current;
        uint8_t power_msb;
        uint8_t power_lsb;
        uint8_t current_msb;
        uint8_t current_lsb;
    } fh{}, lights{};

    for (;;)
    {
        fh_cs.writeConfig(0x4123);
        osDelay(5);
        lights_cs.writeConfig(0x4123);
        osDelay(5);

        // measurement gives floating point values, not sure if these are used
        fh_cs.readMeasurement(fh_cs_m);
        lights_cs.readMeasurement(lights_cs_m);

        fh_cs.readReg16(INA226::REG_POWER, fh.power);
        fh_cs.readReg16(INA226::REG_CURRENT, fh.current);
        lights_cs.readReg16(INA226::REG_POWER, lights.power);
        lights_cs.readReg16(INA226::REG_CURRENT, lights.current);

        fh.power_lsb = static_cast<uint8_t>(fh.power);
        fh.power_msb = static_cast<uint8_t>(fh.power >> 8);
        fh.current_lsb = static_cast<uint8_t>(fh.current);
        fh.current_msb = static_cast<uint8_t>(fh.current >> 8);

        lights.power_lsb = static_cast<uint8_t>(lights.power);
        lights.power_msb = static_cast<uint8_t>(lights.power >> 8);
        lights.current_lsb = static_cast<uint8_t>(lights.current);
        lights.current_msb = static_cast<uint8_t>(lights.current >> 8);

        frontvcu::state.lights_power_msb.store(lights.power_msb);
        frontvcu::state.lights_power_lsb.store(lights.power_lsb);
        frontvcu::state.fh_power_msb.store(fh.power_msb);
        frontvcu::state.fh_power_lsb.store(fh.power_lsb);

        osDelay(40);
    }
}
