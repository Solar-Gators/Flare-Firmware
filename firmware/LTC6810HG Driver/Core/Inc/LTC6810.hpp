#include "main.h"

class LTC6810 {
public:
	LTC6810(SPI_HandleTypeDef *phspi_, GPIO_TypeDef *pgpio_port, uint16_t cs_pin, uint8_t num_devices);

	HAL_StatusTypeDef ReadVoltages();


	uint16_t calculatePEC(uint8_t *pData , int len);
private:

	void InitPEC15Table();

	HAL_StatusTypeDef WriteRegister(uint8_t cmdByteHigh, uint8_t cmdByteLow, uint8_t *dataBytes, uint8_t dataLen);
	HAL_StatusTypeDef ReadRegister(uint8_t cmdByteHigh, uint8_t cmdByteLow, uint8_t *receiveBytes, uint8_t receiveLen);
	HAL_StatusTypeDef SendCommand(uint8_t cmdByteHigh, uint8_t cmdByteLow);
	HAL_StatusTypeDef WakeDevices();

	uint16_t *pCellVoltages;

	SPI_HandleTypeDef *phspi;
	GPIO_TypeDef *pgpioPort;
	uint16_t csPin;
	uint8_t numDevices;

};
