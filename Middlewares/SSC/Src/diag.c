/**
\addtogroup Diagnosis Object
@{
*/

/**
\file diag.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the Diagnose object handling

\version 5.11

<br>Changes to version V5.0:<br>
V5.11 DIAG1: update diag message time stamp calculation<br>
V5.11 DIAG2: update clear message history handling<br>
V5.11 DIAG4: change parameter handling in DIAG_CreateNewMessage()<br>
<br>Changes to version V4.41:<br>
V5.0 DIAG1: Implementation according ETG.1020 (V1.0.0)<br>
<br>Changes to version V4.40:<br>
V4.41 DIAG1: add variable to store last read diag message, add Subindex3 checking function
*/

/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if DIAGNOSIS_SUPPORTED

#include "ecatslv.h"

#define    _DIAG_
#include "diag.h"
#undef  _DIAG_


/*---------------------------------------------------------------------------------------
------
------    Local Variables
------
---------------------------------------------------------------------------------------*/
UINT8 u8LastDiagMsgRead = 0; /*Local Index to store the last read message*/
/*---------------------------------------------------------------------------------------
------
------    Functions
------
---------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This functions clears the message history.
 In overwrite mode all message are deleted in acknowledge mode only acknowledged message are deleted
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 ClearMessageHistory(void)
{
    UINT8 result = 0;
    if((!(sDiagHistory.Flags & DIAG_OPERATION_MODE))
/*ECATCHANGE_START(V5.11) DIAG2*/
        || (sDiagHistory.u8NewestAckMessage == 0 ))
/*ECATCHANGE_END(V5.11) DIAG2*/
    {
        /*Operation Mode Bit (0x10F3.5 bit4) is 0 => Overwrite Mode is active
         OR no non acknowledged message available (SI4 = 0)*/
        UINT16 cnt = 0;

        for(cnt = 0; cnt < MAX_DIAG_MSG;cnt++)
        {
            aDiagMessages[cnt].u16MessageLength = 0;

            if(aDiagMessages[cnt].pMessage != NULL)
            {
                FREEMEM(aDiagMessages[cnt].pMessage);
                aDiagMessages[cnt].pMessage = NULL;
            }
        }
        sDiagHistory.u16SubIndex0 = 5;

        /*set status entries to startup values*/
        sDiagHistory.u8NewestMessage = 0;
        sDiagHistory.u8NewMessage = 0;
    }
    else
    {
        /*Acknowledge Mode is active and at least one non acknowledged message is available*/
        UINT8 MessagesLeft = 0; //indicates how many messages are left in the message history queue

        if(sDiagHistory.u8NewestAckMessage < sDiagHistory.u8NewestMessage)
        {
            /* messages from SI6 to "Newest Acknowledged Message" will be deleted
               messages from "Newest Acknowledged Message" to "Newest Message" will be shifted to the beginning of the message history queue*/
            UINT16 cnt = 0;
            UINT16 ShiftCnt = 0;

            /*delete messages*/
            for(cnt = 0; cnt < (sDiagHistory.u8NewestAckMessage - 6) ; cnt++)
            {
                aDiagMessages[cnt].u16MessageLength =0;

                if(aDiagMessages[cnt].pMessage != NULL)
                {
                    FREEMEM(aDiagMessages[cnt].pMessage);
                    aDiagMessages[cnt].pMessage = NULL;
                }
            }


            /*shift messages*/
            cnt = 0;
            for(ShiftCnt = (sDiagHistory.u8NewestAckMessage - 6); ShiftCnt < (sDiagHistory.u8NewestMessage -6);ShiftCnt++)
            {
                aDiagMessages[cnt].u16MessageLength = aDiagMessages[ShiftCnt].u16MessageLength;
                aDiagMessages[cnt].pMessage = aDiagMessages[ShiftCnt].pMessage;
                cnt++;

                aDiagMessages[ShiftCnt].u16MessageLength =0;
                aDiagMessages[ShiftCnt].pMessage = NULL;
            }
/*ECATCHANGE_START(V5.11) DIAG2*/
            MessagesLeft = sDiagHistory.u8NewestMessage - sDiagHistory.u8NewestAckMessage;
/*ECATCHANGE_END(V5.11) DIAG2*/
        }
        else
        {
           /* messages from SI6 to "Newest Message" and from "Newest Acknowledged Message" to "Subindex0" will be deleted
              messages from "Newest Message" to "Newest Acknowledged Message" will be  shifted to the beginning of the message history queue*/
            UINT16 cnt = 0;
            UINT16 ShiftCnt = 0;

            /*delete messages SI6 to "Newest Message"*/
            for(cnt = 0; cnt < (sDiagHistory.u8NewestMessage - 6) ; cnt++)
            {
                aDiagMessages[cnt].u16MessageLength =0;

                if(aDiagMessages[cnt].pMessage != NULL)
                {
                    FREEMEM(aDiagMessages[cnt].pMessage);
                    aDiagMessages[cnt].pMessage = NULL;
                }
            }

            /*delete messages "Newest Acknowledged Message" to "Subindex0"*/
            for(cnt = (sDiagHistory.u8NewestAckMessage - 6); cnt <  MAX_DIAG_MSG; cnt++)
            {
                aDiagMessages[cnt].u16MessageLength =0;

                if(aDiagMessages[cnt].pMessage != NULL)
                {
                    FREEMEM(aDiagMessages[cnt].pMessage);
                    aDiagMessages[cnt].pMessage = NULL;
                }
            }

            /*shift messages between "Newest Message" and "Newest Acknowledged Message" to the beginning of the message history queue*/
            cnt = 0;
            for(ShiftCnt = (sDiagHistory.u8NewestMessage - 6); ShiftCnt < (sDiagHistory.u8NewestAckMessage -6);ShiftCnt++)
            {
                aDiagMessages[cnt].u16MessageLength = aDiagMessages[ShiftCnt].u16MessageLength;
                aDiagMessages[cnt].pMessage = aDiagMessages[ShiftCnt].pMessage;
                cnt++;

                aDiagMessages[ShiftCnt].u16MessageLength = 0;
                aDiagMessages[ShiftCnt].pMessage = NULL;
            }

            MessagesLeft = sDiagHistory.u8NewestMessage - sDiagHistory.u8NewestAckMessage;
        }
        
        sDiagHistory.u16SubIndex0 = 5 + MessagesLeft;
/*ECATCHANGE_START(V5.11) DIAG2*/
        sDiagHistory.u8NewestMessage = 5 + MessagesLeft;
/*ECATCHANGE_END(V5.11) DIAG2*/
    }

    /*Reset Overwrite/Discard Indication*/
    sDiagHistory.Flags &= ~((UINT16)DIAG_OVERWRITE_DISCARD);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index                    index of the requested object.
 \param     subindex                subindex of the requested object.
 \param    dataSize                received data size of the SDO Download
 \param    pData                    Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                                     object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function writes the object 0x10F3
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 Write0x10F3( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
    UINT8 result = 0;
    /* Complete Access is not supported for object 0x10F3 */
    if ( bCompleteAccess )
        return ABORTIDX_UNSUPPORTED_ACCESS;

    if(subindex == 3)
    {
        /*Write "Newest Acknowledged Message" (Subindex 3) */
        UINT8 NewValueSi3 = ((UINT8 MBXMEM *)pData)[0];
        
        if(NewValueSi3 == 0)
        {
            /*New value for subindex3 is 0 => Clear Message history*/
            result = ClearMessageHistory();
        }
        else if (NewValueSi3 >= 6 && NewValueSi3 <= (UINT8)sDiagHistory.u16SubIndex0)
        {
            /*Messages need to be acknowledged*/
            sDiagHistory.u8NewestAckMessage = NewValueSi3;

            /*Reset overwrite status bit*/
            sDiagHistory.Flags &= ~((UINT16)DIAG_OVERWRITE_DISCARD);            

            if(sDiagHistory.Flags & DIAG_OPERATION_MODE)
            {
                /*Update "New Messages Available" Indication (only in acknowledge mode); in overwrite mode "New Message Available" is updated when diagnosis messages are read*/
                if(sDiagHistory.u8NewestMessage == sDiagHistory.u8NewestAckMessage)
                    sDiagHistory.u8NewMessage = 0;
            }
        }
        else
        {
            result = ABORTIDX_VALUE_EXCEEDED;
        }

    } //write subindex 3 (newest Acknowledged Message)
    else if (subindex == 5)
    {
        /*
        * Write "Flags"
        * Only Bit 1, Bit 2 and Bit 3 are writeable
        */
        UINT16 ReadOnlyMask = (UINT16)~(DIAG_DISABLE_INFO_MSG | DIAG_DISABLE_WARNING_MSG | DIAG_DISABLE_ERROR_MSG);
        UINT8 Value = (UINT8) *pData;

        if(((UINT16)Value & ReadOnlyMask) == (sDiagHistory.Flags & ReadOnlyMask))
        {
            /*Readonly bits are equal => write new flags value*/    
            sDiagHistory.Flags = (UINT16)Value;
        }
        else
        {
            /*One or more read only bits changed => return error code (Value exceed)*/
            result = ABORTIDX_VALUE_EXCEEDED;
        }
    }
    else if(subindex > (5+ MAX_DIAG_MSG))
    {
        result = ABORTIDX_SUBINDEX_NOT_EXISTING;
    }
    else
    {
        result =  ABORTIDX_READ_ONLY_ENTRY;
    }


    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    index              Index of the requested object.
 \param    subindex           Subindex of the requested object.
 \param    dataSize           Size of the requested object data, calculated with OBJ_GetObjectLength
 \param    pData                    Pointer to the buffer where the data can be copied to
 \param    bCompleteAccess    Indicates if a complete read of all subindices of the
                                     object shall be done or not

 \return    result of the read operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function reads the object 0x10F3
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 Read0x10F3( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{
    if ( bCompleteAccess )
        /* Complete Access is not supported for object 0x10F3 */
        return ABORTIDX_UNSUPPORTED_ACCESS;

    if(subindex == 0)
    {
        *pData = (UINT8)sDiagHistory.u16SubIndex0;
    }
    else if (subindex == 1)
    {
        /* Maximum Messages*/
        *pData = sDiagHistory.u8MaximumMessages;
    }
    else if (subindex == 2)
    {
        /* Newest Message*/
        *pData = sDiagHistory.u8NewestMessage;
    }
    else if (subindex == 3)
    {
        /* Newest Acknowledged Message*/
        *pData = sDiagHistory.u8NewestAckMessage;
    }
    else if (subindex == 4)
    {
        /* New Message Available*/
        *pData = sDiagHistory.u8NewMessage;
    }
    else if (subindex == 5)
    {
        /* Flags*/
        *pData = sDiagHistory.Flags;
    }
    else if(subindex <= sDiagHistory.u16SubIndex0)
    {
        u8LastDiagMsgRead = subindex;

        if(u8LastDiagMsgRead == sDiagHistory.u8NewestMessage)
            sDiagHistory.u8NewMessage = 0;

        if(aDiagMessages[(subindex-6)].u16MessageLength > 0)
            MBXMEMCPY(pData,aDiagMessages[(subindex-6)].pMessage,dataSize);
    }
    else
    {
        return ABORTIDX_SUBINDEX_NOT_EXISTING;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    Initialize array to handle memory for diagnosis messages
*////////////////////////////////////////////////////////////////////////////////////////
void Diag_InitMemory(void)
{
    UINT16 cnt = 0;

    for(cnt = 0; cnt < MAX_DIAG_MSG;cnt++)
    {
        aDiagMessages[cnt].u16MessageLength =0;
        aDiagMessages[cnt].pMessage = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     DiagCode            Diagnosis code according to ETG.1020
 \param     Type                Type of the diagnosis message
                                0 = Info
                                1 = Warning
                                2 = Error
 \param        TextID           Device unique textID defined in the device description (ETG.2000)
 \param        NumParam         Number of parameters which should be added to the message
 \param        pParam           Pointer to the first parameter information
                                The information contains parameter flags according to ETG.1020 and
                                a pointer to local memory which should be added as parameter
                                (Note: the parameter information passed to this function shall be located in continuous memory)

 \brief    This function creates a new diagnosis message
*////////////////////////////////////////////////////////////////////////////////////////
void Diag_CreateNewMessage(UINT32 DiagCode,UINT8 Type,UINT16 TextID,UINT8 NumParam, TDIAGMSGPARAMINFO *pParam)
{
    UINT8 MaskedType = Type & DIAG_MSG_TYPE_MASK;

    /*Check if Diagnosis Message shall be stored in Diagnosis Message queue*/
    
    if(((sDiagHistory.Flags & DIAG_OVERWRITE_DISCARD) == 0 || (sDiagHistory.Flags & DIAG_OPERATION_MODE) == 0) &&
        (((MaskedType == DIAG_MSG_TYPE_INFO) && ((sDiagHistory.Flags & DIAG_DISABLE_INFO_MSG) == 0))
        || ((MaskedType == DIAG_MSG_TYPE_WARNING) && ((sDiagHistory.Flags & DIAG_DISABLE_WARNING_MSG) == 0))
        || ((MaskedType == DIAG_MSG_TYPE_ERROR) && ((sDiagHistory.Flags & DIAG_DISABLE_ERROR_MSG) == 0))))
    {
        /*message buffer is not full or message history is working in Overwrite mode */
        UINT8 NextBuffer = 0;
        TDIAGMESSAGE *pNewDiagMsg = NULL;
        UINT8 cnt = 0;
        TDIAGMSGPARAMINFO *pParamWorking = pParam;

        /* get next buffer if a diagnosis message was already stored */
        if(sDiagHistory.u8NewestMessage > 5)
        {
            /*Get current array index*/
            NextBuffer = sDiagHistory.u8NewestMessage - 6;

            NextBuffer++;

            /*Wrap around if last buffer was used*/
            if(NextBuffer >= MAX_DIAG_MSG)
            {
                NextBuffer = 0;

                if(sDiagHistory.u8NewestAckMessage == 0)
                {
                    /*no message was acknowledged yet => set Overwrite/Discard Flag*/
                    sDiagHistory.Flags |= (UINT16)DIAG_OVERWRITE_DISCARD;
                }
            }

            /*Overrun flag need to be set if the next Message index would be equal to newest acknowledged Message */
            if(sDiagHistory.u8NewestAckMessage == (NextBuffer + 6))
            {
                sDiagHistory.Flags |= (UINT16)DIAG_OVERWRITE_DISCARD;

                if((sDiagHistory.Flags & DIAG_OPERATION_MODE) == 0)
                {
                    /*message history is working in overwrite mode => reset SI3 ("Newest acknowledged Message") and local "LastReadIndex" indication*/
                    sDiagHistory.u8NewestAckMessage = 0;
                    u8LastDiagMsgRead = 0;
                }
            }
        }


        /*Continue if Overwrite/Discard flag is 0 or the message history is handled in Overwrite Mode*/
        if((sDiagHistory.Flags & DIAG_OVERWRITE_DISCARD) == 0 || (sDiagHistory.Flags & DIAG_OPERATION_MODE) == 0)
        {
            aDiagMessages[NextBuffer].u16MessageLength = DIAG_MSG_DEFAULT_LEN;

            if(NumParam > 0 && pParam != NULL)
            {
                UINT16 ParamLength = 0;
                UINT8 ParamType = 0;
                /*Calculate additional buffer length for parameters*/
                for(cnt = 0; cnt < NumParam ; cnt++)
                {
                    ParamLength = 2; //add length for parameter flags
                    ParamType = pParamWorking->ParamFlags >> DIAG_MSG_PARAM_TYPE_OFFSET;

                    if(ParamType == DIAG_MSG_PARAM_TYPE_DATA)
                    {
                        if(((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_INTEGER8)
                            ||((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_UNSIGNED8))
                        {
                            ParamLength += 1;
                        }
                        else if(((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_INTEGER16)
                            ||((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_UNSIGNED16))
                        {
                            ParamLength += 2;
                        }
                        else if(((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_INTEGER32)
                            ||((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_UNSIGNED32))
                        {
                            ParamLength += 4;
                        }
                    }
                    else if((ParamType == DIAG_MSG_PARAM_TYPE_B_ARRY)
                        || (ParamType == DIAG_MSG_PARAM_TYPE_ASCII)
                        || (ParamType == DIAG_MSG_PARAM_TYPE_UNICODE))
                    {
                        /*The length for Byte arrays, ASCII and UNICODE ist stored in bit 0 - 11*/
                        ParamLength += pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK;
                    }
                    /*parameter types 4 -15 reserved for future standardization*/

                    aDiagMessages[NextBuffer].u16MessageLength += ParamLength;

                    /*Get next parameter*/
                    pParamWorking++;
                }
            }//Parameters available

            /*free already allocated buffer*/
            if(aDiagMessages[NextBuffer].pMessage != NULL)
            {
                FREEMEM(aDiagMessages[NextBuffer].pMessage);
                aDiagMessages[NextBuffer].pMessage = NULL;
            }

            /*Allocate required buffer*/
            aDiagMessages[NextBuffer].pMessage = (TDIAGMESSAGE *) ALLOCMEM(aDiagMessages[NextBuffer].u16MessageLength);

            if(aDiagMessages[NextBuffer].pMessage != NULL)
            {
                MBXMEMSET(aDiagMessages[NextBuffer].pMessage,0x00,aDiagMessages[NextBuffer].u16MessageLength);

                /*Initialize new allocated buffer*/
                pNewDiagMsg = (TDIAGMESSAGE *)aDiagMessages[NextBuffer].pMessage;

                pNewDiagMsg->u32DiagCode = SWAPDWORD(DiagCode);
                pNewDiagMsg->u16Flags = SWAPWORD((((UINT16)NumParam) << 8) | Type);
                pNewDiagMsg->u16TextID = SWAPWORD(TextID);

/*ECATCHANGE_START(V5.11) DIAG1*/
#if DC_SUPPORTED
                /*only read time stamp if synchronisation is established (State SAFEOP or OP)*/
                if(bEcatInputUpdateRunning || bEcatOutputUpdateRunning)
                {
                    /*Read the System time register independent if the local application supports DC (if the DC Unit is disabled the return value is 0)*/
                    HW_EscRead((MEM_ADDR *)pNewDiagMsg->TimeStamp,ESC_SYSTEMTIME_OFFSET,8);
                    pNewDiagMsg->TimeStamp[0] = SWAPDWORD(pNewDiagMsg->TimeStamp[0]);
                    pNewDiagMsg->TimeStamp[1] = SWAPDWORD(pNewDiagMsg->TimeStamp[1]);
                }
                else
#endif
/*ECATCHANGE_END(V5.11) DIAG1*/
                {
                    pNewDiagMsg->TimeStamp[0] = 0;
                    pNewDiagMsg->TimeStamp[1] = 0;
                }

                /*copy parameters to message buffer*/
                if(NumParam > 0 && pParam != NULL)
                {
                    UINT8 *pMsgParamBuffer = (UINT8 *)&pNewDiagMsg->Parameter;
                    UINT16 LengthToCopy = 0;
                    UINT8 ParamType;
                    pParamWorking = pParam;

                    for(cnt = 0; cnt < NumParam ; cnt++)
                    {
                        UINT16 u16PFlag = SWAPWORD(pParamWorking->ParamFlags);

                        MBXMEMCPY(pMsgParamBuffer,&u16PFlag,2);
                        pMsgParamBuffer+=2;
                        ParamType = pParamWorking->ParamFlags >> DIAG_MSG_PARAM_TYPE_OFFSET;

                        if(ParamType == DIAG_MSG_PARAM_TYPE_DATA)
                        {
                            if(((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_INTEGER8)
                                ||((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_UNSIGNED8))
                            {
                                LengthToCopy = 1;
                            }
                            else if(((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_INTEGER16)
                                ||((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_UNSIGNED16))
                            {
                                LengthToCopy = 2;
                            }
                            else if(((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_INTEGER32)
                                ||((pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK) == DEFTYPE_UNSIGNED32))
                            {
                                LengthToCopy = 4;
                            }
                        }
                        else if((ParamType == DIAG_MSG_PARAM_TYPE_B_ARRY)
                            || (ParamType == DIAG_MSG_PARAM_TYPE_ASCII)
                            || (ParamType == DIAG_MSG_PARAM_TYPE_UNICODE))
                        {
                            /*The length for Byte arrays, ASCII and UNICODE ist stored in bit 0 - 11*/
                            LengthToCopy = pParamWorking->ParamFlags & ~DIAG_MSG_PARAM_TYPE_MASK;
                        }
                        /*parameter types 4 -15 reserved for future standardization*/

                        /*Copy Parameter Value to message buffer*/
                        if(LengthToCopy > 0 )
                        {
/* ECATCHANGE_START(V5.11) DIAG4*/
                            MBXMEMCPY(pMsgParamBuffer,pParamWorking->pParam,LengthToCopy);
/* ECATCHANGE_END(V5.11) DIAG4*/
                            pMsgParamBuffer += LengthToCopy;
                            LengthToCopy = 0;
                        }
                        /*Get next parameter*/
                        pParamWorking++;
                    }
                }

                /*Update newest message index */
                sDiagHistory.u8NewestMessage = (NextBuffer + 6);

                /*indicate new DiagMessage*/
                sDiagHistory.u8NewMessage = 1;



                 /*increment subindex 0 until all message entries used at least once*/
                if(sDiagHistory.u16SubIndex0 < (MAX_DIAG_MSG +5))
                {
                    sDiagHistory.u16SubIndex0++;
                }
            }
            else
            {
                /*No valid buffer allocated*/
                aDiagMessages[NextBuffer].u16MessageLength = 0;
            }
        }// no Overwrite/Discard flag or message history is working in overwrite mode
    }
}

#endif // #if DIAGNOSIS_SUPPORTED

/** @} */

