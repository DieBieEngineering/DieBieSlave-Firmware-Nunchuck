#ifndef __DRIVERSWNUNCHUCK_H
#define __DRIVERSWNUNCHUCK_H

#include <string.h>
#include "driverHWI2C3.h"
#include "modDelay.h"

#define NUNCHUCK_ADDRESS 0x52         			// NunChuck Address
#define driverNunChuckInterval 10					  // GetSensorData interval in ms

typedef struct {
	uint16_t accelero0GX;
	uint16_t accelero0GY;
	uint16_t accelero0GZ;
	uint16_t accelero1GX;
	uint16_t accelero1GY;
	uint16_t accelero1GZ;
	uint8_t joystickXMax;
	uint8_t joystickXMin;
	uint8_t joystickXCenter;
	uint8_t joystickYMax;
	uint8_t joystickYMin;
	uint8_t joystickYCenter;
} driverSWNunChuckCalibrationDataStruct;

typedef struct {
	uint8_t joystickX;
	uint8_t joystickY;
	uint16_t accelerometerX;
	uint16_t accelerometerY;
	uint16_t accelerometerZ;
	bool buttonC;
	bool buttonZ;	
} driverSWNunChuckSensorDataStruct;

bool driverSWNunChuckInit(void);
bool driverSWNunChuckGetID(uint64_t *IDPointer);
bool driverSWNunChuckGetCalibrationData(driverSWNunChuckCalibrationDataStruct *dataPointer);
bool driverSWNunChuckGetSensorData(driverSWNunChuckSensorDataStruct *dataPointer);

#endif
