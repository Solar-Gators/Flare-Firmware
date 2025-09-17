//Source file for the RDF900x RF Module for Telemetry Board

//INCLUDES
#include "rfd900x.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

//DEFINES
#define UART_TIMEOUT 10

/**
 * THE FUNCTIONS ARE WRITTEN IN THE ORDER THEY APPEAR IN THE RFD900x DATASHEET AT COMMANDS PAGE.
 */


/**
 * sendData - sends the data buffer over the TX pin using HAL USART
 *
 * @databuffer: pointer to the buffer of data that needs to be sent over USART
 *
 * @sizeData: size of the buffer being sent over USART
 *
 * @return -> HAL status of whether the transmission sent
 */

HAL_StatusTypeDef sendData(uint8_t *databuffer, uint16_t sizeData){

	HAL_StatusTypeDef status;

	status = HAL_UART_Transmit(&huart1, databuffer, sizeData, HAL_MAX_DELAY);
	HAL_Delay(1000);

	return status;
}

/**
 * defaultConfig - Function to automatically assign a config for one/both radios.
 *
 * @NO PARAMETERS
 *
 * @return -> HAL status of whether the transmission sent
 */
//standard config whatever we decide, test if RIGHT
HAL_StatusTypeDef defaultConfig(){
	uint8_t errors = 0;

	if(errors != 0){
		return HAL_ERROR;
	}
	else{
		return HAL_OK;
	}

}

/**
 * enterLocalATCommandMode - Used to enter command mode where the AT-adjacent commands can be used to configure/view the local and remote RF module parameters.
 *
 * @NO PARAMETERS
 *
 * @return -> HAL status of whether the transmission sent
 */

HAL_StatusTypeDef enterLocalATCommandMode(){

	uint8_t errors = 0;
	uint8_t data[] = {'+','+','+'};

	//Figure out how to use putty to send and receive data

	//send "+++" to enterATCommandMode, no quotes
	if(sendData(data, sizeof(data)) != HAL_OK){
		errors++;
	}

	if(defaultConfig != HAL_OK){
		errors++;
	}

	if(errors != 0){
		return HAL_ERROR;
	}
	return HAL_OK;
}


/**ATI
 *
 * getLocalFirmwareData - Get local data from local RF module using ATI Command, VISIT MANUAL
 *
 * @ATI_val - Trailing value of the ATI command being sent
 *
 *
 * @return -> HAL status of whether the transmission sent
 */

HAL_StatusTypeDef getLocalFirmwareData(uint8_t ATI_val){
	//check if ATI_val is within the correct parameters
	if(1 < ATI_val > 9){
		return HAL_ERROR;
	}

	int index = 0;
	uint8_t data[4];
	data[index++] = 'A';
	data[index++] = 'T';
	data[index++] = 'I';
	data[index++] = ATI_val + '0';
	return sendData(data, sizeof(data));
}

/**ATO
 *
 * exitATCommandMode - Use to exit command mode on local RF module, so you can send transmissions and receive.
 *
 * @NO PARAMETERS
 *
 * @return -> HAL status of whether the transmission sent
 */

HAL_StatusTypeDef exitLocalATCommandMode(){
	uint8_t data[] = {'A','T','O'};
	return sendData(data, sizeof(data));
}

/**ATSn?
 *
 * getLocalRegisterValue - Get the value of a parameter(register) from the local RF module.
 */

HAL_StatusTypeDef getLocalRegisterValue(uint8_t reg_num){

	//Check if register number is valid
	if(reg_num >= 29){
		return HAL_ERROR;
	}

	uint8_t data[10];
	uint8_t index = 0;

	data[index++] = 'A';
	data[index++] = 'T';
	data[index++] = 'S';

	//Attach the register value to the buffer
	if (reg_num < 10){
		data[index++] = reg_num + '0';
	}
	else{
		data[index++] = (reg_num / 10) + '0';
		data[index++] = (reg_num % 10) + '0';
	}

	data[index ++] = '?';

	return sendData(data, index);
}

/**ATSn=X
 *
 * setParameter - Set parameter numbers(registers) on the local RF module using command ATSn=X. You must use saveRegisterValues and rebootRadio to ensure values
 * 					are changed correctly.
 *
 * @reg_num - The register number(S?) which will be changed
 *
 * @reg_val - The value of which register(S?) will be set to.
 *
 * @return -> HAL status of whether the transmission sent
 */

HAL_StatusTypeDef setLocalParameter(uint8_t reg_num, uint16_t reg_val ){

	//Check if the register number is valid
	if(reg_num >= 29){
		return HAL_ERROR;
	}

	uint8_t data[20];
	uint8_t val_buffer[6];
	uint8_t index = 0;

	data[index++] = 'A';
	data[index++] = 'T';
	data[index++] = 'S';

	//Attach register number to buffer
	if (reg_num < 10){
		data[index++] = reg_num + '0';
	}
	else{
		data[index++] = (reg_num / 10) + '0';
		data[index++] = (reg_num % 10) + '0';
	}

	data[index++] = '=';

	//Value that you will set the "register" to
	sprintf(val_buffer, "%d", reg_val);

	for(int i =0; i < sizeof(val_buffer); i++){
		//check if end of buffer, NULL
		if(val_buffer[i] == 0){
		    break;
		}
		data[index++] = val_buffer[i];
	}

	return sendData(data, index);

}

//TODO: ATRn?

//TODO: ATRn=X

/**ATZ
 *
 * rebootLocalRadio - Will reboot the local RF module. The green light should start blinking. Once connected the green light will be solid green, the red light will
 * 						be blinking. This is used to save the parameter(register) values after altering.
 *
 * @NO PARAMETERS
 *
 *@return -> HAL status of whether the transmission sent
 */


HAL_StatusTypeDef rebootLocalRadio(){
	uint8_t data[] = {'A','T','Z'};
	return sendData(data, sizeof(data));
}

/**AT&F
 *
 * resetLocalParameters - Will reset the local RF module parameters to the factory reset according to the firmware.
 *
 * @NO PARAMETERS
 *
 * @return -> HAL status of whether the transmission sent
 */

HAL_StatusTypeDef resetLocalParameters(){
	uint8_t data[] = {'A','T','&', 'F'};
	return sendData(data, sizeof(data));
}


/**
 * saveRegisterValues - Save the register values to EEPROM altered with ATSn=X using AT&W command. MUST USE ATSn=X first. Should be followed by rebootLocalRadio
 *
 * @NO_PARAMETERS
 *
 * @return -> HAL status of whether the transmission sent
 */
HAL_StatusTypeDef saveLocalRegisterValues(){
	uint8_t data[] = {'A','T','&', 'W'};
	return sendData(data, sizeof(data));
}

/**
 *  resetBootMode - Reset the parameters of the local RF module and enter BootMode. HAVEN"T TESTED, COULD MESS SOMETHING.
 *
 *  @NO PARAMETERS
 *
 *  @return -> HAL status of whether the transmission sent
 */
HAL_StatusTypeDef resetBootMode(){
	uint8_t data[] = {'A','T','&', 'U','P','D','A','T','E'};
	return sendData(data, sizeof(data));
}


