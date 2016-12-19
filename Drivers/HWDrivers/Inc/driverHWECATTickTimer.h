#include "stm32f3xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

bool driverHWECATTickTimerInit(void);
void driverHWECATTickTimerBindTickFunction(void(*callback)(void));
void driverHWECATTickTimerReset(void);
void driverHWECATTickTimerInterruptEnable(bool newState);
uint32_t driverHWECATTickTimerGetTimerValue(void);
