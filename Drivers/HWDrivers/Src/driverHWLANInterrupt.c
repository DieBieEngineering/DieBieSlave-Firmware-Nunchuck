#include "driverHWLANInterrupt.h"

void (*driverHWLANInterruptcallBackIRQFunction)(void);
void (*driverHWLANInterruptcallBackSYNC0Function)(void);
void (*driverHWLANInterruptcallBackSYNC1Function)(void);
bool driverHWLANInterruptcallBackIRQActive, driverHWLANInterruptcallBackSYNC0Active, driverHWLANInterruptcallBackSYNC1Active;
bool driverHWLANInterruptInitialized = false;

void driverHWLANInterruptInit(void) {
	if(!driverHWLANInterruptInitialized) {
		__HAL_RCC_GPIOB_CLK_ENABLE();																									// Enable clock
		GPIO_InitTypeDef GPIO_InitStruct;																							// Init GPIO
		GPIO_InitStruct.Pin = LAN_IRQ_Pin|LAN_SYNC0_Pin|LAN_SYNC1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x0F, 0x00); 														// IRQ is on PB13, SYNC1 is on PB14, lowest priority interrupt
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		HAL_NVIC_SetPriority(EXTI0_IRQn, 0x0F, 0x00); 																// SYNC0 is on PB0, lowest priority interrupt
		HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		
		driverHWLANInterruptcallBackIRQFunction = 0;																	// Init pointers to nothing
		driverHWLANInterruptcallBackSYNC0Function = 0;
		driverHWLANInterruptcallBackSYNC1Function = 0;
		
		driverHWLANInterruptcallBackIRQActive = false;																// Disable the interrupts
		driverHWLANInterruptcallBackSYNC0Active = false;
		driverHWLANInterruptcallBackSYNC1Active = false;
		
		driverHWLANInterruptInitialized = true;
	}
}

void driverHWLANInterruptIRQBindFunction(void(*callback)(void)) {
	driverHWLANInterruptcallBackIRQFunction = callback;
}

void driverHWLANInterruptSYNC0BindFunction(void(*callback)(void)) {
	driverHWLANInterruptcallBackSYNC0Function = callback;
}

void driverHWLANInterruptSYNC1BindFunction(void(*callback)(void)) {
	driverHWLANInterruptcallBackSYNC1Function = callback;
}

void driverHWLANInterruptDisableAll(void) {
	driverHWLANInterruptEnable(LAN_IRQ_Pin,false);
	driverHWLANInterruptEnable(LAN_SYNC0_Pin,false);
	driverHWLANInterruptEnable(LAN_SYNC1_Pin,false);
}

void driverHWLANInterruptEnableAll(void) {
	driverHWLANInterruptEnable(LAN_IRQ_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC0_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC1_Pin,true);
}

void driverHWLANInterruptEnable(uint16_t GPIO_Pin,bool newState) {
	switch(GPIO_Pin) {
		case LAN_IRQ_Pin:
			driverHWLANInterruptcallBackIRQActive = newState;
			break;
		case LAN_SYNC0_Pin:
			driverHWLANInterruptcallBackSYNC0Active = newState;
			break;
		case LAN_SYNC1_Pin:
			driverHWLANInterruptcallBackSYNC1Active = newState;
			break;
		default:
			break;
	}
}

void EXTI0_IRQHandler(void) {
	if(__HAL_GPIO_EXTI_GET_IT(LAN_SYNC1_Pin) != RESET)														// Detect on what input line
    HAL_GPIO_EXTI_IRQHandler(LAN_SYNC1_Pin);	
}

void EXTI15_10_IRQHandler(void) {																								// An interrupt accured at any input from 10 to 15
	if(__HAL_GPIO_EXTI_GET_IT(LAN_IRQ_Pin) != RESET)															// Detect on what input line
    HAL_GPIO_EXTI_IRQHandler(LAN_IRQ_Pin);
	if(__HAL_GPIO_EXTI_GET_IT(LAN_SYNC0_Pin) != RESET)														// Detect on what input line
    HAL_GPIO_EXTI_IRQHandler(LAN_SYNC0_Pin);	
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {																// An interrupt accured at any input from 5 to 9, find and call the appropriate 
	switch(GPIO_Pin) {
		case LAN_IRQ_Pin:																														// Interrupt on PIN8
			if(driverHWLANInterruptcallBackIRQFunction && driverHWLANInterruptcallBackIRQActive)// If callback contains a pointer
				driverHWLANInterruptcallBackIRQFunction();															// Run function
			break;
		case LAN_SYNC0_Pin:																													// Interrupt on PIN8
			if(driverHWLANInterruptcallBackSYNC0Function && driverHWLANInterruptcallBackSYNC0Active)// If callback contains a pointer
				driverHWLANInterruptcallBackSYNC0Function();														// Run function
			break;
		case LAN_SYNC1_Pin:																													// Interrupt on PIN8
			if(driverHWLANInterruptcallBackSYNC1Function && driverHWLANInterruptcallBackSYNC1Active)// If callback contains a pointer
				driverHWLANInterruptcallBackSYNC1Function();														// Run function
			break;
		default:																																		// No idea what pin
			break;
	}
}
