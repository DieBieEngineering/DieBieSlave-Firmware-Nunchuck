/**
\addtogroup MCI_HW Parallel ESC Access
@{
*/

/**
\file mcihw.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the interface to the ESC via MCI

\version 5.10

<br>Changes to version V5.01:<br>
V5.10 HW1: Add 32Bit test access during hardware initialization (only for 32Bit controller)<br>
V5.10 HW4: Add volatile directive for direct ESC DWORD/WORD/BYTE access<br>
           Add missing swapping in mcihw.c<br>
           Add "volatile" directive vor dummy vairables in enable and disable SyncManger functions<br>
           Add missing swapping in EL9800hw files<br>
<br>Changes to version V5.0:<br>
V5.01 HW1: Invalid ESC access function was used<br>
<br>Changes to version V4.30:<br>
V5.0 ESC4: Save SM disable/Enable. Operation may be pending due to frame handling.<br>
<br>Changes to version V4.20:<br>
V4.30 ESM: if mailbox SyncManager is disabled and bMbxRunning is true the SyncManger settings need to be revalidate<br>
V4.30 SYNC: change synchronisation control function. Add usage of 0x1C32:12 [SM missed counter].<br>
Calculate bus cycle time (0x1C32:02 ; 0x1C33:02) CalcSMCycleTime()<br>
V4.30 PDO: rename PDO specific functions (COE_xxMapping -> PDO_xxMapping and COE_Application -> ECAT_Application)<br>
V4.20 PCI 1: PC_APPLICATION removed<br>
V4.10 PCI 1: Support for PCI EtherCAT Slave<br>
V4.10 MCI 1: Support for PCI EtherCAT Slave<br>
<br>Changes to version V4.07:<br>
V4.08 ECAT 3: The AlStatusCode is changed as parameter of the function AL_ControlInd<br>
<br>Changes to version V4.06:<br>
V4.07 ECAT 1: The sources for SPI and MCI were merged (in ecat_def.h<br>
                   set the switch MCI_HW to 1 when using the MCI,<br>
                   set the switch SPI_HW (obsolete now EL9800_HW)to 1 when using the SPI<br>
V4.07 COEAPPL 2: The example is working for the NIOS with the evaluation board DBC2C20 V1.2<br>
                       which is available by Altera<br>
<br>Changes to version V3.20:<br>
V4.00 MCI 1: When the MAILBOX_QUEUE-switch was set, the mailbox access<br>
             in boot mode was not working correctly<br>
V4.00 MCI 2: The mailbox data link layer was not working correctly<br>
V4.00 MCI 3: if no outputs are supported, the watchdog has to be triggered by the SM3-Event<br>
V4.00 ECAT 1: The handling of the Sync Manager Parameter was included according to<br>
              the EtherCAT Guidelines and Protocol Enhancements Specification<br>
V4.00 APPL 1: The watchdog checking should be done by a microcontroller<br>
                 timer because the watchdog trigger of the ESC will be reset too<br>
                 if only a part of the sync manager data is written<br>
V4.00 APPL 4: The EEPROM access through the ESC is added
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"
#if MCI_HW

#define    _MCIHW_ 1
#include "mcihw.h"
#undef _MCIHW_
#define    _MCIHW_ 0

#include "ecatslv.h"
#include "ecatappl.h"

/*--------------------------------------------------------------------------------------
------
------    local Types and Defines
------
--------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
TSYNCMAN    TmpSyncMan;

/*-----------------------------------------------------------------------------------------
------
------    local functions
------
-----------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
------
------    functions
------
-----------------------------------------------------------------------------------------*/



/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if initialization was successful

 \brief    This function initialize the EtherCAT Slave Interface.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 HW_Init(void)
{
    
    UINT32 intMask = 0;
    /* the memory interface to the ESC, the ESC-interrupt and the ECAT-timer for the
       watchdog monitoring should be initialized here microcontroller specific*/
    /* initialize ESC DPRAM pointer microcontroller specific to the beginning of the physical memory of the ESC,
       the macro MAKE_PTR_TO_ESC should be defined in mcihw.h */
     pEsc = MAKE_PTR_TO_ESC;

    /* we have to wait here, until the ESC is started */
#if ESC_32BIT_ACCESS
    {
    UINT32 u16PdiCtrl = 0;

    do
    {
        HW_EscReadDWord(u16PdiCtrl,ESC_PDI_CONTROL_OFFSET);
        u16PdiCtrl = SWAPDWORD(u16PdiCtrl);

    } while (((u16PdiCtrl & 0xFF) < 0x8) || ((u16PdiCtrl & 0xFF) > 0xD) );
    }
#else
    {
    UINT16 u16PdiCtrl = 0;

    do
    {
        HW_EscReadWord(u16PdiCtrl,ESC_PDI_CONTROL_OFFSET);
        u16PdiCtrl = SWAPWORD(u16PdiCtrl);

    } while (((u16PdiCtrl & 0xFF) < 0x8) || ((u16PdiCtrl & 0xFF) > 0xD) );
    }
#endif


#if AL_EVENT_ENABLED
    /* initialize the PDI - interrupt source*/
    INIT_ESC_INT;

    /* initialize the AL_Event Mask Register */
    /* the AL Event-Mask register is initialized with 0, so that no ESC interrupt is generated yet,
       the AL Event-Mask register will be adapted in the function StartInputHandler in ecatslv.c
        when the state transition from PREOP to SAFEOP is made */
    HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);

    /* enable the ESC-interrupt microcontroller specific,
        the macro ENABLE_ESC_INT should be defined in ecat_def.h */
    ENABLE_ESC_INT();
#else
    /* initialize the AL_Event Mask Register */
    HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
#endif

    return 0;
}

void HW_Release(void)
{
}

#if BOOTSTRAPMODE_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function resets the hardware
*////////////////////////////////////////////////////////////////////////////////////////

void     HW_RestartTarget(void)
{
}
#endif /* BOOTSTRAPMODE_SUPPORTED */

#if ESC_EEPROM_EMULATION
/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if reload was successful

 \brief    This function is called when the master has request an EEPROM reload during EEPROM emulation

*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_EepromReload (void)
{
    return 0;
}
#endif

#if AL_EVENT_ENABLED
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    Interrupt service routine for the interrupts from the EtherCAT Slave Controller
*////////////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
interrupt
#endif
void HW_EcatIsr(void)
{
    PDI_Isr();
}

#endif
#if ECAT_TIMER_INT
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    Timer interrupt service routine which will shall be called every ms
*////////////////////////////////////////////////////////////////////////////////////////

/* header of the ISR shall be defined in mcihw.h */
TIMER_INT_HEADER
void APPL_1MsTimerIsr(void)
{
    ECAT_CheckTimer();
}

#endif

#if UC_SET_ECAT_LED
void HW_SetLed(BOOL RunLed,BOOL ErrLed)
{

}
#endif //#if UC_SET_ECAT_LED

#endif //#if MCI_HW
/** @} */
