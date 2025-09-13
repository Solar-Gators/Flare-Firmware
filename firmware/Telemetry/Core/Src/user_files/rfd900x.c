//Source file for the RDF900x RF Module for Telemetry Board

//INCLUDES
#include "rfd900x.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

#define UART_TIMEOUT 10


//send the whole buffer of data
HAL_StatusTypeDef sendData(uint8_t *databuffer, uint16_t sizeData){

	HAL_StatusTypeDef status;

	status = HAL_UART_Transmit(&huart1, databuffer, sizeData, UART_TIMEOUT );
	HAL_Delay(1000);

	return status;
}

//use to get RF module data
HAL_StatusTypeDef getFirmwareData(uint8_t ATI_val){
	int index = 0;
	uint8_t data[4];
	data[index++] = 'A';
	data[index++] = 'T';
	data[index++] = 'I';
	data[index++] = ATI_val;
	return sendData(data, sizeof(data));
}

//AT&W, Write current parameters to EEPROM
HAL_StatusTypeDef saveRegisterValues(){
	uint8_t data[] = {'A','T','&', 'W'};
	return sendData(data, sizeof(data));
}

//ATZ, Reboot radio
HAL_StatusTypeDef rebootRadio(){
	uint8_t data[] = {'A','T','Z'};
	return sendData(data, sizeof(data));
}



//set register value for configuration
HAL_StatusTypeDef setRegister(uint8_t reg_num, uint16_t reg_val ){

	uint8_t data[20];
	uint8_t val_buffer[6];
	uint8_t index = 0;

	data[index++] = 'A';
	data[index++] = 'T';
	data[index++] = 'S';

	//attach the register value
	if (reg_num < 10){
		data[index++] = reg_num + '0';
	}
	else{
		data[index++] = (reg_num / 10) + '0';
		data[index++] = (reg_num % 10) + '0';
	}

	data[index++] = '=';

	//value that you will set the "register" to
	sprintf(val_buffer, "%d", reg_val);

	for(int i =0; i < sizeof(val_buffer); i++){
		data[index++] = val_buffer[i];
	}
	data[index++] = '\0';

	return sendData(data, sizeof(data));

}


//use to enter AT Command Mode to "set registers" for the module
HAL_StatusTypeDef enterATCommandMode(){

	uint8_t errors = 0;

	//Figure out how to use putty to send and receive data

	//send "+++" to enterATCommandMode, no quotes
	if(sendData("+++", sizeof("+++")) != HAL_OK){
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
