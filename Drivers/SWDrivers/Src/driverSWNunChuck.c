#include "driverSWNunChuck.h"

uint32_t driverNunChuckInitDelayLastTick = 0;
uint32_t driverNunChuckMeasureIntervalLastTick = 0;

volatile uint8_t readData[6] = {0,0,0,0,0,0};

bool driverSWNunChuckInit(void) {
	driverHWI2C3Init();
	driverNunChuckMeasureIntervalLastTick = driverNunChuckInitDelayLastTick = HAL_GetTick();
	
	uint8_t writeData[2] = {0xF0,0x55};
	driverHWI2C3ReadWrite(NUNCHUCK_ADDRES,false,writeData,2);
	
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));					// Wait one milisecond - Not needed? - Solve in a better way?

	writeData[0] = 0xFB;
	writeData[1] = 0x00;
	
	driverHWI2C3ReadWrite(NUNCHUCK_ADDRES,false,writeData,2);
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));					// Wait one milisecond - Not needed? - Solve in a better way?

	return false;
}

bool driverSWNunChuckGetData(driverSWNunChuckDataStruct *dataPointer) {
	if(modDelayTick1ms(&driverNunChuckMeasureIntervalLastTick,driverNunChuckInterval)) {
		// Get the new data
		uint8_t writeData[1] = {0x00};
		//uint8_t readData[6];
		driverHWI2C3ReadWrite(NUNCHUCK_ADDRES,false,writeData,1);						// Set register pointer to 0
		driverHWI2C3Read(NUNCHUCK_ADDRES,(uint8_t *)readData,6);						// Read the data
		
		dataPointer->joystickX = readData[0];
		dataPointer->joystickY = readData[1];
		
		dataPointer->accelerometerX = ((readData[2]) << 2) + (((readData[5]) >> 2) & 0x03);
		dataPointer->accelerometerY = ((readData[3]) << 2) + (((readData[5]) >> 4) & 0x03);
		dataPointer->accelerometerZ = ((readData[4]) << 2) + (((readData[5]) >> 6) & 0x03);

		dataPointer->buttonZ = (readData[5] & 0x01) ? false : true;
		dataPointer->buttonC = (readData[5] & 0x02) ? false : true;
		
		return true;
	}else{
		return false;
	}
}
