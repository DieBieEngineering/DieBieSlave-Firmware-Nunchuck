#ifndef __OBJECTLIST_H
#define __OBJECTLIST_H

#include "esc_coe.h"
#include <stddef.h>

typedef struct CC_PACKED {
	int8_t joyStickX;
	int8_t joyStickY;
	int16_t AcceleroMeterX;
	int16_t AcceleroMeterY;
	int16_t AcceleroMeterZ;
	uint8_t buttonC;
	uint8_t buttonZ;
  uint8_t dataValid;
} middleSOESReadbufferTypedef;

typedef struct CC_PACKED {
   uint8_t LED;
} middleSOESWritebufferTypedef;

extern middleSOESReadbufferTypedef 			middleSOESReadBuffer;
extern middleSOESWritebufferTypedef 		middleSOESWriteBuffer;

#endif //__OBJECTLIST_H
