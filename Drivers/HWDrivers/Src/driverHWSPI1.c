#include "driverHWSPI1.h"

SPI_HandleTypeDef driverHWSPI1Handle;

void driverHWSPI1Init(void) {
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = LAN_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LAN_CS_GPIO_Port, &GPIO_InitStruct);
	
  driverHWSPI1Handle.Instance = SPI1;
  driverHWSPI1Handle.Init.Mode = SPI_MODE_MASTER;
  driverHWSPI1Handle.Init.Direction = SPI_DIRECTION_2LINES;
  driverHWSPI1Handle.Init.DataSize = SPI_DATASIZE_8BIT;
  driverHWSPI1Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
  driverHWSPI1Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
  driverHWSPI1Handle.Init.NSS = SPI_NSS_SOFT;
  driverHWSPI1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  driverHWSPI1Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
  driverHWSPI1Handle.Init.TIMode = SPI_TIMODE_DISABLE;
  driverHWSPI1Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  driverHWSPI1Handle.Init.CRCPolynomial = 7;
  driverHWSPI1Handle.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  driverHWSPI1Handle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&driverHWSPI1Handle) != HAL_OK)
  {
    while(true);
  }
	
	HAL_GPIO_WritePin(LAN_CS_GPIO_Port,LAN_CS_Pin,GPIO_PIN_SET);
};

bool driverHWSPI1Write(uint8_t *writeBuffer, uint8_t noOfBytesToWrite) {
	uint8_t *readBuffer;																																					// Make fake buffer holder
	HAL_StatusTypeDef halReturnStatus;																														// Make holder for HAL state
	readBuffer = malloc(noOfBytesToWrite);																												// Make fake buffer for
	
	halReturnStatus = HAL_SPI_TransmitReceive(&driverHWSPI1Handle,writeBuffer,readBuffer,noOfBytesToWrite,driverHWSPI1DefaultTimout);	// Write desired data to slave and store the received data in readBuffer
	while( driverHWSPI1Handle.State == HAL_SPI_STATE_BUSY );  																		// Wait until transmission is complete
	
	free(readBuffer);																																							// Dump de fake buffer
	
	return (halReturnStatus == HAL_OK);																														// Return true if all went OK
};

bool driverHWSPI1WriteRead(uint8_t *writeBuffer, uint8_t noOfBytesToWrite, uint8_t *readBuffer, uint8_t noOfBytesToRead) {
	uint8_t *writeArray, *readArray;
	HAL_StatusTypeDef halReturnStatus;																														// Make holder for HAL state
	
	writeArray = malloc(sizeof(uint8_t)*(noOfBytesToWrite+noOfBytesToRead));
	readArray = malloc(sizeof(uint8_t)*(noOfBytesToWrite+noOfBytesToRead));	
	memset(writeArray,0xFF,noOfBytesToWrite+noOfBytesToRead);
	memcpy(writeArray,writeBuffer,noOfBytesToWrite);
	
	halReturnStatus = HAL_SPI_TransmitReceive(&driverHWSPI1Handle,writeArray,readArray,noOfBytesToWrite+noOfBytesToRead,driverHWSPI1DefaultTimout);
	while( driverHWSPI1Handle.State == HAL_SPI_STATE_BUSY );  																		// wait xmission complete

	memcpy(readBuffer,readArray+noOfBytesToWrite,noOfBytesToRead);
	
	free(writeArray);
	free(readArray);
	
	return (halReturnStatus == HAL_OK);																														// Return true if all went OK
};

void driverHWSPI1SetCS(GPIO_PinState PinState) {
		HAL_GPIO_WritePin(LAN_CS_GPIO_Port,LAN_CS_Pin,PinState);
}
