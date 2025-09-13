//main file for user, to prevent with regular main function that performs
	//configurations
#include "rfd900x.h"

//main function inside main.c while(1)
void app_main(){

	while(1){
		}

}


void testing(){
	//test for regular transmittion //WORKS!!!
	/*
	uint8_t data[] = "checking";

	while(1){

		HAL_StatusTypeDef check = sendData(data, sizeof(data));
		if(check == HAL_OK){
			data[1] = 'w';
		}
	}
	*/

	//test other general send functions
	while(1){
		HAL_StatusTypeDef status = setRegister(10,20);
	}

}
