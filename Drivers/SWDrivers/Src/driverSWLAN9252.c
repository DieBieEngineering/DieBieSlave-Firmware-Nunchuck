#include "driverSWLAN9252.h"

void SPIWriteBytes(uint16_t Address, uint8_t *Val, uint8_t nLenght);

void driverSWLAN9252Init(void) {
	driverHWSPI1Init();
}

void SPIWritePDRamRegister(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count) {
	UINT32_VAL param32_1 = {0};
	uint8_t i = 0,nlength, nBytePosition,nWrtSpcAvlCount;

	/*Reset or Abort any previous commands.*/
	param32_1.Val = PRAM_RW_ABORT_MASK;                                                

	SPIWriteDWord (PRAM_WRITE_CMD_REG, param32_1.Val);

	/*Make sure there is no previous write is pending
	(PRAM Write Busy) bit is a 0 */
	do{
			param32_1.Val = SPIReadDWord (PRAM_WRITE_CMD_REG);
	}while((param32_1.v[3] & PRAM_RW_BUSY_8B));

	/*Write Address and Length Register (ECAT_PRAM_WR_ADDR_LEN) with the
	starting UINT8 address and length)*/
	param32_1.w[0] = Address;
	param32_1.w[1] = Count;

	SPIWriteDWord (PRAM_WRITE_ADDR_LEN_REG, param32_1.Val);

	/*write to the EtherCAT Process RAM Write Command Register (ECAT_PRAM_WR_CMD) with the  PRAM Write Busy (PRAM_WRITE_BUSY) bit set*/

	param32_1.Val = PRAM_RW_BUSY_32B; /*TODO:replace with #defines*/

	SPIWriteDWord (PRAM_WRITE_CMD_REG, param32_1.Val);

	/*Read PRAM write Data Available (PRAM_READ_AVAIL) bit is set*/
	do {
		 param32_1.Val = SPIReadDWord (PRAM_WRITE_CMD_REG);
	}while(!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

	/*Check write data available count*/
	nWrtSpcAvlCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

	/*Write data to Write FIFO) */ 
	/*get the byte lenth for first read*/
	nBytePosition = (Address & 0x03);

	nlength = (4-nBytePosition) > Count ? Count:(4-nBytePosition);

	param32_1.Val = 0;
	memcpy(&param32_1.v[nBytePosition],WriteBuffer+i, nlength);

	SPIWriteDWord (PRAM_WRITE_FIFO_REG,param32_1.Val);

	nWrtSpcAvlCount--;
	Count-=nlength;
	i+=nlength;

	//Auto increment mode
	driverHWSPI1SetCS(GPIO_PIN_RESET);

	//Write Command
	uint8_t writeByte[1] = {CMD_SERIAL_WRITE};
	driverHWSPI1Write(writeByte,1);

	SPISendAddr(PRAM_WRITE_FIFO_REG);

	while(Count) {
			nlength = Count > 4 ? 4: Count;
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
	//uint16_t RefAddr = Address;

	/*Reset/Abort any previous commands.*/
	param32_1.Val = PRAM_RW_ABORT_MASK;                                                 

	SPIWriteDWord (PRAM_READ_CMD_REG, param32_1.Val);

	/*The host should not modify this field unless the PRAM Read Busy
	(PRAM_READ_BUSY) bit is a 0.*/
	do
	{
			param32_1.Val = SPIReadDWord (PRAM_READ_CMD_REG);

	}while((param32_1.v[3] & PRAM_RW_BUSY_8B));

	/*Write Address and Length Register (PRAM_READ_ADDR_LEN) with the
	starting UINT8 address and length) and Set PRAM Read Busy (PRAM_READ_BUSY) bit(-EtherCAT Process RAM Read Command Register)
	to start read operatrion*/
	param32_1.w[0] = Address;
	param32_1.w[1] = Count;
	param32_1.w[2] = 0x0;
	param32_1.w[3] = 0x8000;

	SPIWriteBytes (PRAM_READ_ADDR_LEN_REG, (uint8_t*)&param32_1.Val,8);   

	/*Read PRAM Read Data Available (PRAM_READ_AVAIL) bit is set*/
	do {
			param32_1.Val = SPIReadDWord (PRAM_READ_CMD_REG);
	}while(!(param32_1.v[0] & IS_PRAM_SPACE_AVBL_MASK));

	nReadSpaceAvblCount = param32_1.v[1] & PRAM_SPACE_AVBL_COUNT_MASK;

	/*Fifo registers are aliased address. In indexed it will read indexed data reg 0x04, but it will point to reg 0
	 In other modes read 0x04 FIFO register since all registers are aliased*/

	/*get the UINT8 lenth for first read*/
	//Auto increment is supported in SPIO
	param32_1.Val = SPIReadDWord (PRAM_READ_FIFO_REG);
	nReadSpaceAvblCount--;
	nBytePosition = (Address & 0x03);
	nlength = (4-nBytePosition) > Count ? Count:(4-nBytePosition);
	memcpy(ReadBuffer+i ,&param32_1.v[nBytePosition],nlength);
	Count-=nlength;
	i+=nlength;

	//Lets do it in auto increment mode
	driverHWSPI1SetCS(GPIO_PIN_RESET);

	//Write Command
	uint8_t writeByte[1] = {CMD_FAST_READ};
	driverHWSPI1Write(writeByte,1);
	
	SPISendAddr(PRAM_READ_FIFO_REG);

	//Dummy Byte
	writeByte[0] = CMD_FAST_READ_DUMMY;
	driverHWSPI1Write(writeByte,1);

	while(Count) {
			param32_1.Val = SPIReadBurstMode ();

			nlength = Count > 4 ? 4 : Count;
			memcpy((ReadBuffer+i) ,&param32_1,nlength);

			i+=nlength;
			Count-=nlength;
			nReadSpaceAvblCount --;
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

	SPIWriteDWord (ESC_CSR_DATA_REG, param32_1.Val);

	wAddr.Val = Address;
	param32_1.v[0] = wAddr.byte.LB;
	param32_1.v[1] = wAddr.byte.HB;
	param32_1.v[2] = Count;
	param32_1.v[3] = ESC_WRITE_BYTE;

	SPIWriteDWord (0x304, param32_1.Val);
	do {
			param32_1.Val = SPIReadDWord (0x304);
	}while(param32_1.v[3] & ESC_CSR_BUSY);

	return;
}

void SPIWriteDWord(uint16_t Address, uint32_t Val) {
	UINT32_VAL dwData;
	UINT16_VAL wAddr;

	wAddr.Val  = Address;
	dwData.Val = Val;

	driverHWSPI1SetCS(GPIO_PIN_RESET);
	uint8_t writeBytes[7] = {CMD_SERIAL_WRITE, wAddr.byte.HB, wAddr.byte.LB, dwData.byte.LB, dwData.byte.HB, dwData.byte.UB, dwData.byte.MB};
	driverHWSPI1Write(writeBytes,7);
	driverHWSPI1SetCS(GPIO_PIN_SET);
}

uint32_t SPIReadDWord (uint16_t Address) {
	UINT32_VAL dwResult;
	UINT16_VAL wAddr;
	
	wAddr.Val  = Address;
	uint8_t writeBytes[] = {CMD_SERIAL_READ, wAddr.byte.HB, wAddr.byte.LB};
	uint8_t readBytes[4];
	
	driverHWSPI1WriteRead(writeBytes, 3,(uint8_t*) readBytes, 4);

	dwResult.byte.LB = readBytes[0];
	dwResult.byte.HB = readBytes[1];
	dwResult.byte.UB = readBytes[2];
	dwResult.byte.MB = readBytes[3];

	return dwResult.Val;
}

void SPIWriteBurstMode (uint32_t Val) {
	UINT32_VAL dwData;
	dwData.Val = Val;

	uint8_t writeBytes[4] = {dwData.byte.LB, dwData.byte.HB, dwData.byte.UB, dwData.byte.MB};
	driverHWSPI1Write(writeBytes,4);
}

uint32_t SPIReadBurstMode (void) {
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

void SPISendAddr (uint16_t Address) {
	UINT16_VAL wAddr;
	wAddr.Val  = Address;
	uint8_t writeBytes[2] = {wAddr.byte.HB, wAddr.byte.LB};
	
	driverHWSPI1Write(writeBytes,2);
}

void SPIWriteBytes(uint16_t Address, uint8_t *Val, uint8_t nLenght) {
	uint8_t *dwData;
	UINT16_VAL wAddr;
	wAddr.Val  = Address;
	dwData = Val;

	driverHWSPI1SetCS(GPIO_PIN_RESET);
	uint8_t writeAddresBytes[3] = {CMD_SERIAL_WRITE, wAddr.byte.HB|ADDRESS_AUTO_INCREMENT, wAddr.byte.LB};
	driverHWSPI1Write(writeAddresBytes,3);
	driverHWSPI1Write(dwData,nLenght);
	driverHWSPI1SetCS(GPIO_PIN_SET);
}

void SPIReadDRegister(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count) {
  if (Address >= 0x1000)
    SPIReadPDRamRegister(ReadBuffer, Address,Count);
	else
		SPIReadRegUsingCSR(ReadBuffer, Address,Count);
}

void SPIWriteRegister(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count) { 
  if (Address >= 0x1000)
		SPIWritePDRamRegister(WriteBuffer, Address,Count);
	else
		SPIWriteRegUsingCSR(WriteBuffer, Address,Count);
}
