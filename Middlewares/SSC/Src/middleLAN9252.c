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

#define SYNC0_ACTIVE_LOW  true
#define SYNC1_ACTIVE_LOW  true

#ifndef    PIC32_HW
    #define PIC32_HW	// Use PIC32MX board
#endif

///////////////////////////////////////////////////////////////////////////////
// Internal Type Defines

typedef union {
    unsigned short    Word;
    unsigned char    Byte[2];
}UBYTETOWORD;

typedef union {
    UINT8           Byte[2];
    UINT16          Word;
}UALEVENT;

volatile unsigned int restore_intsts = 0;

///////////////////////////////////////////////////////////////////////////////
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

static void GetInterruptRegister(void) {
	volatile unsigned int int_status;
	int_status = PDI_Disable_Global_Interrupt();
	HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, 0x220, 2);
	PDI_Restore_Global_Interrupt(int_status);
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

static void ISR_GetInterruptRegister(void) {
	HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, 0x220, 2);
}

/*******************************************************************************
  Function:
    UINT8 HW_Init(void)

  Summary:
    This function intialize the Process Data Interface (PDI) and the host controller.

  Description:
    
*****************************************************************************/
UINT8 HW_Init(void) {
	UINT16 intMask;
	uint32_t temp = 0;
	UINT32 data;

	PDI_Init();
	PDI_BindISR_Timer(&ECAT_CheckTimer);																// Bind SSC Timer intterupt handler to the interrupt
	PDI_BindISR_IRQ(&PDI_Isr);																					// Bind SSC IRQ intterupt handler to the interrupt
	PDI_BindISR_SYNC0(&Sync0_Isr);																			// Bind SSC SYNC0 intterupt handler to the interrupt
	PDI_BindISR_SYNC1(&Sync1_Isr);																			// Bind SSC SYNC1 intterupt handler to the interrupt

	do{
		data = PDIReadLAN9252DirectReg( LAN9252_BYTE_ORDER_REG);					// Read byte order register
	}while(0x87654321 != data);

	do{
		intMask = 0x93;
		HW_EscWriteWord(intMask, ESC_AL_EVENTMASK_OFFSET);
	 
		intMask = 0;
		HW_EscReadWord(intMask, ESC_AL_EVENTMASK_OFFSET);
	} while (intMask != 0x93);
	
	//IRQ enable,IRQ polarity, IRQ buffer type in Interrupt Configuration register.
	//Wrte 0x54 - 0x00000101
	//data = 0x00000101; <- original
	data = 0x00000100;
	
	PDIWriteLAN9252DirectReg(data, LAN9252_CSR_INT_CONF);
	
	//Write in Interrupt Enable register -->
	//Write 0x5c - 0x00000001
	data = 0x00000001;
	PDIWriteLAN9252DirectReg(data, LAN9252_CSR_INT_EN);

	//Read Interrupt Status register
	data = PDIReadLAN9252DirectReg(LAN9252_CSR_INT_STS);

#ifdef DC_SUPPORTED
	PDI_Init_SYNC_Interrupts();																					// Init and enable SYNC interrupts
#endif

	PDI_Init_Timer_Interrupt();    																			// Init and enable Timer interrupt
	HW_ResetALEventMask(0);
	PDI_Init_IRQ_Interrupt();																						// Init and enable IRQ interrupt
	
	/* enable all interrupts */
	PDI_Enable_Global_interrupt();
 
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

void HW_Release(void) {

}


/*******************************************************************************
  Function:
    UINT16 HW_GetALEventRegister(void)

  Summary:
    This function gets the current content of ALEvent register.

  Description:
    Returns first two Bytes of ALEvent register (0x220)
  *****************************************************************************/

UINT16 HW_GetALEventRegister(void) {
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

UINT16 HW_GetALEventRegister_Isr(void) {
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

void HW_ResetALEventMask(UINT16 intMask) {
	volatile unsigned int int_status;
	UINT16 mask;
	
	HW_EscReadWord(mask, ESC_AL_EVENTMASK_OFFSET);

	mask &= intMask;
	int_status = PDI_Disable_Global_Interrupt();
	HW_EscWriteWord(mask, ESC_AL_EVENTMASK_OFFSET);
	HW_EscReadWord(nAlEventMask, ESC_AL_EVENTMASK_OFFSET);
	PDI_Restore_Global_Interrupt(int_status);
}


/*******************************************************************************
  Function:
    void HW_SetALEventMask(UINT16 intMask)

  Summary:
    This function makes an logical or with the AL Event Mask register (0x204)

  Description:
    Input param: intMask - interrupt mask (disabled interrupt shall be zero)
  *****************************************************************************/

void HW_SetALEventMask(UINT16 intMask) {
	volatile unsigned int int_status;
	UINT16 mask;

	HW_EscReadWord(mask, ESC_AL_EVENTMASK_OFFSET);

	mask |= intMask;
	int_status = PDI_Disable_Global_Interrupt();
	HW_EscWriteWord(mask, ESC_AL_EVENTMASK_OFFSET);
	HW_EscReadWord(nAlEventMask, ESC_AL_EVENTMASK_OFFSET);
	PDI_Restore_Global_Interrupt(int_status);
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

void HW_EscRead( MEM_ADDR *pData, UINT16 Address, UINT16 Len ) {
	volatile unsigned int int_status;
	UINT16 i;
	UINT8 *pTmpData = (UINT8 *)pData;

	/* loop for all bytes to be read */
	while ( Len > 0 ) {
		if (Address >= MIN_PD_READ_ADDRESS){
			i = Len;
		}else{
			i= (Len > 4) ? 4 : Len;

			if(Address & 01){
				i=1;
			}else if (Address & 02){
				i= (i&1) ? 1:2;
			}else if (i == 03){
				i=1;
			}
		}

		int_status = PDI_Disable_Global_Interrupt();
		PDIReadReg(pTmpData, Address, i);
		PDI_Restore_Global_Interrupt(int_status);

		Len -= i;
		pTmpData += i;
		Address += i;
	}
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

void HW_EscReadIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len ) {
	UINT16 i;
	UINT8 *pTmpData = (UINT8 *)pData;

	/* send the address and command to the ESC */
	/* loop for all bytes to be read */
	while ( Len > 0 ) {
	if (Address >= MIN_PD_READ_ADDRESS) {
		i = Len;
	}else{
		i= (Len > 4) ? 4 : Len;

		if(Address & 01){
			 i=1;
		}else if (Address & 02){
			 i= (i&1) ? 1:2;
		}else if (i == 03){
				i=1;
		}
	}

	PDIReadReg(pTmpData, Address,i);

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

void HW_EscWrite( MEM_ADDR *pData, UINT16 Address, UINT16 Len ) {
	volatile unsigned int int_status;
	UINT16 i;
	UINT8 *pTmpData = (UINT8 *)pData;

	/* loop for all bytes to be written */
	while ( Len ){
		if (Address >= MIN_PD_WRITE_ADDRESS){
			i = Len;
		}else{
			i= (Len > 4) ? 4 : Len;

			if(Address & 01){
				i=1;
			}else if (Address & 02){
				i= (i&1) ? 1:2;
			}else if (i == 03){
				i=1;
			}
		}

		int_status = PDI_Disable_Global_Interrupt();
	 
		/* start transmission */
		PDIWriteReg(pTmpData, Address, i);
		PDI_Restore_Global_Interrupt(int_status);

		/* next address */
		Len -= i;
		pTmpData += i;
		Address += i;
	}
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

void HW_EscWriteIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len ) {
	UINT16 i ;
	UINT8 *pTmpData = (UINT8 *)pData;

	/* loop for all bytes to be written */
	while ( Len ){
		if (Address >= MIN_PD_WRITE_ADDRESS){
			i = Len;
		}else{
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
		PDIWriteReg(pTmpData, Address, i);

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
void HW_DisableSyncManChannel(UINT8 channel) {
	UINT16 Offset;
	volatile UINT32 smStatus = SM_SETTING_PDI_DISABLE;
	smStatus = SWAPDWORD(smStatus);
	Offset = (ESC_SYNCMAN_CONTROL_OFFSET + (SIZEOF_SM_REGISTER*channel));

	HW_EscWriteDWord(smStatus,Offset);

	/*wait until SyncManager is disabled*/
	do{
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
void HW_EnableSyncManChannel(UINT8 channel) {
    UINT16 Offset;
    volatile UINT32 smStatus = 0x00000000;
    Offset = (ESC_SYNCMAN_CONTROL_OFFSET + (SIZEOF_SM_REGISTER*channel));

    HW_EscWriteDWord(smStatus,Offset);

    /*wait until SyncManager is enabled*/
    do{
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
TSYNCMAN ESCMEM * HW_GetSyncMan(UINT8 channel) {
	// get a temporary structure of the Sync Manager
	HW_EscRead( (MEM_ADDR *)&TmpSyncMan, ESC_SYNCMAN_REG_OFFSET + (channel * SIZEOF_SM_REGISTER), SIZEOF_SM_REGISTER );

	return &TmpSyncMan;
}

/*******************************************************************************

 \param RunLed            desired EtherCAT Run led state
 \param ErrLed            desired EtherCAT Error led state

  \brief    This function updates the EtherCAT run and error led
*****************************************************************************/
void HW_SetLed(UINT8 RunLed,UINT8 ErrLed) {
	PDI_UpdateLED(RunLed,ErrLed);																									// Update the led status
}

uint32_t HW_GetTimer(void) {
	return PDI_GetTimer();																												// Get timer value
}

void HW_ClearTimer(void) {
	PDI_ClearTimer();																															// Reset timer value
}

void DISABLE_ESC_INT(void) {
	restore_intsts = PDI_Disable_Global_Interrupt();
}

void ENABLE_ESC_INT(void) {
	PDI_Restore_Global_Interrupt(restore_intsts);
}
