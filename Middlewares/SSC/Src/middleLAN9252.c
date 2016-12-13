/*******************************************************************************
 LAN9252 Hardware Abtraction Layer - Implementation file

  Company:
    Microchip Technology Inc.

  File Name:
    9252_HW.c

  Description:
    This file  cContains the functional implementation of LAN9252 Hardware Abtraction Layer
	
  Change History:
    Version		Changes
	0.1			Initial version.
	0.2			-
	0.3			-
	0.4			*Disabled Sync Manager & Application Layer Event Requests.
				*Commented out the ISR call backs related to Sync Manager & AL Event Request.
	1.0			*Enabled Sync Manager & Application Layer Event Requests.
				*Added ISR call backs related to Sync Manager & AL Event Request.
*******************************************************************************/


#include "ecat_def.h"
#include "ecatslv.h"

#define  _9252_HW_ 1
#include "middleLAN9252.h"
#include "driverSWLAN9252.h"

#undef    _9252_HW_
#define   _9252_HW_ 0

#include "ecatappl.h"

#define SYNC0_ACTIVE_LOW         0x01
#define SYNC1_ACTIVE_LOW         0x01

#ifndef    PIC32_HW
    #define PIC32_HW	// Use PIC32MX board
#endif

///////////////////////////////////////////////////////////////////////////////
// Internal Type Defines

typedef union
{
    unsigned short    Word;
    unsigned char    Byte[2];
} UBYTETOWORD;

typedef union 
{
    UINT8           Byte[2];
    UINT16          Word;
}
UALEVENT;

/*-----------------------------------------------------------------------------------------
------
------    LED defines
------
-----------------------------------------------------------------------------------------*/

#define LED_ECATRED                    LATDbits.LATD1

BOOL bEscInterrupt = 0;
BOOL bSync0Interrupt = 0;
BOOL bSync1Interrupt = 0;
BOOL bTimer5Interrupt = 0;
///////////////////////////////////////////////////////////////////////////////
// Global Interrupt setting

#define DISABLE_GLOBAL_INT          INTDisableInterrupts()
#define ENABLE_GLOBAL_INT           INTEnableInterrupts()
#define DISABLE_AL_EVENT_INT        DISABLE_GLOBAL_INT
#define ENABLE_AL_EVENT_INT         ENABLE_GLOBAL_INT



// ESC Interrupt 0 - falling edge 1-
#define   INIT_ESC_INT           //{(INTCONbits.INT0EP) = 0;(IPC0bits.INT0IP) = 7;} 
#define   ESC_INT_REQ            (0) /*(IFS0bits.INT0IF) ESC Interrupt (INT0) state/flag */
#define   INT_EL                 (0) //ESC Interrupt input port TODO: Make this that it reads pin input
#define   EscIsr                 (0) /*(_INT0Interrupt) // primary interrupt vector name*/
#define   ACK_ESC_INT            /*{(ESC_INT_REQ)=0;}*/
#define 	IS_ESC_INT_ACTIVE    ((INT_EL) == 0) //0 - fro active low; 1 for hactive high

// SYNC0 Interrupt
#define    INIT_SYNC0_INT                  //{(INTCONbits.INT1EP) = 0;(IPC1bits.INT1IP) = 5;}  TODO: Make that this inits SYNC0 input interrupt 
#define    SYNC0_INT_REQ                   (0)/*(IFS0bits.INT1IF) //(_INT3IF) //Sync0 Interrupt (INT1) state*/
#define    INT_SYNC0                       (0)/*(_RE8) //Sync1 Interrupt input port*/
#define    Sync0Isr                        (0)/*(_INT1Interrupt) // primary interrupt vector name*/
#define    DISABLE_SYNC0_INT               /*{IEC0bits.INT1IE=0;}//{(_INT1IE)=0;}//disable interrupt source INT1*/
#define    ENABLE_SYNC0_INT                /*{IEC0bits.INT1IE=1;} //enable interrupt source INT1*/
#define    ACK_SYNC0_INT                   /*{(SYNC0_INT_REQ) = 0;}*/
#define    SET_SYNC0_INT                   /*{(SYNC0_INT_REQ) = 1;}*/
#define    SYNC0_INT_PORT_IS_ACTIVE        {(INT_EL) == 0;}
#define    IS_SYNC0_INT_ACTIVE             ((INT_SYNC0) == 0) //0 - fro active low; 1 for hactive high

// SYNC1 Interrupt
#define    INIT_SYNC1_INT                   /*{(INTCONbits.INT2EP) = 0;(IPC2bits.INT2IP) = 4;}*/
#define    SYNC1_INT_REQ                    (0)/*(IFS0bits.INT2IF) //(_INT4IF) //Sync1 Interrupt (INT2) state*/
#define    INT_SYNC1                        (0)/*(_RE9) //Sync1 Interrupt input port*/
#define    Sync1Isr                         (0)/*(_INT2Interrupt) // primary interrupt vector name*/
#define    DISABLE_SYNC1_INT                /*{IEC0bits.INT2IE=0;}//disable interrupt source INT2*/
#define    ENABLE_SYNC1_INT                 /*{IEC0bits.INT2IE=1;} //enable interrupt source INT2*/
#define    ACK_SYNC1_INT                    /*{(SYNC1_INT_REQ) = 0;}*/
#define    SET_SYNC1_INT                    /*{(SYNC1_INT_REQ) = 1;}*/
#define    SYNC1_INT_PORT_IS_ACTIVE         {(INT_EL) == 0;}
#define    IS_SYNC1_INT_ACTIVE              ((INT_SYNC1) == 0) //0 - fro active low; 1 for hactive high

// Hardware timer
#define STOP_ECAT_TIMER         /*{(T5CONbits.TON) = 0;} Implement disable timer */
#define INIT_ECAT_TIMER         /*{ OpenTimer5(T5_ON | T5_IDLE_CON | T5_PS_1_256 | T5_SOURCE_INT,  ECAT_TIMER_INC_P_MS);STOP_ECAT_TIMER(TMR5) = 0;} Implement init and clear timer */
#define START_ECAT_TIMER        /* {(T5CONbits.TON) = 1;  } Implement enable timer */

// Internal Variables
UALEVENT      EscALEvent;     // contains the content of the ALEvent register (0x220), this variable is updated on each Access to the Esc
UINT16        nAlEventMask;   // current ALEventMask (content of register 0x204:0x205)
TSYNCMAN      TmpSyncMan;

///////////////////////////////////////////////////////////////////////////////
// Internal functions

/*******************************************************************************
  Function:
    void GetInterruptRegister(void)

  Summary:
    The function operates a SPI access without addressing.

  Description:
    The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
    It will be saved in the global "EscALEvent"
  *****************************************************************************/
	
static void INTDisableInterrupts(void) {
	// Disable intterrupt TODO: Implement
}

static void INTEnableInterrupts(void) {
	// Enable interrupts TODO: Implement
}

static void GetInterruptRegister(void)
{
      DISABLE_AL_EVENT_INT;
      HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, 0x220, 2);
      ENABLE_AL_EVENT_INT;

}

/*******************************************************************************
  Function:
    void ISR_GetInterruptRegister(void)

  Summary:
    The function operates a SPI access without addressing.
        Shall be implemented if interrupts are supported else this function is equal to "GetInterruptRegsiter()"

  Description:
    The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
  *****************************************************************************/

static void ISR_GetInterruptRegister(void)
{
     HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, 0x220, 2);
}


///////////////////////////////////////////////////////////////////////////////
// Exported HW Access functions


/*******************************************************************************
  Function:
    UINT8 HW_Init(void)

  Summary:
    This function intialize the Process Data Interface (PDI) and the host controller.

  Description:
    
  *****************************************************************************/

UINT8 HW_Init(void)
{
    UINT16 intMask;
    UINT32 data;
		
		driverSWLAN9252Init();

    do
    {
        intMask = 0x93;
        HW_EscWriteWord(intMask, ESC_AL_EVENTMASK_OFFSET);
       
        intMask = 0;
        HW_EscReadWord(intMask, ESC_AL_EVENTMASK_OFFSET);
    } while (intMask != 0x93);

   
    //IRQ enable,IRQ polarity, IRQ buffer type in Interrupt Configuration register.
    //Wrte 0x54 - 0x00000101
    data = 0x00000101;

    SPIWriteDWord(0x54, data);
    
    //Write in Interrupt Enable register -->
    //Write 0x5c - 0x00000001
    data = 0x00000001;

    SPIWriteDWord (0x5C, data);

    //Read Interrupt Status register
    //Read 0x58.

    SPIReadDWord(0x58);

#ifdef DC_SUPPORTED
#ifndef RUN_FROM_SVB_FPGA
    INIT_SYNC0_INT

    INIT_SYNC1_INT

    ENABLE_SYNC0_INT;
    ENABLE_SYNC1_INT;

#else

    //This is used in SVBS and FPAGA
    //1.Set Synch0 and Synch 1 as inputs
    //2.Enable DIGIO mode for synch0
    // 3. Enable CN Module
    // 4. Enable Interrupt on synch0 and synch1 + enable pullup for both pins
		// Clear initial interrupt
    // 8. Enable CN Interrupt
    IEC1SET = 0x0001; // Enable Change Notice interrupts
#endif

#endif
    INIT_ECAT_TIMER;

    //ConfigIntTimer5(T5_INT_ON | T5_INT_PRIOR_3 );
    START_ECAT_TIMER;

    INIT_ESC_INT;
    HW_ResetALEventMask(0);
    ENABLE_ESC_INT();
    
    /* enable all interrupts */
    ENABLE_GLOBAL_INT;

    return 0;
}


/*******************************************************************************
  Function:
    void HW_Release(void)

  Summary:
    This function shall be implemented if hardware resources need to be release
        when the sample application stops

  Description:
  *****************************************************************************/

void HW_Release(void)
{

}


/*******************************************************************************
  Function:
    UINT16 HW_GetALEventRegister(void)

  Summary:
    This function gets the current content of ALEvent register.

  Description:
    Returns first two Bytes of ALEvent register (0x220)
  *****************************************************************************/

UINT16 HW_GetALEventRegister(void)
{
    GetInterruptRegister();
    return EscALEvent.Word;
}


/*******************************************************************************
  Function:
    UINT16 HW_GetALEventRegister_Isr(void)

  Summary:
    The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_GetALEventRegister()"

  Description:
    Returns  first two Bytes of ALEvent register (0x220)
  *****************************************************************************/

UINT16 HW_GetALEventRegister_Isr(void)
{
     ISR_GetInterruptRegister();
    return EscALEvent.Word;
}


/*******************************************************************************
  Function:
    void HW_ResetALEventMask(UINT16 intMask)

  Summary:
    This function makes an logical and with the AL Event Mask register (0x204)

  Description:
    Input param: intMask - interrupt mask (disabled interrupt shall be zero)
  *****************************************************************************/

void HW_ResetALEventMask(UINT16 intMask)
{
    UINT16 mask;

    HW_EscReadWord(mask, ESC_AL_EVENTMASK_OFFSET);

    mask &= intMask;
    DISABLE_AL_EVENT_INT;
    HW_EscWriteWord(mask, ESC_AL_EVENTMASK_OFFSET);
    HW_EscReadWord(nAlEventMask, ESC_AL_EVENTMASK_OFFSET);
    ENABLE_AL_EVENT_INT;
}


/*******************************************************************************
  Function:
    void HW_SetALEventMask(UINT16 intMask)

  Summary:
    This function makes an logical or with the AL Event Mask register (0x204)

  Description:
    Input param: intMask - interrupt mask (disabled interrupt shall be zero)
  *****************************************************************************/

void HW_SetALEventMask(UINT16 intMask)
{
    UINT16 mask;

    HW_EscReadWord(mask, ESC_AL_EVENTMASK_OFFSET);

    mask |= intMask;
    DISABLE_AL_EVENT_INT;
    HW_EscWriteWord(mask, ESC_AL_EVENTMASK_OFFSET);
    HW_EscReadWord(nAlEventMask, ESC_AL_EVENTMASK_OFFSET);
    ENABLE_AL_EVENT_INT;
}


/*******************************************************************************
  Function:
    void HW_EscRead( MEM_ADDR *pData, UINT16 Address, UINT16 Len )

  Summary:
    This function operates the SPI read access to the EtherCAT ASIC.

  Description:
    Input param:
     pData    - Pointer to a byte array which holds data to write or saves read data.
     Address  - EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
     Len      - Access size in Bytes.
  *****************************************************************************/

void HW_EscRead( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{
    UINT16 i;
    UINT8 *pTmpData = (UINT8 *)pData;

    /* loop for all bytes to be read */
    while ( Len > 0 )
    {
        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i= (Len > 4) ? 4 : Len;

            if(Address & 01)
            {
               i=1;
            }
            else if (Address & 02)
            {
               i= (i&1) ? 1:2;
            }
            else if (i == 03)
            {
                i=1;
            }
        }

        DISABLE_AL_EVENT_INT;

       SPIReadDRegister(pTmpData,Address,i);
      
       ENABLE_AL_EVENT_INT;

        Len -= i;
        pTmpData += i;
        Address += i;
    }

#ifdef INTERRUPTS_SUPPORTED
    if(IS_ESC_INT_ACTIVE)
    {
        DISABLE_ESC_INT();
        PDI_Isr();
        /* reset the interrupt flag */
        ACK_ESC_INT;
    }
#endif
#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
if(IS_SYNC0_INT_ACTIVE)
    {
        Sync0_Isr();

       /* reset the interrupt flag */
       ACK_SYNC0_INT;
    }
#endif
#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
if(IS_SYNC1_INT_ACTIVE)
    {
        Sync1_Isr();

        /* reset the interrupt flag */
        ACK_SYNC1_INT;
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

\brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_EscRead()"
*////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
  Function:
    void HW_EscReadIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len )

  Summary:
    The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_EscRead()"

  Description:
    Input param:
    pData          - Pointer to a byte array which holds data to write or saves read data.
    param Address  - EtherCAT ASIC address ( upper limit is 0x1FFF ) for access.
    param Len      - Access size in Bytes.
  *****************************************************************************/

void HW_EscReadIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{

   UINT16 i;
   UINT8 *pTmpData = (UINT8 *)pData;

    /* send the address and command to the ESC */

    /* loop for all bytes to be read */
   while ( Len > 0 )
   {

        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i= (Len > 4) ? 4 : Len;

            if(Address & 01)
            {
               i=1;
            }
            else if (Address & 02)
            {
               i= (i&1) ? 1:2;
            }
            else if (i == 03)
            {
                i=1;
            }
        }

    #ifndef USE_SPI
        PMPReadDRegister(pTmpData, Address,i);
    #else
        SPIReadDRegister(pTmpData, Address,i);
    #endif

        Len -= i;
        pTmpData += i;
        Address += i;
    }
   
}


/*******************************************************************************
  Function:
    void HW_EscWrite( MEM_ADDR *pData, UINT16 Address, UINT16 Len )

  Summary:
    This function operates the SPI write access to the EtherCAT ASIC.

  Description:
    Input param:
    pData          - Pointer to a byte array which holds data to write or saves write data.
    param Address  - EtherCAT ASIC address ( upper limit is 0x1FFF ) for access.
    param Len      - Access size in Bytes.
  *****************************************************************************/

void HW_EscWrite( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{

    UINT16 i;
    UINT8 *pTmpData = (UINT8 *)pData;

    /* loop for all bytes to be written */
    while ( Len )
    {

        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i= (Len > 4) ? 4 : Len;

            if(Address & 01)
            {
               i=1;
            }
            else if (Address & 02)
            {
               i= (i&1) ? 1:2;
            }
            else if (i == 03)
            {
                i=1;
            }
        }

        DISABLE_AL_EVENT_INT;
       
        /* start transmission */
#ifndef USE_SPI
        PMPWriteRegister(pTmpData, Address, i);
#else
        SPIWriteRegister(pTmpData, Address, i);
#endif

        ENABLE_AL_EVENT_INT;

       
   
        /* next address */
        Len -= i;
        pTmpData += i;
        Address += i;

    }

#ifdef INTERRUPTS_SUPPORTED
    if(IS_ESC_INT_ACTIVE)
    {
        DISABLE_ESC_INT();
        PDI_Isr();
        /* reset the interrupt flag */
        ACK_ESC_INT;
    }
#endif
#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
if(IS_SYNC0_INT_ACTIVE)
    {
        Sync0_Isr();

       /* reset the interrupt flag */
       ACK_SYNC0_INT;
    }
#endif
#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
if(IS_SYNC1_INT_ACTIVE)
    {
        Sync1_Isr();

        /* reset the interrupt flag */
        ACK_SYNC1_INT;
    }
#endif
}


/*******************************************************************************
  Function:
    void HW_EscWriteIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len )

  Summary:
    The SPI PDI requires an extra ESC write access functions from interrupts service routines.
        The behaviour is equal to "HW_EscWrite()"

  Description:
    Input param:
    pData          - Pointer to a byte array which holds data to write or saves write data.
    param Address  - EtherCAT ASIC address ( upper limit is 0x1FFF ) for access.
    param Len      - Access size in Bytes.
  *****************************************************************************/

void HW_EscWriteIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{

    UINT16 i ;
    UINT8 *pTmpData = (UINT8 *)pData;

  
    /* loop for all bytes to be written */
    while ( Len )
    {

        if (Address >= 0x1000)
        {
            i = Len;
        }
        else
        {
            i= (Len > 4) ? 4 : Len;

            if(Address & 01)
            {
               i=1;
            }
            else if (Address & 02)
            {
               i= (i&1) ? 1:2;
            }
            else if (i == 03)
            {
                i=1;
            }
        }
        
       /* start transmission */
    #ifndef USE_SPI
       PMPWriteRegister(pTmpData, Address, i);
    #else
       SPIWriteRegister(pTmpData, Address, i);
    #endif
       
       /* next address */
        Len -= i;
        pTmpData += i;
        Address += i;
    }

}


/*******************************************************************************
  Function:
    void HW_DisableSyncManChannel(UINT8 channel)

  Summary:
    This function disables a Sync Manager channel

  Description:
    Input param: channel - Sync Manager channel
  *****************************************************************************/

void HW_DisableSyncManChannel(UINT8 channel)
{
    UINT16 Offset;


    volatile UINT32 smStatus = SM_SETTING_PDI_DISABLE;
    smStatus = SWAPDWORD(smStatus);

    Offset = (ESC_SYNCMAN_CONTROL_OFFSET + (SIZEOF_SM_REGISTER*channel));

    HW_EscWriteDWord(smStatus,Offset);

    /*wait until SyncManager is disabled*/
    do
    {
        HW_EscReadDWord(smStatus, Offset);

        smStatus = SWAPDWORD(smStatus);

    }while(!(smStatus & SM_SETTING_PDI_DISABLE));
}


/*******************************************************************************
  Function:
    void HW_EnableSyncManChannel(UINT8 channel)

  Summary:
    This function enables a Sync Manager channel

  Description:
    Input param: channel - Sync Manager channel
  *****************************************************************************/

void HW_EnableSyncManChannel(UINT8 channel)
{
    UINT16 Offset;

    volatile UINT32 smStatus = 0x00000000;

    Offset = (ESC_SYNCMAN_CONTROL_OFFSET + (SIZEOF_SM_REGISTER*channel));

    HW_EscWriteDWord(smStatus,Offset);

    /*wait until SyncManager is enabled*/
    do
    {
        HW_EscReadDWord(smStatus,Offset);

        smStatus = SWAPDWORD(smStatus);

    }while((smStatus & SM_SETTING_PDI_DISABLE));
}


/*******************************************************************************
  Function:
    TSYNCMAN ESCMEM * HW_GetSyncMan(UINT8 channel)

  Summary:
    This function is called to read the SYNC Manager channel descriptions of the
             process data SYNC Managers.

  Description:
    Input param: channel - Sync Manager channel information requested
	Returns: Pointer to the SYNC Manager channel description
  *****************************************************************************/


TSYNCMAN ESCMEM * HW_GetSyncMan(UINT8 channel)
{
    // get a temporary structure of the Sync Manager
    HW_EscRead( (MEM_ADDR *)&TmpSyncMan, ESC_SYNCMAN_REG_OFFSET + (channel * SIZEOF_SM_REGISTER), SIZEOF_SM_REGISTER );

    return &TmpSyncMan;
}

/*******************************************************************************

 \param RunLed            desired EtherCAT Run led state
 \param ErrLed            desired EtherCAT Error led state

  \brief    This function updates the EtherCAT run and error led
  *****************************************************************************/
void HW_SetLed(UINT8 RunLed,UINT8 ErrLed)
{
    /* Here RunLed is not used. Because on chip supported RUN Led is available*/    
    //LED_ECATRED   = ErrLed;
}

/*******************************************************************************
  Function:
    void __ISR(_EXTERNAL_0_VECTOR, ipl7srs) ExtInterruptHandler(void)

  Summary:
    Interrupt service routine for the PDI interrupt from the EtherCAT Slave Controller

  *****************************************************************************/

/* Handle interrupt service routing
void __ISR(_EXTERNAL_0_VECTOR, ipl7srs) ExtInterruptHandler(void) {
   DISABLE_ESC_INT();

   PDI_Isr();

   //reset the interrupt flag
   ACK_ESC_INT;
   ENABLE_ESC_INT();

#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
if(IS_SYNC0_INT_ACTIVE)
    {
        Sync0_Isr();

       // reset the interrupt flag
       ACK_SYNC0_INT;
    }
#endif
#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
if(IS_SYNC1_INT_ACTIVE)
    {
        Sync1_Isr();

        //reset the interrupt flag
        ACK_SYNC1_INT;
    }
#endif
}
*/


#ifdef DC_SUPPORTED

/*******************************************************************************
  Function:
    void __ISR(_EXTERNAL_1_VECTOR, ipl5) Sync0Isr(void)

  Summary:
    Interrupt service routine for the interrupts from SYNC0
  *****************************************************************************/

/* Handle the other ISR
void __ISR(_EXTERNAL_1_VECTOR, ipl5) Sync0Isr(void)
{
    
   DISABLE_ESC_INT();
    
   Sync0_Isr();
   
   // reset the interrupt flag
   ACK_SYNC0_INT;
   ENABLE_ESC_INT();

   //If interrupt is missed then handle it here.
#ifdef INTERRUPTS_SUPPORTED
   if(IS_ESC_INT_ACTIVE)
   {
       DISABLE_ESC_INT();
       PDI_Isr();
       // reset the interrupt flag
       ACK_ESC_INT;
   }
#endif
//#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
//if(IS_SYNC1_INT_ACTIVE)
//    {
//        Sync1_Isr();
//
//        // reset the interrupt flag
//        ACK_SYNC1_INT;
//    }
//#endif
}

*/


/*******************************************************************************
  Function:
    void __ISR(_EXTERNAL_2_VECTOR, ipl4) Sync1Isr(void)


  Summary:
    Interrupt service routine for the interrupts from SYNC1
  *****************************************************************************/

/* Handle the other other ISR
void __ISR(_EXTERNAL_2_VECTOR, ipl4) Sync1Isr(void)
{

   DISABLE_ESC_INT();
   Sync1_Isr();
   
    // reset the interrupt flag
    ACK_SYNC1_INT;

   ENABLE_ESC_INT();

#ifdef INTERRUPTS_SUPPORTED
    if(IS_ESC_INT_ACTIVE)
    {
        DISABLE_ESC_INT();
        PDI_Isr();
        // reset the interrupt flag
        ACK_ESC_INT;
    }
#endif
//#if defined (INTERRUPTS_SUPPORTED) && defined(DC_SUPPORTED)
//if(IS_SYNC0_INT_ACTIVE)
//    {
//        Sync0_Isr();
//
//       // reset the interrupt flag 
//       ACK_SYNC0_INT;
//    }
//#endif

}*/

/* Ethercat systick interval timer
void __ISR(_TIMER_5_VECTOR,ipl3) _TIMER3_HANDLER(void)
 {
    ECAT_CheckTimer();

    bTimer5Interrupt = 1;
    mT5ClearIntFlag(); //clear flag
 }
 */

/*******************************************************************************
  Function:
    void __ISR(_CHANGE_NOTICE_VECTOR, ipl5) ChangeNoticeHandler(void)

  Summary:
    Interrupt service routine for the Change Notice Handler
  *****************************************************************************/
/*
BOOL volatile sync0 = 0,sync0_cleared=1,sync1 =0,sync1_cleared = 1;
void __ISR(_CHANGE_NOTICE_VECTOR, ipl5) ChangeNoticeHandler(void)
{
    //Disable ISR
    IEC1CLR = 0x0001;

    sync1 = _RB3;  	// Read PORTB to clear CN5 mismatch condition
    sync0 = _RG6;  	// Read PORTG to clear CN8 mismatch condition
    IFS1CLR = 0x0001;	// Be sure to clear the CN interrupt status

    if((!(sync1 & SYNC1_ACTIVE_LOW))& sync1_cleared)
    {
        Sync1_Isr();
        sync1_cleared = 0;
    }
    else
    {
        sync1_cleared = 1;
    }

    if((!(sync0 & SYNC0_ACTIVE_LOW))& sync0_cleared)
    {
        Sync0_Isr();
        sync0_cleared = 0;
    }
    else
    {
        sync0_cleared = 1;
    }
        
     // Enable ISR
     IEC1SET = 0x0001;
}
*/

#endif //DC_SUPPORTED
