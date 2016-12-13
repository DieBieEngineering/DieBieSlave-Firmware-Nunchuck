#include <stdint.h>
#include <stdbool.h>

bool driverHWECATTickTimerInit(void);
void driverHWECATTickTimerSetEnableState(bool EnableState);
void driverHWECATTickTimerReset(void);
uint32_t driverHWECATTickTimerGetTimerValue(void);