//Source file for the RDF900x RF Module for Telemetry Board

//INCLUDES
#include "rfd900x.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

#define UART_TIMEOUT 10


//send the whole buffer of data
HAL_StatusTypeDef sendData(uint8_t *databuffer){

	HAL_StatusTypeDef status;

	status = HAL_UART_Transmit(&huart1, databuffer, sizeof(databuffer), UART_TIMEOUT );
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
	return sendData(data);
}

//AT&W, Write current paramters to EEPROM
HAL_StatusTypeDef writeRegister(){
	uint8_t data[] = {'A','T','&', 'W'};
	return sendData(data);
}

//ATZ, Rebbot radio
HAL_StatusTypeDef rebootRadio(){
	uint8_t data[] = {'A','T','Z'};
	return sendData(data);
}



//set register value for configuration
HAL_StatusTypeDef setRegister(uint8_t* reg_num, uint8_t* reg_val ){

	uint8_t data[20];
	uint8_t index = 0;

	data[index++] = 'A';
	data[index++] = 'T';
	data[index++] = 'S';

	//attach the register value
	for (int i = 0; i < sizeof(reg_num); i++){
		data[index++] = reg_num[i];
	}

	data[index++] = '=';

	//value that you will set the "register" to
	for(int i =0; i < sizeof(reg_val); i++){
		data[index++] = reg_val[i];
	}
	data[index++] = '\0';

	return sendData(data);

}


//use to enter AT Command Mode to "set registers" for the module
HAL_StatusTypeDef enterATCommandMode(){

	uint8_t errors = 0;

	//Figure out how to use putty to send and receive data

	//send "+++" to enterATCommandMode, no quotes
	if(sendData("+++") != HAL_OK){
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
