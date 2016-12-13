#ifndef __DRIVERSWLAN9252_H
#define	__DRIVERSWLAN9252_H

#include "stm32f3xx_hal.h"
#include "stdbool.h"
#include "dataHelper.h"
#include "driverHWSPI1.h"
#include "middleLAN9252.h"

#define CMD_SERIAL_READ 0x03
#define CMD_FAST_READ 0x0B
#define CMD_DUAL_OP_READ 0x3B
#define CMD_DUAL_IO_READ 0xBB
#define CMD_QUAD_OP_READ 0x6B
#define CMD_QUAD_IO_READ 0xEB
#define CMD_SERIAL_WRITE 0x02
#define CMD_DUAL_DATA_WRITE 0x32
#define CMD_DUAL_ADDR_DATA_WRITE 0xB2
#define CMD_QUAD_DATA_WRITE 0x62
#define CMD_QUAD_ADDR_DARA_WRITE 0xE2

#define CMD_SERIAL_READ_DUMMY 0
#define CMD_FAST_READ_DUMMY 1
#define CMD_DUAL_OP_READ_DUMMY 1
#define CMD_DUAL_IO_READ_DUMMY 2
#define CMD_QUAD_OP_READ_DUMMY 1
#define CMD_QUAD_IO_READ_DUMMY 4
#define CMD_SERIAL_WRITE_DUMMY 0
#define CMD_DUAL_DATA_WRITE_DUMMY 0
#define CMD_DUAL_ADDR_DATA_WRITE_DUMMY 0
#define CMD_QUAD_DATA_WRITE_DUMMY 0
#define CMD_QUAD_ADDR_DARA_WRITE_DUMMY 0

#define ESC_CSR_CMD_REG		0x304
#define ESC_CSR_DATA_REG	0x300
#define ESC_WRITE_BYTE 		0x80
#define ESC_READ_BYTE 		0xC0
#define ESC_CSR_BUSY		0x80

#define ADDRESS_AUTO_INCREMENT 0x40

// TEMP
#define HW_CFG                  0x0074      // hardware configuration register
#define BYTE_TEST               0x0064      // byte order test register
#define RESET_CTL               0x01F8      // reset register       
#define ID_REV                  0x0050      // chip ID and revision
// TEMP

void driverSWLAN9252Init(void);
void SPIWritePDRamRegister(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count);
void SPIReadPDRamRegister(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count);
void SPIReadRegUsingCSR(uint8_t *ReadBuffer, uint16_t Address, uint8_t Count);
void SPIWriteRegUsingCSR( uint8_t *WriteBuffer, uint16_t Address, uint8_t Count);
void SPIWriteDWord (uint16_t Address, uint32_t Val);
uint32_t SPIReadDWord (uint16_t Address);
void SPIWriteBurstMode (uint32_t Val);
uint32_t SPIReadBurstMode (void);
void SPISendAddr (uint16_t Address);
void SPIReadDRegister(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count);
void SPIWriteRegister(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count);

#endif	/* __DRIVERSWLAN9252_H */

