/**
 * \addtogroup FC1100HW FC1100 Hardware Access
 * @{
 */

/**
\file fc1100hw.h
\brief Specific functions for FC1100 hardware

\version 5.01

<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _FC1100HW_H_

#define _FC1100HW_H_

/*-----------------------------------------------------------------------------------------
------
------    Include
------
-----------------------------------------------------------------------------------------*/
#include "esc.h"

/*-----------------------------------------------------------------------------------------
------
------    Define and Typedef
------
-----------------------------------------------------------------------------------------*/
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     /**< \brief Specifies the minimum required Windows platform (Vista)
                                    *
                                    * Change this to the appropriate value to target other versions of Windows.*/
#endif



#if (AL_EVENT_ENABLED == 1)
#define DISABLE_AL_EVENT_INT
#define ENABLE_AL_EVENT_INT
#define DISABLE_ESC_INT()
#define ENABLE_ESC_INT()
#endif


#define     HW_GetALEventRegister()             ((((UINT16 ESCMEM *)pEsc)[((ESC_AL_EVENT_OFFSET)>>1)])) /**< \brief Returns the first 16Bit of the AL Event register (0x220)*/
#define     HW_GetALEventRegister_Isr           HW_GetALEventRegister /**< \brief Returns the first 16Bit of the AL Event register (0x220).<br>Called from ISRs.*/
#if ESC_32BIT_ACCESS
#define HW_EscRead(pData,Address,Len) ESCMEMCPY((MEM_ADDR *)(pData), &((UINT32 *) pEsc)[(Address)>>2], (Len)) /**< \brief Generic ESC (register and DPRAM) read access.*/
#elif ESC_16BIT_ACCESS
#define HW_EscRead(pData,Address,Len)  ESCMEMCPY((MEM_ADDR *)(pData), &((UINT16 *) pEsc)[(Address)>>1], (Len)) /**< \brief Generic ESC (register and DPRAM) read access.*/
#else
#define HW_EscRead(pData,Address,Len)  ESCMEMCPY((MEM_ADDR *)(pData), &((UINT8 *) pEsc)[(Address)], (Len)) /**< \brief Generic ESC (register and DPRAM) read access.*/
#endif

#define     HW_EscReadIsr                           HW_EscRead /**< \brief Generic ESC (register and DPRAM) read access.<br>Called for ISRs.*/
#define     HW_EscReadDWord(DWordValue, Address)    ((DWordValue) = (UINT32)(((UINT32 *)pEsc)[(Address>>2)])) /**< \brief 32Bit specific ESC (register and DPRAM) read access.*/
#define     HW_EscReadDWordIsr(DWordValue, Address) ((DWordValue) = (UINT32)(((UINT32 *)pEsc)[(Address>>2)])) /**< \brief 32Bit specific ESC (register and DPRAM) read access.<br>Called from ISRs.*/
#define     HW_EscReadWord(WordValue, Address)      ((WordValue) = (UINT16)(((UINT16 *)pEsc)[(Address>>1)])) /**< \brief 16Bit specific ESC (register and DPRAM) read access.*/
#define     HW_EscReadWordIsr(WordValue, Address)   ((WordValue) = (UINT16)(((UINT16 *)pEsc)[(Address>>1)])) /**< \brief 16Bit specific ESC (register and DPRAM) read access.<br>Called from ISRs.*/
#define     HW_EscReadByte(ByteValue, Address)      ((ByteValue) = (UINT8)(((UINT8 *)pEsc)[(Address)])) /**< \brief 8Bit specific ESC (register and DPRAM) read access.*/
#define     HW_EscReadByteIsr(ByteValue, Address)   ((ByteValue) = (UINT8)(((UINT8 *)pEsc)[(Address)])) /**< \brief 8Bit specific ESC (register and DPRAM) read access.<br>Called from ISRs.*/
#define     HW_EscReadMbxMem(pData,Address,Len)     HW_EscRead(pData,Address,Len); /**< \brief Macro to copy data from the application mailbox memory(not the ESC memory, this access is handled by HW_EscRead).*/

#if ESC_32BIT_ACCESS
#define HW_EscWrite(pData,Address,Len) ESCMEMCPY(&((UINT32 *) pEsc)[(Address)>>2], (MEM_ADDR *)(pData), (Len)) /**< \brief Generic ESC (register and DPRAM) write access.*/
#elif ESC_16BIT_ACCESS
#define HW_EscWrite(pData,Address,Len) ESCMEMCPY(&((UINT16 *) pEsc)[(Address)>>1], (MEM_ADDR *)(pData), (Len)) /**< \brief Generic ESC (register and DPRAM) write access.*/
#else
#define HW_EscWrite(pData,Address,Len) ESCMEMCPY(&((UINT8 *) pEsc)[(Address)], (MEM_ADDR *)(pData), (Len)); /**< \brief Generic ESC (register and DPRAM) write access.*/
#endif

#define     HW_EscWriteIsr                          HW_EscWrite /**< \brief Generic ESC (register and DPRAM) write access.<br>Called for ISRs.*/
#define     HW_EscWriteDWord(DWordValue, Address)   ((((UINT32 *)pEsc)[(Address>>2)]) = (DWordValue)) /**< \brief 32Bit specific ESC (register and DPRAM) write access.*/
#define     HW_EscWriteDWordIsr(DWordValue, Address)((((UINT32 *)pEsc)[(Address>>2)]) = (DWordValue)) /**< \brief 32Bit specific ESC (register and DPRAM) write access.<br>Called from ISRs.*/
#define     HW_EscWriteWord(WordValue, Address)     ((((UINT16 *)pEsc)[(Address>>1)]) = (WordValue)) /**< \brief 16Bit specific ESC (register and DPRAM) write access.*/
#define     HW_EscWriteWordIsr(WordValue, Address)  ((((UINT16 *)pEsc)[(Address>>1)]) = (WordValue)) /**< \brief 16Bit specific ESC (register and DPRAM) write access.<br>Called from ISRs.*/
#define     HW_EscWriteByte(ByteValue, Address)     ((((UINT8 *)pEsc)[(Address)]) = (ByteValue)) /**< \brief 8Bit specific ESC (register and DPRAM) write access.*/
#define     HW_EscWriteByteIsr(ByteValue, Address)  ((((UINT8 *)pEsc)[(Address)]) = (ByteValue)) /**< \brief 8Bit specific ESC (register and DPRAM) write access.<br>Called from ISRs.*/
#define     HW_EscWriteMbxMem(pData,Address,Len)    HW_EscWrite((MEM_ADDR *)pData,Address,Len) /**< \brief Macro to copy data from the application mailbox memory (not the ESC memory, this access is handled by HW_EscWrite).*/


/*---------------------------------------------
-    Timer
-----------------------------------------------*/
#define ECAT_TIMER_INC_P_MS        0x1 /**< \brief 1 timer tick per second*/

#if ESC_EEPROM_ACCESS_SUPPORT
#define     ESC_RD                    0x02 /**< \brief Indicates a read access to ESC or EEPROM*/
#define     ESC_WR                    0x04 /**< \brief Indicates a write access to ESC or EEPROM.*/
#endif

#endif //_FC1100HW_H_

#ifdef _FC1100_
    #define PROTO
#else
    #define PROTO extern
#endif

 /*-----------------------------------------------------------------------------------------
------
------    Global variables
------
-----------------------------------------------------------------------------------------*/
PROTO MEM_ADDR ESCMEM *            pEsc; /**< \brief Pointer to the ESC*/

/*-----------------------------------------------------------------------------------------
------
------    Global functions
------
-----------------------------------------------------------------------------------------*/

#ifdef _DEBUG
PROTO void DebugMessage (const char* pszFormat, ...);
#else
#define DebugMessage printf
#endif

#if UC_SET_ECAT_LED
PROTO void HW_SetLed(BOOL RunLed,BOOL ErrLed);
#endif //#if UC_SET_ECAT_LED

PROTO UINT16 HW_Init(void);
PROTO void HW_Release(void);


#if BOOTSTRAPMODE_SUPPORTED
PROTO    void HW_RestartTarget(void);
#endif

#if ESC_EEPROM_EMULATION
PROTO UINT16 HW_EepromReload (void);
#endif

PROTO UINT16 HW_GetTimer(void);
PROTO void HW_ClearTimer(void);

#undef PROTO
/** @}*/
