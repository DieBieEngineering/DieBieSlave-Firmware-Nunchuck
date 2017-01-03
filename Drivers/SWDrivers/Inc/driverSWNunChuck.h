#ifndef __DRIVERSWNUNCHUCK_H
#define __DRIVERSWNUNCHUCK_H

#include "driverHWI2C3.h"
#include "modDelay.h"

#define NUNCHUCK_ADDRES 0x52         // For the fake nunchucks
//#define NUNCHUCK_ADDRES 0x40         // For the real nunchucks

#define driverNunChuckInterval 20					// 20 measurements a second

typedef struct {
	uint8_t joystickX;
	uint8_t joystickY;
	uint16_t accelerometerX;
	uint16_t accelerometerY;
	uint16_t accelerometerZ;
	bool buttonC;
	bool buttonZ;	
} driverSWNunChuckDataStruct;

bool driverSWNunChuckInit(void);
bool driverSWNunChuckGetData(driverSWNunChuckDataStruct *dataPointer);

#endif
