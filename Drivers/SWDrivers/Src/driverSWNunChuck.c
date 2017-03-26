#include "driverSWNunChuck.h"

// https://github.com/infusion/Fritzing/blob/master/Wii-Nunchuk/Nunchuk.h

uint32_t driverNunChuckInitDelayLastTick = 0;

bool driverSWNunChuckInit(void) {
	driverHWI2C3Init();
	driverNunChuckInitDelayLastTick = HAL_GetTick();
	
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));
	uint8_t writeData[2] = {0xF0,0x55};
	driverHWI2C3ReadWrite(NUNCHUCK_ADDRESS,false,writeData,2);
	
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));
	writeData[0] = 0xFB;
	writeData[1] = 0x00;
	driverHWI2C3ReadWrite(NUNCHUCK_ADDRESS,false,writeData,2);
	
	return false;
}

bool driverSWNunChuckSetAddressPointer(uint8_t address) {
	uint8_t writeData[2] = {address};
	return driverHWI2C3ReadWrite(NUNCHUCK_ADDRESS,false,writeData,1);
}

bool driverSWNunChuckGetID(uint64_t *IDPointer) {
	uint8_t readData[6];
	uint8_t offsetPointer;
	bool returnVal = true;

	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,10));
	returnVal &= driverSWNunChuckSetAddressPointer(0xFA);																			// Read the ID data
	
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));
	returnVal &= driverHWI2C3Read(NUNCHUCK_ADDRESS,(uint8_t *)readData,6);
	
	for(offsetPointer = 0; offsetPointer < 6 ; offsetPointer++)
		*IDPointer |= ((uint64_t)readData[offsetPointer] << ((5-offsetPointer)*8));
	
	return returnVal;
}

bool driverSWNunChuckGetCalibrationData(driverSWNunChuckCalibrationDataStruct *dataPointer) {
  uint8_t calibrationData[16];
	bool returnVal = true;
	
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));
	returnVal &= driverSWNunChuckSetAddressPointer(0x20);
	
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1));
	returnVal &= driverHWI2C3Read(NUNCHUCK_ADDRESS,(uint8_t *)calibrationData,14);						// Read the calibration data
	
	dataPointer->accelero0GX = ((calibrationData[0]) << 2) + (((calibrationData[3]) >> 2) & 0x03);
	dataPointer->accelero0GY = ((calibrationData[1]) << 2) + (((calibrationData[3]) >> 2) & 0x03);
	dataPointer->accelero0GZ = ((calibrationData[2]) << 2) + (((calibrationData[3]) >> 2) & 0x03);

	dataPointer->accelero1GX = ((calibrationData[4]) << 2) + (((calibrationData[7]) >> 2) & 0x03);
	dataPointer->accelero1GY = ((calibrationData[5]) << 2) + (((calibrationData[7]) >> 2) & 0x03);
	dataPointer->accelero1GZ = ((calibrationData[6]) << 2) + (((calibrationData[7]) >> 2) & 0x03);
	
	dataPointer->joystickXMax    = calibrationData[8];
	dataPointer->joystickXMin    = calibrationData[9];
	dataPointer->joystickXCenter = calibrationData[10];

	dataPointer->joystickYMax    = calibrationData[11];
	dataPointer->joystickYMin    = calibrationData[12];
	dataPointer->joystickYCenter = calibrationData[13];	
	
	return returnVal;
}

bool driverSWNunChuckGetSensorData(driverSWNunChuckSensorDataStruct *dataPointer) {
	uint8_t readData[6];
	bool returnVal = true;
	
	driverNunChuckInitDelayLastTick = HAL_GetTick();
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1)){};
	returnVal &= driverHWI2C3Read(NUNCHUCK_ADDRESS,(uint8_t *)readData,6);	  // Read the data

	driverNunChuckInitDelayLastTick = HAL_GetTick();
	while(!modDelayTick1ms(&driverNunChuckInitDelayLastTick,1)){};
	returnVal &= driverSWNunChuckSetAddressPointer(0x00);											// Reset data pointer

	dataPointer->joystickX = readData[0];
	dataPointer->joystickY = readData[1];
	dataPointer->accelerometerX = ((readData[2]) << 2) + (((readData[5]) >> 2) & 0x03);
	dataPointer->accelerometerY = ((readData[3]) << 2) + (((readData[5]) >> 4) & 0x03);
	dataPointer->accelerometerZ = ((readData[4]) << 2) + (((readData[5]) >> 6) & 0x03);
	dataPointer->buttonZ = (readData[5] & 0x01) ? false : true;
	dataPointer->buttonC = (readData[5] & 0x02) ? false : true;
	
	return returnVal;
}
