//Header file for the RDF900x RF Module for Telemetry Board

//includes
#include <stdint.h>

#include "main.h"

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

#ifdef __cplusplus
extern "C"
{
#endif

    // important init
    void rfd900SetUartHandle(UART_HandleTypeDef* user_uart);
    HAL_StatusTypeDef rfd900Read(uint8_t* buf,
                                 uint16_t maxLen,
                                 uint16_t* outLen,
                                 uint32_t timeout_ms);

    //General Functions
    HAL_StatusTypeDef rfd900EnterLocalATCommandMode();
    HAL_StatusTypeDef rfd900ExitLocalATCommandMode();
    HAL_StatusTypeDef rfd900SendData(uint8_t* databuffer, uint16_t sizeData);
    HAL_StatusTypeDef rfd900SetRegister(uint8_t reg_num, uint16_t reg_val);
    HAL_StatusTypeDef rfd900SaveLocalRegisterValues();
    HAL_StatusTypeDef rfd900ResetBootMode();

    //Configuration Functions
    HAL_StatusTypeDef rfd900DefaultConfig();
    HAL_StatusTypeDef rfd900ResetLocalParameters();
    HAL_StatusTypeDef rfd900RebootLocalRadio();
    HAL_StatusTypeDef rfd900GetLocalFirmwareData(uint8_t ATI_val);
    HAL_StatusTypeDef rfd900SetLocalParameter(uint8_t reg_num, uint16_t reg_val);
    HAL_StatusTypeDef rfd900GetLocalRegisterValue(uint8_t reg_num);

#ifdef __cplusplus
}
#endif
