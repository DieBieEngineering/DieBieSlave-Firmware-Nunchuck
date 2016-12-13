/**
 * \addtogroup EL9800_HW EL9800 Platform (Serial ESC Access)
 * @{
 */

/**
\file el9800hw.h
\author EthercatSSC@beckhoff.com
\brief Defines to access the EL9800 specific periphery and ESC (via SPI)

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _EL9800HW_H_
#define _EL9800HW_H_


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include  "esc.h"

#if _PIC18
#include <pic18.h>
#elif _PIC24

#include <p24Hxxxx.h>
#endif


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
#define ESC_RD                    0x02 /**< \brief Indicates a read access to ESC or EEPROM*/
#define ESC_WR                    0x04 /**< \brief Indicates a write access to ESC or EEPROM.*/


/*---------------------------------------------
-    Microcontroller definitions
-----------------------------------------------*/
#if    defined(_18F242) || defined(_18F252) || defined(_18F442) || defined(_18F452)
#define  ADREG                ADCON1
#define  ADREG_ALL_DIG_IO     0x07
#elif    defined(_18F2420) || defined(_18F2520) || defined(_18F4420) || defined(_18F4520)
#define  ADREG                ADCON1
#define  ADREG_ALL_DIG_IO     0x0F
#elif    defined(_18F23K20) || defined(_18F24K20) || defined(_18F25K20) || defined(_18F43K20) || defined(_18F44K20) || defined(_18F45K20)
#define  ADREG                ANSEL
#define  ADREG_ALL_DIG_IO     0x00
#endif


#if _PIC24

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

#elif _PIC18

#define PORT_CFG            {PORTB = 0x00; TRISB = 0x0D; PORTC = 0x00; TRISC = 0x10; TRISD = 0x0F; PORTD = 0x00;}
#define SWITCH_1            RD0 /**< \brief Access to switch 1 input*/
#define SWITCH_2            RD1 /**< \brief Access to switch 2 input*/
#define SWITCH_3            RD2 /**< \brief Access to switch 3 input*/
#define SWITCH_4            RD3 /**< \brief Access to switch 4 input*/
#define LED_1               RD4 /**< \brief Access to led 1 output*/
#define LED_2               RD5 /**< \brief Access to led 2 output*/
#define LED_3               RD6 /**< \brief Access to led 3 output*/
#define LED_4               RD7 /**< \brief Access to led 4 output*/

#endif



/*---------------------------------------------
-    hardware timer settings
-----------------------------------------------*/
#if _PIC24

#define ECAT_TIMER_INC_P_MS                0x271 /**< \brief 625 ticks per ms*/

#elif _PIC18

#define ECAT_TIMER_INC_P_MS                0x15F9 /**< \brief 5628 ticks per ms*/

#endif


/*---------------------------------------------
-    Interrupt and Timer defines
-----------------------------------------------*/
#if _PIC24

#ifndef DISABLE_ESC_INT
#define    DISABLE_ESC_INT()            {(_INT1IE)=0;} /**< \brief Disable interrupt source INT1*/
#endif
#ifndef ENABLE_ESC_INT
#define    ENABLE_ESC_INT()            {(_INT1IE)=1;} /**< \brief Enable interrupt source INT1*/
#endif

#ifndef HW_GetTimer
#define HW_GetTimer()        ((UINT16)(TMR7)) /**< \brief Access to the hardware timer*/
#endif

#ifndef HW_ClearTimer
#define HW_ClearTimer()        {(TMR7) = 0;} /**< \brief Clear the hardware timer*/
#endif


#elif _PIC18

#ifndef DISABLE_ESC_INT
#define    DISABLE_ESC_INT()                (INT0IE)=0 /**< \brief Disable interrupt source INT0*/
#endif
#ifndef ENABLE_ESC_INT
#define    ENABLE_ESC_INT()                (INT0IE)=1 /**< \brief Enable interrupt source INT0*/
#endif

#ifndef HW_GetTimer
#define HW_GetTimer()        ((UINT16)TMR3) /**< \brief Access to the hardware timer*/
#endif

#ifndef HW_ClearTimer
#define HW_ClearTimer()        {(TMR3) = 0;} /**< \brief Clear the hardware timer*/
#endif

#endif //#elif _PIC18

#if !INTERRUPTS_SUPPORTED
#define HW_GetALEventRegister_Isr HW_GetALEventRegister /**< \brief No interrupts are supported so the default Get AL Event register (0x220) function is used*/
#endif


#define HW_EscReadWord(WordValue, Address) HW_EscRead(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief 16Bit ESC read access*/
#define HW_EscReadDWord(DWordValue, Address) HW_EscRead(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief 32Bit ESC read access*/
#if !ESC_16BIT_ACCESS
#define HW_EscReadByte(ByteValue, Address) HW_EscRead(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief 8Bit ESC read access*/
#endif
#define HW_EscReadMbxMem(pData,Address,Len) HW_EscRead(((MEM_ADDR *)(pData)),((UINT16)(Address)),(Len)) /**< \brief The mailbox data is stored in the local uC memory therefore the default read function is used.*/

#if INTERRUPTS_SUPPORTED
#define HW_EscReadWordIsr(WordValue, Address) HW_EscReadIsr(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief Interrupt specific 16Bit ESC read access*/
#define HW_EscReadDWordIsr(DWordValue, Address) HW_EscReadIsr(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief Interrupt specific 32Bit ESC read access*/
#if !ESC_16BIT_ACCESS
#define HW_EscReadByteIsr(ByteValue, Address) HW_EscReadIsr(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief Interrupt specific 8Bit ESC read access*/
#endif
#else
#define HW_EscReadIsr(pData, Address, Len ) HW_EscRead(((MEM_ADDR *)(pData)),((UINT16)(Address)),(Len)) /**< \brief No Interrupts are supported so the generic ESC write access function is used*/
#define HW_EscReadWordIsr(WordValue, Address) HW_EscRead(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief No Interrupts are used so the generic (16Bit) ESC read access function is used*/
#define HW_EscReadDWordIsr(DWordValue, Address) HW_EscRead(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief No Interrupts are used so the generic (32Bit) ESC read access function is used*/
#if !ESC_16BIT_ACCESS
#define HW_EscReadByteIsr(ByteValue, Address) HW_EscRead(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief No Interrupts are used so the generic (8Bit) ESC read access function is used*/
#endif //#if !ESC_16BIT_ACCESS
#endif// #if else INTERRUPTS_SUPPORTED


#define HW_EscWriteWord(WordValue, Address) HW_EscWrite(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief 16Bit ESC write access*/
#define HW_EscWriteDWord(DWordValue, Address) HW_EscWrite(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief 32Bit ESC write access*/
#if !ESC_16BIT_ACCESS
#define HW_EscWriteByte(ByteValue, Address) HW_EscWrite(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief 8Bit ESC write access*/
#endif
#define HW_EscWriteMbxMem(pData,Address,Len) HW_EscWrite(((MEM_ADDR *)(pData)),((UINT16)(Address)),(Len)) /**< \brief The mailbox data is stored in the local uC memory therefore the default write function is used.*/

#if INTERRUPTS_SUPPORTED
#define HW_EscWriteWordIsr(WordValue, Address) HW_EscWriteIsr(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief Interrupt specific 16Bit ESC write access*/
#define HW_EscWriteDWordIsr(DWordValue, Address) HW_EscWriteIsr(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief Interrupt specific 32Bit ESC write access*/
#if !ESC_16BIT_ACCESS
#define HW_EscWriteByteIsr(ByteValue, Address) HW_EscWriteIsr(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief Interrupt specific 8Bit ESC write access*/
#endif
#else
#define HW_EscWriteIsr(pData, Address, Len ) HW_EscWrite(((MEM_ADDR *)(pData)),((UINT16)(Address)),(Len)) /**< \brief No Interrupts are supported so the generic ESC write access function is used*/
#define HW_EscWriteWordIsr(WordValue, Address) HW_EscWrite(((MEM_ADDR *)&(WordValue)),((UINT16)(Address)),2) /**< \brief No Interrupts are used so the generic (16Bit) ESC write access function is used*/
#define HW_EscWriteDWordIsr(DWordValue, Address) HW_EscWrite(((MEM_ADDR *)&(DWordValue)),((UINT16)(Address)),4) /**< \brief No Interrupts are used so the generic (32Bit) ESC write access function is used*/
#if !ESC_16BIT_ACCESS
#define HW_EscWriteByteIsr(ByteValue, Address) HW_EscWrite(((MEM_ADDR *)&(ByteValue)),((UINT16)(Address)),1) /**< \brief No Interrupts are used so the generic (8Bit) ESC write access function is used*/
#endif
#endif

#endif //_EL9800HW_H_

/* ECATCHANGE_START(V5.11) ECAT10*/
#if defined(_EL9800HW_) && (_EL9800HW_ == 1)
/* ECATCHANGE_END(V5.11) ECAT10*/
    #define PROTO
#else
    #define PROTO extern
#endif


/*-----------------------------------------------------------------------------------------
------
------    Global variables
------
-----------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
------
------    Global functions
------
-----------------------------------------------------------------------------------------*/
PROTO UINT8 HW_Init(void);
PROTO void HW_Release(void);
PROTO UINT16 HW_GetALEventRegister(void);

#if INTERRUPTS_SUPPORTED
PROTO UINT16 HW_GetALEventRegister_Isr(void);
#endif//#if INTERRUPTS_SUPPORTED

#if UC_SET_ECAT_LED
PROTO void HW_SetLed(UINT8 RunLed,UINT8 ErrLed);
#endif //#if UC_SET_ECAT_LED

PROTO void HW_EscRead( MEM_ADDR * pData, UINT16 Address, UINT16 Len );
#if INTERRUPTS_SUPPORTED
PROTO void HW_EscReadIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len );
#endif

PROTO void HW_EscWrite( MEM_ADDR *pData, UINT16 Address, UINT16 Len );

#if INTERRUPTS_SUPPORTED
PROTO void HW_EscWriteIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len );
#endif

#if BOOTSTRAPMODE_SUPPORTED
PROTO void HW_RestartTarget(void);
#endif

#if ESC_EEPROM_EMULATION
PROTO UINT16 HW_EepromReload (void);
#endif

#undef    PROTO
/** @}*/

