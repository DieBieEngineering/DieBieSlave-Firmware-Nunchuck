/*******************************************************************************
 LAN9252 - Hardware Abtraction Layer header file.

  Company:
    Microchip Technology Inc.

  File Name:
    9252_HW.h

  Description:
    This file contains the defines, function protypes for LAN9252 Hardware Abtraction Layer

  Change History:
    Version		Changes
	0.1			Initial version.
	0.2			-
	0.3			-
	0.4			-
	1.0			-
*******************************************************************************/

/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#ifndef _9252_HW_H_
#define _9252_HW_H_

///////////////////////////////////////////////////////////////////////////////
// Includes

#include  "esc.h"

///////////////////////////////////////////////////////////////////////////////
// Defines, types

#define ESC_RD                    0x02 /**< \brief Indicates a read access to ESC or EEPROM*/
#define ESC_WR                    0x04 /**< \brief Indicates a write access to ESC or EEPROM.*/

#define PORT_CFG            {TRISD = 0xFFFF; TRISB = 0x0008; TRISF = 0xFFCC; TRISG = 0x210C; PORTB = 0x00F4; PORTF = 0x0030; PORTG = 0x1243;}
#define SWITCH_1            PORTDbits.RD7 /**< \brief Access to switch 1 input*/
#define SWITCH_2            PORTDbits.RD6 /**< \brief Access to switch 2 input*/
#define SWITCH_3            PORTDbits.RD5 /**< \brief Access to switch 3 input*/
#define SWITCH_4            PORTDbits.RD4 /**< \brief Access to switch 4 input*/
#define SWITCH_5            PORTDbits.RD3 /**< \brief Access to switch 5 input*/
#define SWITCH_6            PORTDbits.RD2 /**< \brief Access to switch 6 input*/
#define SWITCH_7            PORTDbits.RD1 /**< \brief Access to switch 7 input*/
#define SWITCH_8            PORTDbits.RD0 /**< \brief Access to switch 8 input*/

#define LED_1               LATBbits.LATB8 /**< \brief Access to led 1 output*/
#define LED_2               LATBbits.LATB9 /**< \brief Access to led 2 output*/
#define LED_3               LATBbits.LATB10 /**< \brief Access to led 3 output*/
#define LED_4               LATBbits.LATB11 /**< \brief Access to led 4 output*/
#define LED_5               LATBbits.LATB12 /**< \brief Access to led 5 output*/
#define LED_6               LATBbits.LATB13 /**< \brief Access to led 6 output*/
#define LED_7               LATBbits.LATB14 /**< \brief Access to led 7 output*/
#define LED_8               LATBbits.LATB15 /**< \brief Access to led 8 output*/

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
#define PRAM_RW_BUSY_32B        2147483648/*(1 << 31)*/
#define PRAM_RW_BUSY_8B         (1 << 7)
#define PRAM_SET_READ           (1 << 6)
#define PRAM_SET_WRITE          0

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Hardware timer settings
#define ECAT_TIMER_INC_P_MS              312 /**< \brief 312 ticks per ms*/


///////////////////////////////////////////////////////////////////////////////
// Interrupt and Timer Defines

#ifndef DISABLE_ESC_INT
    #define    DISABLE_ESC_INT()          // {(IEC0bits.INT0IE)=0;} /**< \brief Disable interrupt source INT1*/
#endif
#ifndef ENABLE_ESC_INT
    #define    ENABLE_ESC_INT()           //{(IEC0bits.INT0IE)=1;} /**< \brief Enable interrupt source INT1*/
#endif

//TODO
#ifndef HW_GetTimer
	#define HW_GetTimer()       0//((UINT16)(TMR5)) < \brief Access to the hardware timer TODO: Implement passing the timer value
#endif

#ifndef HW_ClearTimer
	#define HW_ClearTimer()       //{(TMR5) = 0;} < \brief Clear the hardware timer TODO: Implement clearing the timer
#endif

#endif // end of #ifdef PIC32_HW


///////////////////////////////////////////////////////////////////////////////
// Interrupt and Timer defines

#define HW_EscReadWord(WordValue, Address) HW_EscRead(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief 16Bit ESC read access*/
#define HW_EscReadDWord(DWordValue, Address) HW_EscRead(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief 32Bit ESC read access*/
#define HW_EscReadByte(ByteValue, Address) HW_EscRead(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief 8Bit ESC read access*/
#define HW_EscReadMbxMem(pData,Address,Len) HW_EscRead(((MEM_ADDR *)(pData)),((UINT16)(Address)),(Len)) /**< \brief The mailbox data is stored in the local uC memory therefore the default read function is used.*/

#define HW_EscReadWordIsr(WordValue, Address) HW_EscReadIsr(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief Interrupt specific 16Bit ESC read access*/
#define HW_EscReadDWordIsr(DWordValue, Address) HW_EscReadIsr(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief Interrupt specific 32Bit ESC read access*/
#define HW_EscReadByteIsr(ByteValue, Address) HW_EscReadIsr(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief Interrupt specific 8Bit ESC read access*/


#define HW_EscWriteWord(WordValue, Address) HW_EscWrite(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief 16Bit ESC write access*/
#define HW_EscWriteDWord(DWordValue, Address) HW_EscWrite(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief 32Bit ESC write access*/
#define HW_EscWriteByte(ByteValue, Address) HW_EscWrite(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief 8Bit ESC write access*/
#define HW_EscWriteMbxMem(pData,Address,Len) HW_EscWrite(((MEM_ADDR *)(pData)),((UINT16)(Address)),(Len)) /**< \brief The mailbox data is stored in the local uC memory therefore the default write function is used.*/

#define HW_EscWriteWordIsr(WordValue, Address) HW_EscWriteIsr(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief Interrupt specific 16Bit ESC write access*/
#define HW_EscWriteDWordIsr(DWordValue, Address) HW_EscWriteIsr(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief Interrupt specific 32Bit ESC write access*/
#define HW_EscWriteByteIsr(ByteValue, Address) HW_EscWriteIsr(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief Interrupt specific 8Bit ESC write access*/


UINT8 HW_Init(void);
void HW_Release(void);
UINT16 HW_GetALEventRegister(void);
UINT16 HW_GetALEventRegister_Isr(void);
void HW_ResetALEventMask(UINT16 intMask);
void HW_SetALEventMask(UINT16 intMask);
void HW_EscRead( MEM_ADDR * pData, UINT16 Address, UINT16 Len );
void HW_EscReadIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len );
void HW_EscWrite( MEM_ADDR *pData, UINT16 Address, UINT16 Len );
void HW_EscWriteIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len );
void HW_DisableSyncManChannel(UINT8 channel);
void HW_EnableSyncManChannel(UINT8 channel);
TSYNCMAN ESCMEM *HW_GetSyncMan(UINT8 channel);
void HW_SetLed(UINT8 RunLed,UINT8 ErrLed);
