#include "driverHWI2C3.h"

I2C_HandleTypeDef driverHWI2C3Handle;

void driverHWI2C3Init(void) {	
	__HAL_RCC_GPIOC_CLK_ENABLE(); // everything else is handled in the HAL driver
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
  driverHWI2C3Handle.Instance = I2C3;
  driverHWI2C3Handle.Init.Timing = 0x10808DD3;
  driverHWI2C3Handle.Init.OwnAddress1 = 0;
  driverHWI2C3Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  driverHWI2C3Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  driverHWI2C3Handle.Init.OwnAddress2 = 0;
  driverHWI2C3Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  driverHWI2C3Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  driverHWI2C3Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&driverHWI2C3Handle) != HAL_OK)
		while(true);
	
  if (HAL_I2CEx_ConfigAnalogFilter(&driverHWI2C3Handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
		while(true);
}

bool driverHWI2C3ReadWrite(uint16_t DevAddress, bool readWrite, uint8_t *pData, uint16_t Size) {
	uint16_t addresRW = (DevAddress << 1) | readWrite;
	return (HAL_I2C_Master_Transmit(&driverHWI2C3Handle,addresRW,pData,Size,driverHWI2C3DefaultTimout) == HAL_OK);
};

bool driverHWI2C3Read(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
	uint16_t addresRW = (DevAddress << 1) | 0x01; // Read bit high
	return (HAL_I2C_Master_Receive(&driverHWI2C3Handle,addresRW,pData,Size,driverHWI2C3DefaultTimout) == HAL_OK);
};




