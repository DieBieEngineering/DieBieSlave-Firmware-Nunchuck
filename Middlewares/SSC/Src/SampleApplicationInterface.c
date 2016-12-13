/**
 * \addtogroup SlaveLibrary EtherCAT Slave Library 
 * @{
 */

/**
\file SampleApplicationInterface.c
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT11: create application interface function pointer, add eeprom emulation interface functions<br>
<br>Changes to version V5.01:<br>
V5.10 ECAT9: Change slave library interface application<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#ifdef _WIN32
#include <stdio.h>
#include <tchar.h>
#endif

#include "ecat_def.h"

#if SAMPLE_APPLICATION_INTERFACE

/* ECATCHANGE_START(V5.11) ECAT11*/
#include "applInterface.h"
/* ECATCHANGE_END(V5.11) ECAT11*/

#include "sdoserv.h"


#define IMPL_OR_EXTERN 1
#include "EtherCATSampleLibrary.h"
#undef IMPL_OR_EXTERN

#define _SAMPLE_APPLICATION_INTERFACE_
#include "SampleApplicationInterface.h"
#undef _SAMPLE_APPLICATION_INTERFACE_


/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/

UINT8 HardwareOpened = 0;
UINT16 nInputSize = 4;
UINT16 nOutputSize = 4;

ECAT_SLAVE_INTERFACE SlaveInterface;

extern TSDOINFOENTRYDESC    OBJMEM EntryDesc0x6000;
extern TSDOINFOENTRYDESC    OBJMEM EntryDesc0x7010;
/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void    APPL_AckErrorInd(UINT16 stateTrans)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
             all general settings were checked to start the mailbox handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case this function will be called cyclically
            until a value unequal NOERROR_INWORK is returned

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    if(SlaveInterface.pStateTrans != NULL)
    {
        SlaveInterface.pStateTrans(INIT_2_PREOP);
    }        

    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    if(SlaveInterface.pStateTrans != NULL)
    {
        SlaveInterface.pStateTrans(PREOP_2_INIT);
    }        

    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                        register (0x204) when this function is succeeded. The event mask can be adapted
                        in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
           all general settings were checked to start the input handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
    if(SlaveInterface.pStateTrans != NULL)
    {
        SlaveInterface.pStateTrans(PREOP_2_SAFEOP);
    }        

    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
    if(SlaveInterface.pStateTrans != NULL)
    {
        SlaveInterface.pStateTrans(SAFEOP_2_PREOP);
    }        

    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    if(SlaveInterface.pStateTrans != NULL)
    {
        SlaveInterface.pStateTrans(SAFEOP_2_OP);
    }        

    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
    if(SlaveInterface.pStateTrans != NULL)
    {
        SlaveInterface.pStateTrans(OP_2_SAFEOP);
    }        

    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data

\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
#if _PIC18 && AL_EVENT_ENABLED
/* the pragma interrupt_level is used to tell the compiler that these functions will not
   be called at the same time from the main function and the interrupt routine */
#pragma interrupt_level 1
#endif
void APPL_InputMapping(UINT16* pData)
{
    if(pData != NULL && SlaveInterface.pInput != NULL && nInputSize > 0)
    {
        MEMCPY(pData,SlaveInterface.pInput,nInputSize);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
#if _PIC18  && AL_EVENT_ENABLED
/* the pragma interrupt_level is used to tell the compiler that these functions will not
   be called at the same time from the main function and the interrupt routine */
#pragma interrupt_level 1
#endif
void APPL_OutputMapping(UINT16* pData)
{
    if(pData != NULL && SlaveInterface.pOutput != NULL && nOutputSize > 0)
    {
        MEMCPY(SlaveInterface.pOutput,pData,nOutputSize);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_Application(void)
{
    if (SlaveInterface.pApplication != NULL)
    {
        SlaveInterface.pApplication();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index               index of the requested object.
 \param     subindex            subindex of the requested object.
 \param     objSize             size of the requested object data, calculated with OBJ_GetObjectLength
 \param     pData               Pointer to the buffer where the data can be copied to
 \param     bCompleteAccess     Indicates if a complete read of all subindices of the
                                object shall be done or not

 \return    result of the read operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief     This function reads the process data object
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 ReadProcessDataObject( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{

    if(index == 0x7010 || index == 0x6000)
    {
        if(subindex > 0)
        {
            return ABORTIDX_SUBINDEX_NOT_EXISTING;
        }
    
        if(index == 0x7010)
        {
            if(dataSize > nPdOutputSize)
            {
                return ABORTIDX_PARAM_LENGTH_TOO_LONG;
            }
            else
                MEMCPY(pData,SlaveInterface.pOutput,dataSize);
        }
        else 
        {
            if(dataSize > nPdInputSize)
            {
                return ABORTIDX_PARAM_LENGTH_TOO_LONG;
            }
            else
                MEMCPY(pData,SlaveInterface.pInput,dataSize);
        }
    }
    else
    {
        return ABORTIDX_UNSUPPORTED_ACCESS;    
    }

    return 0x00;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    index              index of the requested object.
 \param    subindex           subindex of the requested object.
 \param    dataSize           received data size of the SDO Download
 \param    pData              Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                              object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief     This function writes the process data object
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 WriteProcessDataObject( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
    if(index == 0x7010)
    {
        if(subindex > 0)
        {
            return ABORTIDX_SUBINDEX_NOT_EXISTING;
        }
    
        if(dataSize > nPdOutputSize)
        {
            return ABORTIDX_PARAM_LENGTH_TOO_LONG;
        }

        MEMCPY(SlaveInterface.pOutput,pData,dataSize);
    }
    else
    {
        return ABORTIDX_UNSUPPORTED_ACCESS;    
    }

    return 0x00;
}
#if EXPLICIT_DEVICE_ID
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    The Explicit Device ID of the EtherCAT slave

 \brief     Calculate the Explicit Device ID
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDeviceID()
{
    return 0x5;
}
#endif

pECAT_SLAVE_INTERFACE _stdcall Ecat_Open(unsigned short InputSize, unsigned short OutputSize)
{
    UINT16 result = 0;
    UINT16 counter = 0;
    //Open function shall only called once
    if(HardwareOpened != 0)
        return &SlaveInterface;


    /*Free Input process data memory*/
    if(SlaveInterface.pInput != NULL)
    {
        FREEMEM(SlaveInterface.pInput);
        SlaveInterface.pInput = NULL;
    }

    nPdInputSize = InputSize;
    /* Allocate Input process data memory
        4kB DPRAM of the ESC ((DPRAM - 2*mailbox) / number of process data directions / SyncManager buffer mode
         => (4kB - 256Byte) / 2 /3 =>  640 Byte*/
    if(nPdInputSize > 0 && nPdInputSize <= 640)
    {
        SlaveInterface.pInput = (unsigned long *) ALLOCMEM(nPdInputSize);
    }
    
    /*Allocation was not success full or no input process data defined*/
    if(SlaveInterface.pInput == NULL)
    {
        if(nPdInputSize > 0)
        {
            /*Allocation failed*/
            return NULL;
        }

        sTxPDOassign.u16SubIndex0 = 0;
        TxPDOMap.u16SubIndex0 = 0;
        for(counter = 0; counter < PDO_MAPPING_ENTIRES; counter++)
            TxPDOMap.aEntries[counter] = 0x00000000;

#if COE_SUPPORTED
        EntryDesc0x6000.BitLength = 0;
#endif
    }
    else
    {
        UINT16 tmpPdCnt = nPdInputSize;
        UINT32 EntryValue = 0x60000000;
        counter = 0;

        HMEMSET(SlaveInterface.pInput,0x00,nPdInputSize);

        sTxPDOassign.u16SubIndex0 = 1;
        if(tmpPdCnt <= 31)
        {
            /* up to 31Byte fits into one Pdo mapping entry*/
            TxPDOMap.aEntries[0] = (EntryValue | (tmpPdCnt*8));
            TxPDOMap.u16SubIndex0 = 1;
        }
        else
        {
            /*The process data reference doesn't fit into one entry*/
            while(tmpPdCnt > 0)
            {
                /*all following entries have no index and subindex reference*/
                if(counter > 0)
                    EntryValue = 0x00000000;


                if(tmpPdCnt >= 30)
                {
                    TxPDOMap.aEntries[counter] = EntryValue | 240;
                    tmpPdCnt -= 30;
                }
                else
                {
                    /*last reference*/
                    TxPDOMap.aEntries[counter] = EntryValue | (tmpPdCnt * 8);
                    tmpPdCnt = 0;
                }


                counter++;
            }
            TxPDOMap.u16SubIndex0 = counter;
        }
#if COE_SUPPORTED
        EntryDesc0x6000.BitLength = (nPdInputSize *8);
#endif
    }

  
    /*Free Output process data memory*/
    if(SlaveInterface.pOutput!= NULL)
    {
        FREEMEM(SlaveInterface.pOutput);
        SlaveInterface.pOutput = NULL;
    }

    nPdOutputSize = OutputSize;
    /* Allocate Output process data memory
        4kB DPRAM of the ESC ((DPRAM - 2*mailbox) / number of process data directions / SyncManager buffer mode
         => (4kB - 256Byte) / 2 /3 =>  640 Byte*/
    if(nPdOutputSize > 0 && nPdOutputSize <= 640)
    {
        SlaveInterface.pOutput = (unsigned long *) ALLOCMEM(nPdOutputSize);
    }
    
    /*Allocation was not success full or no input process data defined*/
    if(SlaveInterface.pOutput == NULL)
    {
        if(nPdOutputSize > 0)
        {
            /*Allocation failed*/
            return NULL;
        }

        sRxPDOassign.u16SubIndex0 = 0;
        RxPDOMap.u16SubIndex0 = 0;
        for(counter = 0; counter < PDO_MAPPING_ENTIRES; counter++)
            RxPDOMap.aEntries[counter] = 0x00000000;

#if COE_SUPPORTED
        EntryDesc0x7010.BitLength = 0;
#endif
    }
    else
    {
        UINT16 tmpPdCnt = nPdOutputSize;
        UINT32 EntryValue = 0x70100000;
        counter = 0;

        HMEMSET(SlaveInterface.pOutput,0x00,nPdOutputSize);

        sRxPDOassign.u16SubIndex0 = 1;
        if(tmpPdCnt <= 31)
        {
            /* up to 31Byte fits into one Pdo mapping entry*/
            RxPDOMap.aEntries[0] = (EntryValue | (tmpPdCnt*8));
            RxPDOMap.u16SubIndex0 = 1;
        }
        else
        {
            /*The process data reference doesn't fit into one entry*/
            while(tmpPdCnt > 0)
            {
                /*all following entries have no index and subindex reference*/
                if(counter > 0)
                    EntryValue = 0x00000000;


                if(tmpPdCnt >= 30)
                {
                    RxPDOMap.aEntries[counter] = EntryValue | 240;
                    tmpPdCnt -= 30;
                }
                else
                {
                    /*last reference*/
                    RxPDOMap.aEntries[counter] = EntryValue | (tmpPdCnt * 8);
                    tmpPdCnt = 0;
                }


                counter++;
            }
            RxPDOMap.u16SubIndex0 = counter;
        }
#if COE_SUPPORTED
        EntryDesc0x7010.BitLength = (nPdOutputSize *8);
#endif
    }


    SlaveInterface.pApplication = NULL;
    SlaveInterface.pStateTrans = NULL;

    result = HW_Init();
    if(result == 0)
    {
        MainInit();
        HardwareOpened = 1;
    }
    else
        return NULL;


   return &SlaveInterface;
}

void __stdcall Ecat_OnTimer(void)
{
    if(HardwareOpened == 0)
        return;

    MainLoop();
}

void __stdcall Ecat_Close(void)
{
    if(HardwareOpened == 0)
        return;

    HW_Release();

    HardwareOpened = 0;
}
#endif //#if SAMPLE_APPLICATION_INTERFACE

/** @} */

