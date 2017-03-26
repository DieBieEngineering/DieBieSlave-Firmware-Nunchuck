#include "middleNunChuck.h"

driverSWNunChuckCalibrationDataStruct middleNunChuckCalibrationData;
bool																	middleNunChuckCalibrationDataValid;
uint64_t 															middleNunChuckID;
driverSWNunChuckSensorDataStruct			middleNunChuckSensorData;
middleNunChuckDataStruct 							middleNunChuckData;
uint32_t 															middleNunChuckMeasureIntervalLastTick;
uint32_t															middleNunChuckMeasureIntervalDesired;
uint32_t															middleNunChuckMeasureInterval;

void (*middleNunChuckEventFunctionPointer)(middleNunChuckDataStruct newData);
int32_t middleNunchuckMapVariable(int32_t inputVariable, int32_t inputLowerLimit, int32_t inputUpperLimit, int32_t outputLowerLimit, int32_t outputUpperLimit);

void middleNunChuckInit(void) {
  driverSWNunChuckInit();
	
  middleNunChuckCalibrationDataValid  = driverSWNunChuckGetCalibrationData(&middleNunChuckCalibrationData);
  middleNunChuckCalibrationDataValid &= driverSWNunChuckGetID(&middleNunChuckID);
	
	middleNunChuckMeasureIntervalDesired = middleNunChuckDefaultMeasureInterval;				// 100Hz Measure interval
	middleNunChuckMeasureIntervalLastTick = HAL_GetTick();
	
	middleNunChuckData.nunChuckID = middleNunChuckID;
}

void middleNunChuckTask(void) {
	bool newDataValid = false;
	
	if(modDelayTick1ms(&middleNunChuckMeasureIntervalLastTick,10)) {
		if(middleNunChuckCalibrationDataValid) {
			middleNunChuckMeasureInterval = middleNunChuckMeasureIntervalDesired;
			newDataValid = driverSWNunChuckGetSensorData(&middleNunChuckSensorData);
		}else{
			middleNunChuckMeasureInterval = middleNunChuckErrortMeasureInterval;
			middleNunChuckInit();
		}
		
		if(newDataValid) {
			middleNunChuckData.dataValid = true;
			middleNunChuckMeasureInterval = middleNunChuckMeasureIntervalDesired;
			
			// Convert the raw sensor values to usable data
			middleNunChuckData.buttonC = middleNunChuckSensorData.buttonC;
			middleNunChuckData.buttonZ = middleNunChuckSensorData.buttonZ;
			int32_t middleNunChuckDataJoystickXCentered = middleNunChuckSensorData.joystickX - middleNunChuckCalibrationData.joystickXCenter;
			int32_t middleNunChuckDataJoystickYCentered = middleNunChuckSensorData.joystickY - middleNunChuckCalibrationData.joystickYCenter;
			
			if(middleNunChuckDataJoystickXCentered >= 0)
				middleNunChuckData.joystickX = middleNunchuckMapVariable(middleNunChuckDataJoystickXCentered,0,middleNunChuckCalibrationData.joystickXMax - middleNunChuckCalibrationData.joystickXCenter,0,127);
			else
				middleNunChuckData.joystickX = middleNunchuckMapVariable(middleNunChuckDataJoystickXCentered,middleNunChuckCalibrationData.joystickXMin - middleNunChuckCalibrationData.joystickXCenter,-1,-127,0);

			if(middleNunChuckDataJoystickYCentered >= 0)
				middleNunChuckData.joystickY = middleNunchuckMapVariable(middleNunChuckDataJoystickYCentered,0,middleNunChuckCalibrationData.joystickYMax - middleNunChuckCalibrationData.joystickYCenter,0,127);
			else
				middleNunChuckData.joystickY = middleNunchuckMapVariable(middleNunChuckDataJoystickYCentered,middleNunChuckCalibrationData.joystickYMin - middleNunChuckCalibrationData.joystickYCenter,-1,-127,0);
			
			// TODO: Map data to the correct range			
			middleNunChuckData.accelerometerX = middleNunchuckMapVariable((middleNunChuckSensorData.accelerometerX - middleNunChuckCalibrationData.accelero0GX),-(middleNunChuckCalibrationData.accelero1GX-middleNunChuckCalibrationData.accelero0GX)*2,(middleNunChuckCalibrationData.accelero1GX-middleNunChuckCalibrationData.accelero0GX)*2,INT16_MIN,INT16_MAX);
			middleNunChuckData.accelerometerY = middleNunchuckMapVariable((middleNunChuckSensorData.accelerometerY - middleNunChuckCalibrationData.accelero0GY),-(middleNunChuckCalibrationData.accelero1GY-middleNunChuckCalibrationData.accelero0GY)*2,(middleNunChuckCalibrationData.accelero1GY-middleNunChuckCalibrationData.accelero0GY)*2,INT16_MIN,INT16_MAX);
			middleNunChuckData.accelerometerZ = middleNunchuckMapVariable((middleNunChuckSensorData.accelerometerZ - middleNunChuckCalibrationData.accelero0GZ),-(middleNunChuckCalibrationData.accelero1GZ-middleNunChuckCalibrationData.accelero0GZ)*2,(middleNunChuckCalibrationData.accelero1GZ-middleNunChuckCalibrationData.accelero0GZ)*2,INT16_MIN,INT16_MAX);
			
		}else{
			middleNunChuckData.dataValid = false;
			middleNunChuckMeasureInterval = middleNunChuckErrortMeasureInterval;
			middleNunChuckInit();
		}
		
		// If callback is given, call it
		if(middleNunChuckEventFunctionPointer)
			middleNunChuckEventFunctionPointer(middleNunChuckData);													// Also update if data is invalid
	}
}

bool middleNunChuckDataValid(void) {
	return middleNunChuckData.dataValid;
}

void middleNunChuckNewDataEvent(void (*eventFunctionPointer)(middleNunChuckDataStruct newData)) {
	if(eventFunctionPointer)
		middleNunChuckEventFunctionPointer = eventFunctionPointer;
}

void middleNunChuckSetMeasureInterval(uint32_t delayInMS) {
	middleNunChuckMeasureIntervalDesired = delayInMS;
}

int32_t middleNunchuckMapVariable(int32_t inputVariable, int32_t inputLowerLimit, int32_t inputUpperLimit, int32_t outputLowerLimit, int32_t outputUpperLimit) {
	inputVariable = inputVariable < inputLowerLimit ? inputLowerLimit : inputVariable;
	inputVariable = inputVariable > inputUpperLimit ? inputUpperLimit : inputVariable;
	
	return (inputVariable - inputLowerLimit) * (outputUpperLimit - outputLowerLimit) / (inputUpperLimit - inputLowerLimit) + outputLowerLimit;
}
