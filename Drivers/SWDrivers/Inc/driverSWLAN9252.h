#ifndef __DRIVERSWLAN9252_H
#define	__DRIVERSWLAN9252_H

#include <stdint.h>
#include <stdbool.h>
#include "dataHelper.h"
#include "driverHWSPI1.h"
#include "driverHWECATTickTimer.h"
#include "driverHWLANInterrupt.h"

///////////////////////////////////////////////////////////////////////////////
//9252 HW DEFINES
#define ECAT_REG_BASE_ADDR              0x0300

#define CSR_DATA_REG_OFFSET             0x00
#define CSR_CMD_REG_OFFSET              0x04
#define PRAM_READ_ADDR_LEN_OFFSET       0x08
#define PRAM_READ_CMD_OFFSET            0x0c
#define PRAM_WRITE_ADDR_LEN_OFFSET      0x10
#define PRAM_WRITE_CMD_OFFSET           0x14

#define PRAM_SPACE_AVBL_COUNT_MASK      0x1f
#define IS_PRAM_SPACE_AVBL_MASK         0x01

#define CSR_DATA_REG                    ECAT_REG_BASE_ADDR+CSR_DATA_REG_OFFSET
#define CSR_CMD_REG                     ECAT_REG_BASE_ADDR+CSR_CMD_REG_OFFSET
#define PRAM_READ_ADDR_LEN_REG          ECAT_REG_BASE_ADDR+PRAM_READ_ADDR_LEN_OFFSET
#define PRAM_READ_CMD_REG               ECAT_REG_BASE_ADDR+PRAM_READ_CMD_OFFSET
#define PRAM_WRITE_ADDR_LEN_REG         ECAT_REG_BASE_ADDR+PRAM_WRITE_ADDR_LEN_OFFSET
#define PRAM_WRITE_CMD_REG              ECAT_REG_BASE_ADDR+PRAM_WRITE_CMD_OFFSET

#define PRAM_READ_FIFO_REG              0x04
#define PRAM_WRITE_FIFO_REG             0x20

#define HBI_INDEXED_DATA0_REG           0x04
#define HBI_INDEXED_DATA1_REG           0x0c
#define HBI_INDEXED_DATA2_REG           0x14

#define HBI_INDEXED_INDEX0_REG          0x00
#define HBI_INDEXED_INDEX1_REG          0x08
#define HBI_INDEXED_INDEX2_REG          0x10

#define HBI_INDEXED_PRAM_READ_WRITE_FIFO    0x18

#define PRAM_RW_ABORT_MASK      (1 << 30)
#define PRAM_RW_BUSY_32B        (1 << 31)
#define PRAM_RW_BUSY_8B         (1 << 7)
#define PRAM_SET_READ           (1 << 6)
#define PRAM_SET_WRITE          0

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

#define LAN9252_BYTE_ORDER_REG          0x64
#define LAN9252_CSR_INT_CONF            0x54
#define LAN9252_CSR_INT_EN              0x5C
#define LAN9252_CSR_INT_STS             0x58

#define ADDRESS_AUTO_INCREMENT 0x40

void PDI_Init(void);
void SPIWritePDRamRegister(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count);
void SPIReadPDRamRegister(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count);
void SPIReadRegUsingCSR(uint8_t *ReadBuffer, uint16_t Address, uint8_t Count);
void SPIWriteRegUsingCSR( uint8_t *WriteBuffer, uint16_t Address, uint8_t Count);
void SPIWriteDWord (uint16_t Address, uint32_t Val);
uint32_t SPIReadDWord (uint16_t Address);
void SPIWriteBurstMode (uint32_t Val);
uint32_t SPIReadBurstMode (void);
void SPIWriteByte(uint8_t data);
void SPISendAddr (uint16_t Address);
void SPIWriteBytes(uint16_t Address, uint8_t *Val, uint8_t nLenght);
void PDIReadReg(uint8_t *ReadBuffer, uint16_t Address, uint16_t Count);
void PDIWriteReg(uint8_t *WriteBuffer, uint16_t Address, uint16_t Count);
uint16_t PDIReadAlEventReg(void);
uint32_t PDIReadLAN9252DirectReg(uint16_t Address);
void PDIWriteLAN9252DirectReg(uint32_t Val, uint16_t Address);
uint32_t PDI_GetTimer(void);
void PDI_ClearTimer(void);
void PDI_UpdateLED(uint8_t RunLed,uint8_t ErrLed);
void PDI_Init_Timer_Interrupt(void);
void PDI_Init_IRQ_Interrupt(void);
void PDI_Init_SYNC_Interrupts(void);
void PDI_BindISR_Timer(void (*callbackFunction)(void));
void PDI_BindISR_IRQ(void (*callbackFunction)(void));
void PDI_BindISR_SYNC0(void (*callbackFunction)(void));
void PDI_BindISR_SYNC1(void (*callbackFunction)(void));
void PDI_Enable_Global_interrupt(void);
void PDI_Restore_Global_Interrupt(uint32_t int_sts);
uint32_t PDI_Disable_Global_Interrupt(void);

#endif	/* __DRIVERSWLAN9252_H */

