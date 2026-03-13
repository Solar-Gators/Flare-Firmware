#include "user_threads.hpp"
#include "frontvcu_state.hpp"
#include "can.hpp"

#include <cmsis_os2.h>
#include <main.h>
#include <stm32u5xx_hal.h>
#include <cstdint>


extern ADC_HandleTypeDef hadc1;
uint32_t adc_buffer[frontvcu::state.ADC_BUF_LEN];


void init_user()
{
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);
    static_assert(std::atomic<uint8_t>::is_always_lock_free);

    HAL_GPIO_WritePin(FAN_CTRL_GPIO_Port, FAN_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(HORN_CTRL_GPIO_Port, HORN_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FL_LIGHT_CTRL_GPIO_Port, FL_LIGHT_CTRL_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FR_LIGHT_CTRL_GPIO_Port, FR_LIGHT_CTRL_Pin, GPIO_PIN_RESET);


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

void StartThrottleBrakeRead_user(void* argument)
{
    for (;;)
    {
        if (HAL_ADC_Start_DMA(&hadc1, adc_buffer, frontvcu::state.ADC_BUF_LEN) != HAL_OK)
        {
            Error_Handler();
        }

        GPIO_PinState brake_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
        frontvcu::state.brake_state = brake_state;
    	osDelay(20);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1) {
        double adc_buf_avg = 0.0;
        for (int i = 0; i < frontvcu::state.ADC_BUF_LEN; i++)
        {
            adc_buf_avg += static_cast<double>(adc_buffer[i])/frontvcu::state.ADC_BUF_LEN;
        }
        const auto adc_buf_avg_int = static_cast<uint16_t>(adc_buf_avg);
        frontvcu::state.throttle_data.store(adc_buf_avg_int);

    }
}

void StartCANMessagesTX_user(void* argument)
{
     sg::CANFrame tb_frame = {
        .can_id = 0x080,
        .id_type = sg::CANFrameIDType::STANDARD,
        .rtr_mode = sg::CANFrameRTRMode::DATA,
        .len = sg::CANFrameLen::BYTES_8,
        .data = {0}
    };

    for(;;)
    {
        tb_frame.data[0] = static_cast<uint8_t>(frontvcu::state.throttle_data.load());
        tb_frame.data[1] = static_cast<uint8_t>(frontvcu::state.throttle_data.load() >> 8);
        tb_frame.data[7] = frontvcu::state.brake_state.load();
        frontvcu::can_device.send(tb_frame);
        osDelay(20);
    }
}

void StartLightsControl_user(void* argument)
{
    uint8_t blinkCounter = 0;
    for(;;)
    {
        //driver fan and horn
        HAL_GPIO_WritePin(FAN_CTRL_GPIO_Port, FAN_CTRL_Pin, static_cast<GPIO_PinState>(frontvcu::state.fan_state.load()));
        HAL_GPIO_WritePin(HORN_CTRL_GPIO_Port, HORN_CTRL_Pin, static_cast<GPIO_PinState>(frontvcu::state.horn_state.load()));

        // reset blink counter if we enter hazards / turn signal condition from some other state
        if ((frontvcu::state.lights_req[0] == frontvcu::LIGHTS_STATES::HAZARDS || frontvcu::state.lights_req[1] == frontvcu::LIGHTS_STATES::HAZARDS)\
            && ((frontvcu::state.lights_req[1] != frontvcu::state.lights[1]) || (frontvcu::state.lights_req[0] != frontvcu::state.lights[0])))
        {
            blinkCounter = 0;
        }
        else if ((frontvcu::state.lights_req[0] == frontvcu::LIGHTS_STATES::TURN || frontvcu::state.lights_req[1] == frontvcu::LIGHTS_STATES::TURN)\
            && (frontvcu::state.lights[0] != frontvcu::state.lights_req[0] || frontvcu::state.lights[1] != frontvcu::state.lights_req[1]))
        {
            blinkCounter = 0;
        }

        frontvcu::state.lights[0].store(frontvcu::state.lights_req[0].load());
        frontvcu::state.lights[1].store(frontvcu::state.lights_req[1].load());


        if (blinkCounter%25 == 0)
        {
            blinkCounter = blinkCounter%25;
            if (frontvcu::state.lights[0] == frontvcu::LIGHTS_STATES::TURN || frontvcu::state.lights[0] == frontvcu::LIGHTS_STATES::HAZARDS)
            {
                HAL_GPIO_TogglePin(FL_LIGHT_CTRL_GPIO_Port, FL_LIGHT_CTRL_Pin);
            }
            if (frontvcu::state.lights[1] == frontvcu::LIGHTS_STATES::TURN || frontvcu::state.lights[1] == frontvcu::LIGHTS_STATES::HAZARDS)
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


