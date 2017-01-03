#include "stm32f3xx_hal.h"
#include "stdbool.h"
#include "stdlib.h"

#define driverHWI2C3DefaultTimout										100

void driverHWI2C3Init(void);
bool driverHWI2C3ReadWrite(uint16_t DevAddress, bool readWrite, uint8_t *pData, uint16_t Size);
bool driverHWI2C3Read(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
