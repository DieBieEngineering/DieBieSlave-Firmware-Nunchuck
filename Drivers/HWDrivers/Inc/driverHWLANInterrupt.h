#include "stm32f3xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

void driverHWLANInterruptInit(void);
void driverHWLANInterruptIRQBindFunction(void(*callback)(void));
void driverHWLANInterruptSYNC0BindFunction(void(*callback)(void));
void driverHWLANInterruptSYNC1BindFunction(void(*callback)(void));
void driverHWLANInterruptDisableAll(void);
void driverHWLANInterruptEnableAll(void);
void driverHWLANInterruptEnable(uint16_t GPIO_Pin,bool newState);
