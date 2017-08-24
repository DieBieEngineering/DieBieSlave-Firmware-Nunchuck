#ifndef __OBJECTLIST_H
#define __OBJECTLIST_H

#include "esc_coe.h"
#include <stddef.h>

typedef struct CC_PACKED {
	struct CC_PACKED {
		int8_t JoyStickX;
		int8_t JoyStickY;
		int16_t AcceleroMeterX;
		int16_t AcceleroMeterY;
		int16_t AcceleroMeterZ;
		uint8_t ButtonC:1;
		uint8_t ButtonZ:1;
		uint8_t NunChuckDataValid:1;
		uint8_t pad0:5;
	} NunChuck;
} middleSOESReadbufferTypedef;

typedef struct {
		int8_t ButtonC;
		int8_t ButtonZ;
		int8_t NunChuckDataValid;
		int8_t LED0;
} middleSOESReadBufferBooleansTypedef;

typedef struct CC_PACKED {
   struct CC_PACKED {
      uint8_t LED0:1;
      uint8_t pad0:7;
   } Digital_outputs;
} middleSOESWritebufferTypedef;

extern middleSOESReadbufferTypedef 					middleSOESReadBuffer;
extern middleSOESReadBufferBooleansTypedef	middleSOESReadBufferBooleans;
extern middleSOESWritebufferTypedef 				middleSOESWriteBuffer;

#endif //__OBJECTLIST_H
