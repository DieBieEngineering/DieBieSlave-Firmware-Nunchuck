#include "driverHWECATTickTimer.h"

void (*driverHWECATTickTimercallBackTickFunction)(void);

TIM_HandleTypeDef driverHWECATTickTimerTIM2;
uint32_t driverHWECATTick;
bool driverHWECATTickTimerInterruptActive;

bool driverHWECATTickTimerInit(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  driverHWECATTickTimerTIM2.Instance = TIM2;
  driverHWECATTickTimerTIM2.Init.Prescaler = 71; 														// 1MHz into counter.
  driverHWECATTickTimerTIM2.Init.CounterMode = TIM_COUNTERMODE_UP;
  driverHWECATTickTimerTIM2.Init.Period = 199; 															// 10kHz ISR calls.
  driverHWECATTickTimerTIM2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 		// clock devision doesn't work?
  if (HAL_TIM_Base_Init(&driverHWECATTickTimerTIM2) != HAL_OK) 							// <-- Global interrupt enable is in here
    while(true);
	
  if (HAL_TIM_Base_Start_IT(&driverHWECATTickTimerTIM2) != HAL_OK) 					// <-- Start the timer
    while(true);	

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&driverHWECATTickTimerTIM2, &sClockSourceConfig) != HAL_OK)
    while(true);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&driverHWECATTickTimerTIM2, &sMasterConfig) != HAL_OK)
    while(true);
	
	driverHWECATTickTimerReset();
	driverHWECATTickTimercallBackTickFunction = 0;														// Point callback to nothing
	driverHWECATTickTimerInterruptActive = false;
	
	return false;
}

void driverHWECATTickTimerBindTickFunction(void(*callback)(void)) {
	driverHWECATTickTimercallBackTickFunction = callback;
}

void driverHWECATTickTimerReset(void) {
	driverHWECATTick = 0;
}

uint32_t driverHWECATTickTimerGetTimerValue (void) {
	return driverHWECATTick; // <-- return timer value
}

void driverHWECATTickTimerInterruptEnable(bool newState) {
	driverHWECATTickTimerInterruptActive = newState;
}

void TIM2_IRQHandler(void) {
  HAL_TIM_IRQHandler(&driverHWECATTickTimerTIM2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	// Check soures of Interrupt
	if(driverHWECATTickTimerInterruptActive){		
		if(driverHWECATTickTimercallBackTickFunction && driverHWECATTickTimerInterruptActive)
			driverHWECATTickTimercallBackTickFunction();
	}
	
	driverHWECATTick++;
}
	


