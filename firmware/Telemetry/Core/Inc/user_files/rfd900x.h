//Header file for the RDF900x RF Module for Telemetry Board

//includes
#include <stdint.h>
#include "stm32f4xx_hal.h"

//UART specifications

/*
57600 Baud Rate
No parity bit
8 Data bits
1 stop bit
RTS/CTS flow control OFF
*/

//N-F114RE details
/*
 Using USART1

 Pins:
 PA9 -> TX, OUT (Green)
 PA10 -> RX, IN (Blue)
 PC9 -> RTS, OUT (Orange)
 PC8 -> CTS, IN (Purple)

*/

//General Functions
HAL_StatusTypeDef enterLocalATCommandMode();
HAL_StatusTypeDef exitLocalATCommandMode();
HAL_StatusTypeDef sendData(uint8_t *databuffer, uint16_t sizeData);
HAL_StatusTypeDef setRegister(uint8_t reg_num, uint16_t reg_val );
HAL_StatusTypeDef saveLocalRegisterValues();
HAL_StatusTypeDef resetBootMode();

//Configuration Functions
HAL_StatusTypeDef defaultConfig();
HAL_StatusTypeDef resetLocalParameters();
HAL_StatusTypeDef rebootLocalRadio();
HAL_StatusTypeDef getLocalFirmwareData(uint8_t ATI_val);
HAL_StatusTypeDef setLocalParameter(uint8_t reg_num, uint16_t reg_val );
HAL_StatusTypeDef getLocalRegisterValue(uint8_t reg_num);

