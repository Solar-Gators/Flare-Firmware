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
 PA9 -> TX, OUT
 PA10 -> RX, IN
 PC9 -> RTS, OUT
 PC8 -> CTS, IN

*/

HAL_StatusTypeDef enterATCommandMode();
HAL_StatusTypeDef sendData(uint8_t *databuffer);

HAL_StatusTypeDef defaultConfig();
