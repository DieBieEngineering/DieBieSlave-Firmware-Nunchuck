/**
\addtogroup SoE Servo Drive Profile over EtherCAT
@{
*/

/**
\file ecatsoe.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file shows a short example for SoE

\version 5.10

<br>Changes to version V5.0:<br>
V5.10 MBX1: Remove multiple swapping of length filed in mailbox header<br>
<br>Changes to version V4.40:<br>
V5.0 SOE1: Support SoE without emergency handling.<br>
<br>Changes to version V3.00:<br>
V4.40 MBX6: Change return value if no mailbox buffer is available<br>
V4.40 SOE1: adapt code to mailbox structure changes
*/

/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if SOE_SUPPORTED

#include "ecatslv.h"

#define _ECATSOE_ 1
#include "ecatsoe.h"
#undef _ECATSOE_ 
#define _ECATSOE_ 0

#if EMERGENCY_SUPPORTED
#include "emcy.h"
#endif


/*---------------------------------------------------------------------------------------
------
------    Functions
------
---------------------------------------------------------------------------------------*/
#endif


#if SOE_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pReceiveMbx      Pointer to the received mailbox data from the master.

 \brief    This function is called when a ServoDriveProfile over EtherCAT service is received from
             the master.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 SOE_ServiceInd(TMBX MBXMEM *pReceiveMbx)
{
    UINT8 result = 0;
    TSOEMBX MBXMEM *pSoeMbx = (TSOEMBX MBXMEM *) pReceiveMbx;

    if(SWAPWORD(pSoeMbx->MbxHeader.Length) < SOE_HEADER_SIZE)
        return MBXERR_INVALIDSIZE;

    switch (pSoeMbx->SoeHeader.Flags.Word & SOEFLAGS_OPCODE)
    {
    case ECAT_SOE_OPCODE_RRQ:
        SOE_ReadInd( pSoeMbx );
        break;

    case ECAT_SOE_OPCODE_WRQ:
        SOE_WriteInd( pSoeMbx );
        break;

    default:
        result = MBXERR_INVALIDHEADER;
        break;
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx      Pointer to the free mailbox to sent.

 \brief    This function is called when the next mailbox fragment can be stored the ESC.
*////////////////////////////////////////////////////////////////////////////////////////

void SOE_ContinueInd(TMBX MBXMEM * pMbx)
{
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pSoeMbx    Pointer to the received mailbox data from the master.

 \return    Indicates if an error occurred while the operation ( good = 0 ).

 \brief    This function is called when a SDO request service
            is received from the master and calls depending from
            the command the concerning function.
*////////////////////////////////////////////////////////////////////////////////////////

void SOE_ReadInd(TSOEMBX MBXMEM *pSoeMbx)
{
    UINT32 data=0x12345678;
    pSoeMbx->SoeHeader.Flags.Word &= ~SOEFLAGS_OPCODE;
    pSoeMbx->SoeHeader.Flags.Word |= ECAT_SOE_OPCODE_RRS;
    MBXMEMCPY( ( UINT8 MBXMEM * ) &pSoeMbx->Data[0], ( UINT8 MBXMEM * ) &data, SIZEOF( data ) );
    pSoeMbx->MbxHeader.Length     = SOE_HEADER_SIZE + SIZEOF(data);

    MBX_MailboxSendReq( (TMBX MBXMEM *) pSoeMbx, SOE_SERVICE);
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pSoeMbx    Pointer to the received mailbox data from the master.

 \return    Indicates if an error occurred while the operation ( good = 0 ).

 \brief    This function is called when a SDO request service
            is received from the master and calls depending from
            the command the concerning function.
*////////////////////////////////////////////////////////////////////////////////////////

void SOE_WriteInd(TSOEMBX MBXMEM *pSoeMbx)
{
    UINT16 dataLen;
    UINT16 errorCode = 0;

    dataLen     = SWAPWORD(pSoeMbx->MbxHeader.Length) - SOE_HEADER_SIZE;

    pSoeMbx->SoeHeader.Flags.Word &= ~SOEFLAGS_OPCODE;
    pSoeMbx->SoeHeader.Flags.Word |= ECAT_SOE_OPCODE_WRS;
    pSoeMbx->MbxHeader.Length = SOE_HEADER_SIZE;
    if ( dataLen < SIZEOF(UINT32) )
        errorCode = 0x7002;
    else if ( dataLen > SIZEOF(UINT32) )
        errorCode = 0x7003;
    if ( errorCode != 0 )
    {
        pSoeMbx->SoeHeader.Flags.Word |= SOEFLAGS_ERROR;
        pSoeMbx->MbxHeader.Length      += SIZEOF( errorCode );
        MBXMEMCPY( (UINT8 MBXMEM *) &pSoeMbx->Data[0], ( UINT8 MBXMEM * ) &errorCode, SIZEOF(errorCode) );
    }
    MBX_MailboxSendReq( (TMBX MBXMEM *) pSoeMbx,SOE_SERVICE);
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pSoeMbx    Pointer to the received mailbox data from the master.

 \return    Indicates if an error occurred while the operation ( good = 0 ).

 \brief    This function is called from the SoE application if a pending command
            was successfully finished.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 SOE_NotificationReq(UINT16 idn, UINT16 channel, UINT16 dataState)
{
#if EMERGENCY_SUPPORTED
    TEMCYMESSAGE EMCYMEM *    pNotiReq    = EMCY_GetEmcyBuffer();

    if (pNotiReq)
    {
        ((TSOEHEADER EMCYMEM *) &pNotiReq->SoeHeader)->Flags.Word      = 0;
        ((TSOEHEADER EMCYMEM *) &pNotiReq->SoeHeader)->Flags.Word     |= ECAT_SOE_OPCODE_NFC;
        ((TSOEHEADER EMCYMEM *) &pNotiReq->SoeHeader)->Flags.Word    |= (channel<<5) & SOEFLAGS_DRIVENO;
        ((TSOEHEADER EMCYMEM *) &pNotiReq->SoeHeader)->Flags.Word   |= SOEFLAGS_DATASTATE;
        ((TSOEHEADER EMCYMEM *) &pNotiReq->SoeHeader)->IDN_Frag                    = idn;
        pNotiReq->Emcy.Code = dataState;

        EMCY_SendEmergency(pNotiReq);

        return 0;
    }
    else
    {
        return MBXERR_NOMOREMEMORY;    // error, no buffer!
    }
#else
    return MBXERR_SERVICENOTSUPPORTED;
#endif
}


#endif // SOE_SUPPORTED
/** @} */

