#include "driverSWLAN9252.h"

void (*driverSWLAN9252TimerHandler)(void);
void (*driverSWLAN9252IRQHandler)(void);
void (*driverSWLAN9252SYNC0Handler)(void);
void (*driverSWLAN9252SYNC1Handler)(void);

volatile uint8_t RunLedje;
volatile uint8_t ErrorLedje;

void PDI_Init(void) {
	driverHWSPI1Init(); 
	
	driverSWLAN9252TimerHandler = 0;																							// Init to point to nothing
	driverSWLAN9252IRQHandler = 0;
	driverSWLAN9252SYNC0Handler = 0;
	driverSWLAN9252SYNC1Handler = 0;
}

void SPIWritePDRamRegister(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count) {
	UINT64_VAL param32_1 = {0};
	uint8_t i = 0,nlength, nBytePosition,nWrtSpcAvlCount;

	/*Reset or Abort any previous commands.*/
	param32_1.Val = PRAM_RW_ABORT_MASK;                                                

	SPIWriteDWord(PRAM_WRITE_CMD_REG, param32_1.Val);

	/*Make sure there is no previous write is pending
	(PRAM Write Busy) bit is a 0 */
	do{
		param32_1.Val = SPIReadDWord (PRAM_WRITE_CMD_REG);
	}while((param32_1.v[3] & PRAM_RW_BUSY_8B));

	/*Write Address and Length Register (ECAT_PRAM_WR_ADDR_LEN) with the
	starting uint8_t address and length) and write to the EtherCAT Process RAM Write Command Register (ECAT_PRAM_WR_CMD) with the  PRAM Write Busy
	(PRAM_WRITE_BUSY) bit set*/
	param32_1.w[0] = Address;
	param32_1.w[1] = Count;
	param32_1.w[2] = 0x0;
	param32_1.w[3] = 0x8000;
    
  SPIWriteBytes (PRAM_WRITE_ADDR_LEN_REG, (uint8_t*)&param32_1.Val,8); // TODO: check if this works

	/*Read PRAM write Data Available (PRAM_READ_AVAIL) bit is set*/
	do {
		param32_1.Val = SPIReadDWord (PRAM_WRITE_CMD_REG);
	}while(!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

	/*Check write data available count*/
	nWrtSpcAvlCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

	/*Write data to Write FIFO */ 
	/*get the byte lenth for first read*/
	nBytePosition = (Address & 0x03);

	nlength = (4-nBytePosition) > Count ? Count:(4-nBytePosition);

	param32_1.Val = 0;
	memcpy(&param32_1.v[nBytePosition],WriteBuffer+i, nlength);

	SPIWriteDWord (PRAM_WRITE_FIFO_REG,param32_1.Val);

	nWrtSpcAvlCount--;
	Count-=nlength;
	i+=nlength;

	driverHWSPI1SetCS(GPIO_PIN_RESET);
	SPIWriteByte(CMD_SERIAL_WRITE);																								// Auto increment mode
	SPISendAddr(PRAM_WRITE_FIFO_REG);																							// Write Command	

	while(Count) {
		nlength = Count > 4 ? 4 : Count;
		param32_1.Val = 0;
		memcpy(&param32_1, (WriteBuffer+i), nlength);

		SPIWriteBurstMode (param32_1.Val);
		i+=nlength;
		Count-=nlength;
		nWrtSpcAvlCount--;
	}

	driverHWSPI1SetCS(GPIO_PIN_SET);
}

void SPIReadPDRamRegister(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count) {
	UINT64_VAL param32_1 = {0};
	uint8_t i = 0,nlength, nBytePosition;
	uint8_t nReadSpaceAvblCount;

	// Reset/Abort any previous commands.
	param32_1.Val = PRAM_RW_ABORT_MASK;                                                 

	SPIWriteDWord(PRAM_READ_CMD_REG, param32_1.Val);

	/*The host should not modify this field unless the PRAM Read Busy (PRAM_READ_BUSY) bit is a 0.*/
	do{
			param32_1.Val = SPIReadDWord (PRAM_READ_CMD_REG);
	}while((param32_1.v[3] & PRAM_RW_BUSY_8B));

	/*Write Address and Length Register (PRAM_READ_ADDR_LEN) with the
	starting uint8_t address and length) and Set PRAM Read Busy (PRAM_READ_BUSY) bit(-EtherCAT Process RAM Read Command Register)
	to start read operatrion*/
	param32_1.w[0] = Address;
	param32_1.w[1] = Count;
	param32_1.w[2] = 0x0;
	param32_1.w[3] = 0x8000;

	SPIWriteBytes(PRAM_READ_ADDR_LEN_REG, (uint8_t*)&param32_1.Val,8);   

	/*Read PRAM Read Data Available (PRAM_READ_AVAIL) bit is set*/
	do {
			param32_1.Val = SPIReadDWord (PRAM_READ_CMD_REG);
	}while(!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

	nReadSpaceAvblCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

	/*Fifo registers are aliased address. In indexed it will read indexed data reg 0x04, but it will point to reg 0
	 In other modes read 0x04 FIFO register since all registers are aliased*/

	/*get the uint8_t lenth for first read*/
	//Auto increment is supported in SPIO
	param32_1.Val = SPIReadDWord (PRAM_READ_FIFO_REG);
	nReadSpaceAvblCount--;
	nBytePosition = (Address & 0x03);
	nlength = (4-nBytePosition) > Count ? Count:(4-nBytePosition);
	memcpy(ReadBuffer+i ,&param32_1.v[nBytePosition],nlength);
	Count-=nlength;
	i+=nlength;

	driverHWSPI1SetCS(GPIO_PIN_RESET);																						//Lets do it in auto increment mode
	SPIWriteByte(CMD_FAST_READ);																									// Write Command	
	SPISendAddr(PRAM_READ_FIFO_REG);
	SPIWriteByte(CMD_FAST_READ_DUMMY);																						// Dummy Byte	

	while(Count) {
		param32_1.Val = SPIReadBurstMode ();

		nlength = Count > 4 ? 4 : Count;
		memcpy((ReadBuffer+i) ,&param32_1,nlength);

		i += nlength;
		Count -= nlength;
		nReadSpaceAvblCount--;
	}

	driverHWSPI1SetCS(GPIO_PIN_SET);
}

void SPIReadRegUsingCSR(uint8_t *ReadBuffer, uint16_t Address, uint8_t Count) {
	UINT32_VAL param32_1 = {0};
	uint8_t i = 0;
	UINT16_VAL wAddr;
	wAddr.Val = Address;

	param32_1.v[0] = wAddr.byte.LB;
	param32_1.v[1] = wAddr.byte.HB;
	param32_1.v[2] = Count;
	param32_1.v[3] = ESC_READ_BYTE;

	SPIWriteDWord(ESC_CSR_CMD_REG, param32_1.Val);

	do {
		param32_1.Val = SPIReadDWord (ESC_CSR_CMD_REG);
	}while(param32_1.v[3] & ESC_CSR_BUSY);

	param32_1.Val = SPIReadDWord(ESC_CSR_DATA_REG);

	for(i=0;i<Count;i++)
		ReadBuffer[i] = param32_1.v[i];
}

void SPIWriteRegUsingCSR( uint8_t *WriteBuffer, uint16_t Address, uint8_t Count) {
	UINT32_VAL param32_1 = {0};
	uint8_t i = 0;
	UINT16_VAL wAddr;

	for(i=0;i<Count;i++)
			 param32_1.v[i] = WriteBuffer[i];

	SPIWriteDWord(ESC_CSR_DATA_REG, param32_1.Val);

	wAddr.Val = Address;
	param32_1.v[0] = wAddr.byte.LB;
	param32_1.v[1] = wAddr.byte.HB;
	param32_1.v[2] = Count;
	param32_1.v[3] = ESC_WRITE_BYTE;

	SPIWriteDWord (0x304, param32_1.Val);
	
	do {
			param32_1.Val = SPIReadDWord (0x304);
	}while(param32_1.v[3] & ESC_CSR_BUSY);
}

void SPIWriteDWord(uint16_t Address, uint32_t Val) { // Checked, OK
	UINT32_VAL dwData;
	UINT16_VAL wAddr;

	wAddr.Val  = Address;
	dwData.Val = Val;

	uint8_t writeBytes[7] = {CMD_SERIAL_WRITE, wAddr.byte.HB, wAddr.byte.LB, dwData.byte.LB, dwData.byte.HB, dwData.byte.UB, dwData.byte.MB};

	driverHWSPI1SetCS(GPIO_PIN_RESET);				// CS Low
	driverHWSPI1Write(writeBytes,7);
	driverHWSPI1SetCS(GPIO_PIN_SET);					// CS Hight
}

uint32_t SPIReadDWord (uint16_t Address) { 	// Checked, OK
	UINT32_VAL dwResult;
	UINT16_VAL wAddr;
	
	wAddr.Val  = Address;
	uint8_t writeBytes[] = {CMD_SERIAL_READ, wAddr.byte.HB, wAddr.byte.LB};
	
	driverHWSPI1SetCS(GPIO_PIN_RESET);				// CS Low
	driverHWSPI1WriteRead(writeBytes, 3,(uint8_t *)&dwResult, 4);
	driverHWSPI1SetCS(GPIO_PIN_SET);					// CS Hight

	return dwResult.Val;
}

void SPIWriteBurstMode(uint32_t Val) {			// Checked, OK
	UINT32_VAL dwData;
	dwData.Val = Val;

	//Write the Bytes
	uint8_t writeBytes[4] = {dwData.byte.LB, dwData.byte.HB, dwData.byte.UB, dwData.byte.MB};
	driverHWSPI1Write(writeBytes,4);
}

uint32_t SPIReadBurstMode(void) {						// Checked, OK
	UINT32_VAL dwResult;
	uint8_t readBytes[4];
	memset(readBytes,0x00,4);
	
	driverHWSPI1WriteRead(NULL,0x00,readBytes,4);
	
	//Read Bytes
	dwResult.byte.LB = readBytes[0];
	dwResult.byte.HB = readBytes[1];
	dwResult.byte.UB = readBytes[2];
	dwResult.byte.MB = readBytes[3];
	
	return dwResult.Val;
}

void SPIWriteByte(uint8_t data) {						// Checked, OK
	driverHWSPI1WriteSingleByte(data);
}


void SPISendAddr(uint16_t Address) {				// Checked, OK
	UINT16_VAL wAddr;
	wAddr.Val  = Address;
	uint8_t writeBytes[2] = {wAddr.byte.HB, wAddr.byte.LB};
	
	driverHWSPI1Write(writeBytes,2);
}

void SPIWriteBytes(uint16_t Address, uint8_t *Val, uint8_t nLenght) { // Checked, OK
	uint8_t *dwData;
	UINT16_VAL wAddr;
	wAddr.Val  = Address;
	dwData = Val;

	uint8_t writeAddresBytes[3] = {CMD_SERIAL_WRITE, wAddr.byte.HB|ADDRESS_AUTO_INCREMENT, wAddr.byte.LB};
	
	driverHWSPI1SetCS(GPIO_PIN_RESET);				// CS Low
	driverHWSPI1Write(writeAddresBytes,3);	
	driverHWSPI1Write(dwData,nLenght);
	driverHWSPI1SetCS(GPIO_PIN_SET);					// CS High
}

void PDIReadReg(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count) {
	if (Address >= 0x1000)
	 SPIReadPDRamRegister(ReadBuffer, Address,Count);
	else
	 SPIReadRegUsingCSR(ReadBuffer, Address,Count);
}

void PDIWriteReg(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count) {	
	if (Address >= 0x1000)
		SPIWritePDRamRegister(WriteBuffer, Address,Count);
	else
		SPIWriteRegUsingCSR(WriteBuffer, Address,Count); 
}

uint16_t PDIReadAlEventReg(void) {
	uint16_t eventRegister;
	// Read even register
	SPIReadRegUsingCSR((uint8_t *)&eventRegister,0x220, 2);
	
	return eventRegister;
}

uint32_t PDIReadLAN9252DirectReg(uint16_t Address) {   
	uint32_t data;
	data = SPIReadDWord (Address);
	return data;
}

void PDIWriteLAN9252DirectReg(uint32_t Val, uint16_t Address) {
  SPIWriteDWord (Address, Val);
}

uint32_t PDI_GetTimer(void) {
	return driverHWECATTickTimerGetTimerValue();
}

void PDI_ClearTimer(void) {
	driverHWECATTickTimerReset();
}

void PDI_UpdateLED(uint8_t RunLed,uint8_t ErrLed) {	
	RunLedje = RunLed;
	ErrorLedje = ErrLed;
	// Update the LED's - RUN LED is already on the ESC - Error LED maybe in future version?
}

void PDI_Enable_Global_interrupt(void) {
	// TODO: This desires to disable all interrupts, but why?
	driverHWECATTickTimerInterruptEnable(true);
	driverHWLANInterruptEnable(LAN_IRQ_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC0_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC1_Pin,true);
}

void PDI_Init_Timer_Interrupt(void) {
	driverHWECATTickTimerInit();
	driverHWECATTickTimerBindTickFunction(driverSWLAN9252TimerHandler);
	driverHWECATTickTimerInterruptEnable(true);
}

void PDI_Init_IRQ_Interrupt(void) {
	driverHWLANInterruptInit();
	driverHWLANInterruptIRQBindFunction(driverSWLAN9252IRQHandler);
	driverHWLANInterruptEnable(LAN_IRQ_Pin,true);
}

void PDI_Init_SYNC_Interrupts(void) {
	driverHWLANInterruptInit();
	driverHWLANInterruptSYNC0BindFunction(driverSWLAN9252SYNC0Handler);
	driverHWLANInterruptSYNC1BindFunction(driverSWLAN9252SYNC1Handler);
	driverHWLANInterruptEnable(LAN_SYNC0_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC1_Pin,true);
}

void PDI_BindISR_Timer(void (*callbackFunction)(void)) {
	driverSWLAN9252TimerHandler = callbackFunction;
}

void PDI_BindISR_IRQ(void (*callbackFunction)(void)) {
	driverSWLAN9252IRQHandler = callbackFunction;
}

void PDI_BindISR_SYNC0(void (*callbackFunction)(void)) {
	driverSWLAN9252SYNC0Handler = callbackFunction;
}

void PDI_BindISR_SYNC1(void (*callbackFunction)(void)) {
	driverSWLAN9252SYNC0Handler = callbackFunction;
}

void PDI_Restore_Global_Interrupt(uint32_t int_sts) {
	// For now only manage ASIC specific interrupts
	driverHWECATTickTimerInterruptEnable(true);
	driverHWLANInterruptEnable(LAN_IRQ_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC0_Pin,true);
	driverHWLANInterruptEnable(LAN_SYNC1_Pin,true);
}

uint32_t PDI_Disable_Global_Interrupt(void) {
	// For now only manage ASIC specific interrupts
	driverHWECATTickTimerInterruptEnable(false);
	driverHWLANInterruptEnable(LAN_IRQ_Pin,false);
	driverHWLANInterruptEnable(LAN_SYNC0_Pin,false);
	driverHWLANInterruptEnable(LAN_SYNC1_Pin,false);
	return 0; // Return the configuration
}

