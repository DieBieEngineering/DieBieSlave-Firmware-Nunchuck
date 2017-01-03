#ifndef __OBJECTLIST_H
#define __OBJECTLIST_H

#include "esc_coe.h"
#include <stddef.h>

typedef struct CC_PACKED {
	uint8_t joyStickX;
	uint8_t joyStickY;
	uint8_t buttonC;
	uint8_t buttonZ;
	uint32_t encoder;
} _Rbuffer;

typedef struct CC_PACKED {
   uint8_t LED;
} _Wbuffer;

typedef struct {
   uint32_t reset_counter;
} _Cbuffer;

extern _Rbuffer ReadBuffer;
extern _Wbuffer WriteBuffer;
extern _Cbuffer CommandBuffer;
extern uint32_t encoder_scale;
extern uint32_t encoder_scale_mirror;

#endif //__OBJECTLIST_H
