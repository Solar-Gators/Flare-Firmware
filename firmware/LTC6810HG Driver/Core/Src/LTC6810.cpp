#include "LTC6810.hpp"
#include "LTC6810Registers.hpp"

int16_t pec15Table[256];
int16_t CRC15_POLY = 0x4599;

LTC6810::LTC6810(SPI_HandleTypeDef *phspi_, GPIO_TypeDef *pgpio_port, uint16_t cs_pin, uint8_t num_devices) :
	phspi(phspi_), pgpioPort(pgpio_port), csPin(cs_pin), numDevices(num_devices)
{
	InitPEC15Table();
	WakeDevices();
}

HAL_StatusTypeDef LTC6810::ReadVoltages(){
	HAL_StatusTypeDef status;



	// cells 1 through 3 and 4 through 6 are two different registers, and each value is sent as two bytes
	// this code combines 2 bytes into 1 uint16, and also interweaves values so first 6 voltages in pCellVoltages belong to device 1, next 6 belong to device 2 etc

	// start ADC conversions
	status = SendCommand(LTC6810_REG_ADCV_H, LTC6810_REG_ADCV_L);
	if (status != HAL_OK){return status;}
	HAL_Delay(2); // max time for ADC to finish is about 1.3 ms


	// code below is unecessary and probably wouldn't work bc data line should be read later than for a normal spi command
//	uint8_t *pollADC = nullptr;
//	while (*pollADC != 0b11111111){
//		HAL_Delay(1);
//		status = ReadRegister(LTC6810_REG_PLADCV_H, LTC6810_REG_PLADCV_L, pollADC, 1);
//		if (status != HAL_OK){return status;}
//	}


	uint8_t pCell1to3VoltageBytes[numDevices*6];
	uint8_t pCell4to6VoltageBytes[numDevices*6];
	status = ReadRegister(LTC6810_REG_RDCVA_H, LTC6810_REG_RDCVA_L, pCell1to3VoltageBytes, numDevices*6);
	if (status != HAL_OK){return status;}
	status = ReadRegister(LTC6810_REG_RDCVB_H, LTC6810_REG_RDCVB_L, pCell4to6VoltageBytes, numDevices*6);
	if (status != HAL_OK){return status;}

	for (int i = 0; i < numDevices; i++){
		for (int j = 0; j < 3; j++){
			pCellVoltages[6*i + j] 		= (pCell1to3VoltageBytes[6*i + 2*j] << 8) | (pCell1to3VoltageBytes[6*i + 2*j + 1]);
			pCellVoltages[6*i + j + 3] 	= (pCell4to6VoltageBytes[6*i + 2*j] << 8) | (pCell4to6VoltageBytes[6*i + 2*j + 1]);
			// i don't know what order bytes are shifted out, the indices could be backwards (so +1 could be MSB 8 bits instead)
			// i think first device (one directly connected to mcu) is the one whose data is received first by mcu
		}
	}

	return status;
}



HAL_StatusTypeDef LTC6810::WriteRegister(uint8_t cmdByteHigh, uint8_t cmdByteLow, uint8_t *dataBytes, uint8_t dataLen) //length in bytes
{	// writes the same bytes (dataBytes) to all devices at the register defined by the cmd bytes

	HAL_StatusTypeDef status;
	if (cmdByteHigh & (0xF8))	{return HAL_ERROR;} 		//only lowest 3 bits of cmdByteHigh can be set

	uint8_t transmitLen = 2 + 2 + (dataLen + 2)*numDevices; 	//command + command PEC + data + data PEC, last 2 repeated for each device
	uint8_t transmitBytes[transmitLen];							//all bytes being sent


	//	data bytes and their PEC (first device):
	uint8_t transmitDataBytes[(dataLen + 2) * numDevices];		//just data bytes and their PECs
	for (int i = 0; i < dataLen; i++){
		transmitDataBytes[i] = dataBytes[i];
	}

	uint16_t dataPEC = calculatePEC(dataBytes, dataLen);
	transmitDataBytes[dataLen] 		= (uint8_t) (dataPEC >> 8);
	transmitDataBytes[dataLen+1]	= (uint8_t) (dataPEC & 0x00FF);

	// repeat data & PEC bytes, once for each device
	for (int n = 1; n <= numDevices; n++){
		for (int i = 0; i < dataLen + 2; i++){
			transmitDataBytes[n*(dataLen + 2) + i] = transmitDataBytes[i];
		}
	}

	// command bytes and their PEC:
	transmitBytes[0] = cmdByteHigh;
	transmitBytes[1] = cmdByteLow;

	uint16_t cmdPEC = calculatePEC(transmitBytes, 2);
	transmitBytes[2] = (uint8_t) (cmdPEC >> 8);
	transmitBytes[3] = (uint8_t) (cmdPEC & 0x00FF);

	// put data & command bytes together
	for (int i = 0; i < (dataLen + 2)*numDevices; i++){
		transmitBytes[4 + i] = transmitDataBytes[i];
	}

	status = HAL_SPI_Transmit(phspi, transmitBytes, transmitLen, HAL_MAX_DELAY);
	return status;
}

HAL_StatusTypeDef LTC6810::ReadRegister(uint8_t cmdByteHigh, uint8_t cmdByteLow, uint8_t *receiveBytes, uint8_t receiveLen) //length in bytes
{	//reads contents of register defined by cmd bytes for all connected devices and stores them in receiveBytes

	HAL_StatusTypeDef status;
	if (receiveLen < numDevices*6)	{return HAL_ERROR;} //not enough space to store all data

	uint8_t transmitBytes[4];

	// command bytes and their PEC:
	transmitBytes[0] = cmdByteHigh;
	transmitBytes[1] = cmdByteLow;

	uint16_t cmdPEC = calculatePEC(transmitBytes, 2);
	transmitBytes[2] = (uint8_t) (cmdPEC >> 8);
	transmitBytes[3] = (uint8_t) (cmdPEC & 0x00FF);

	status = HAL_SPI_TransmitReceive(phspi, transmitBytes, receiveBytes, 36, HAL_MAX_DELAY);
	if (status != HAL_OK) {return status;}

	return status;
}

HAL_StatusTypeDef LTC6810::SendCommand(uint8_t cmdByteHigh, uint8_t cmdByteLow){ // only for initiating ADC conversions and polling if ADC is done
	uint8_t transmitBytes[4];
	HAL_StatusTypeDef status;

	// command bytes and their PEC:
	transmitBytes[0] = cmdByteHigh;
	transmitBytes[1] = cmdByteLow;

	uint16_t cmdPEC = calculatePEC(transmitBytes, 2);
	transmitBytes[2] = (uint8_t) (cmdPEC >> 8);
	transmitBytes[3] = (uint8_t) (cmdPEC & 0x00FF);

	status = HAL_SPI_Transmit(phspi, transmitBytes, 6*numDevices, HAL_MAX_DELAY);
	if (status != HAL_OK) {return status;}

	return status;
}

HAL_StatusTypeDef LTC6810::WakeDevices()
{
	uint8_t dummyData[36];
	return ReadRegister(LTC6810_REG_RDCFG_H, LTC6810_REG_RDCFG_L, dummyData, 36);
}

void LTC6810::InitPEC15Table()
{
	for (int i = 0; i < 256; i++)
	{
		int16_t remainder = i << 7;
		for (int bit = 8; bit > 0; --bit)
		{
			if (remainder & 0x4000)
			{
				remainder = ((remainder << 1));
				remainder = (remainder ^ CRC15_POLY);
			}
			else
			{
				remainder = ((remainder << 1));
			}
		}
		pec15Table[i] = remainder&0xFFFF;
	}
}

uint16_t LTC6810::calculatePEC(uint8_t *pData, int len)
{
	int16_t remainder, address;
	uint8_t currentBit;
	remainder = 16; //PEC seed
	for (int i = 0; i < len; i++)
	{
		currentBit = pData[i/8] & (1 << (i % 8));
		address = ((remainder >> 7) ^ currentBit) & 0xFF;//calculate PEC table address
		remainder = (remainder << 8 ) ^ pec15Table[address];
	}
	return (remainder << 1);
}


