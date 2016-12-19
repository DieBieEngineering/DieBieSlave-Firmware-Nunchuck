#include "stm32f3xx_hal.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#define driverHWSPI1DefaultTimout										100

void driverHWSPI1Init(void);
bool driverHWSPI1Write(uint8_t *writeBuffer, uint8_t noOfBytesToWrite);
bool driverHWSPI1WriteSingleByte(uint8_t data);
bool driverHWSPI1WriteRead(uint8_t *writeBuffer, uint8_t noOfBytesToWrite, uint8_t *readBuffer, uint8_t noOfBytesToRead);
void driverHWSPI1SetCS(GPIO_PinState PinState);
