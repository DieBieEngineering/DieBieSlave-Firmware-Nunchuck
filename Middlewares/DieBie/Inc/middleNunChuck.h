#include "driverSWNunChuck.h"
#include <stdint.h>

#define middleNunChuckDefaultMeasureInterval 10
#define middleNunChuckErrortMeasureInterval 500


typedef struct {
	int8_t joystickX;
	int8_t joystickY;
	int16_t accelerometerX;
	int16_t accelerometerY;
	int16_t accelerometerZ;
	bool buttonC;
	bool buttonZ;	
	uint64_t nunChuckID;
	bool dataValid;
} middleNunChuckDataStruct;

void middleNunChuckInit(void);
void middleNunChuckTask(void);
bool middleNunChuckDataValid(void);
void middleNunChuckNewDataEvent(void (*eventFunctionPointer)(middleNunChuckDataStruct newData));
void middleNunChuckSetMeasureInterval(uint32_t delayInMS);
