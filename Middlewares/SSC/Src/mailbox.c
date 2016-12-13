/**
\addtogroup Mailbox Mailbox Functions
@{
*/

/**
\file    mailbox.c
\author EthercatSSC@beckhoff.com
\brief Implementation

<br>Description of the mailbox buffer handling (MAILBOX_QUEUE = 0):<br>
There are two mailbox buffer for sending and receiving mailbox services.<br>
Normal operation:<br>
When starting the mailbox handler psWriteMbx contains mailbox buffer 1,<br>
psReadMbx, psRepeatMbx and psStoreMbx are 0.<br>
In this state a repeat request would be ignored because there was no service sent yet.<br>
When the first mailbox service is sent (in MBX_CopyToSendMailbox), psWriteMbx gets mailbox buffer 2<br>
and psReadMbx gets the sent mailbox buffer 1, psRepeatMbx and psStoreMbx are still 0.<br>
When the first mailbox service was read from the master, the sent mailbox buffer 1 is stored<br>
in psRepeatMbx (in MBX_MailboxReadInd).<br>
After that psReadMbx gets always the actual sent mailbox buffer, psWriteMbx is set to 0 (another<br>
received mailbox service from the master will not be handled until the sent mailbox service was read<br>
and MBX_MailboxReadInd was called).<br>
When the mailbox service is read, psWriteMbx gets the Buffer of psRepeatMbx and psRepeatMbx gets the <br>
buffer of psReadMbx.<br>
Repeat Request from the master:<br>
When a Repeat from the master is requested (MBX_MailboxRepeatReq), there are three different possibilities:<br>
1. no mailbox service was sent since the mailbox handler was started (psRepeatMbx = 0): nothing to do<br>
2. the acknowledge of the last sent mailbox service was received (in MBX_MailboxReadInd) (bSendMbxIsFull = 0):<br>
the last sent mailbox service (psRepeatMbx) will be sent again (in MBX_CopyToSendMailbox) and stored in psReadMbx,<br>
psRepeatMbx will be set to 0<br>
3. the acknowledge of the last sent mailbox service was not received (psReadMbx and psRepeatMbx contain different buffers,<br>
psReadMbx is still in the mailbox (because MBX_MailboxReadInd is not called yet, bSendMbxIsFull = 1): <br>
psReadMbx will be deleted in the mailbox (call of DisableSyncManChannel and EnableSyncManChannel) and<br>
stored in psStoreMbx, psRepeatMbx will be sent again (in MBX_CopyToSendMailbox) and stored in psReadMbx,<br>
psRepeatMbx will be set to 0.<br>
When the repeated mailbox service was sent (call of MBX_MailboxReadInd), psReadMbx will be stored in psRepeatMbx<br>
and psStoreMbx will be sent (in MBX_CopyToSendMailbox) and stored in psReadMbx, psStoreMbx will be set to 0.<br>

<br>Description of the mailbox buffer handling (MAILBOX_QUEUE = 1):<br>
There are two mailbox buffer for sending and receiving mailbox services.<br>
Normal operation (psWriteMbx is only used for local storage):<br>
When starting the mailbox handler psReadMbx, psRepeatMbx and psStoreMbx are 0.<br>
In this state a repeat request would be ignored because there was no service sent yet.<br>
When a mailbox service is received from the master (in MBX_CheckAndCopyMailbox) a mailbox buffer<br>
will be get with APPL_AllocMailboxBuffer and the corresponding protocol service function will<br>
be called (in MBX_WriteMailboxInd). This buffer shall be used for the protocol service response.<br>
When the first mailbox service is sent (in MBX_CopyToSendMailbox), psReadMbx gets the sent mailbox buffer,<br>
psRepeatMbx and psStoreMbx are still 0.<br>
When the first mailbox service was read from the master, the sent mailbox buffer (psReadMbx) is stored<br>
in psRepeatMbx (in MBX_MailboxReadInd).<br>
After that psReadMbx gets always the actual sent mailbox buffer (in MBX_CopyToSendMailbox)<br>
When the mailbox service is read, psRepeatMbx is returned (with APPL_FreeMailboxBuffer) and psRepeatMbx gets the <br>
buffer of psReadMbx.<br>
Repeat Request from the master:<br>
When a Repeat from the master is requested (MBX_MailboxRepeatReq), there are three different possibilities:<br>
1. no mailbox service was sent since the mailbox handler was started (psRepeatMbx = 0): nothing to do<br>
2. the acknowledge of the last sent mailbox service was received (in MBX_MailboxReadInd) (bSendMbxIsFull = 0):<br>
the last sent mailbox service (psRepeatMbx) will be sent again (in MBX_CopyToSendMailbox) and stored in psReadMbx,<br>
psRepeatMbx will be set to 0<br>
3. the acknowledge of the last sent mailbox service was not received (psReadMbx and psRepeatMbx contain different buffers,<br>
psReadMbx is still in the mailbox (because MBX_MailboxReadInd is not called yet, bSendMbxIsFull = 1): <br>
psReadMbx will be deleted in the mailbox (call of DisableSyncManChannel and EnableSyncManChannel) and<br>
stored in psStoreMbx, psRepeatMbx will be sent again (in MBX_CopyToSendMailbox) and stored in psReadMbx,<br>
psRepeatMbx will be set to 0.<br>
When the repeated mailbox service was sent (call of MBX_MailboxReadInd), psReadMbx will be stored in psRepeatMbx<br>
and psStoreMbx will be sent (in MBX_CopyToSendMailbox) and stored in psReadMbx, psStoreMbx will be set to 0.

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 ECAT7: add missing big endian swapping<br>
V5.11 HW1: "move hardware independent functions ""HW_DisableSyncManChannel()"", ""HW_EnableSyncManChannel()"", ""HW_GetSyncMan()"", ""HW_ResetALEventMask()"", ""HW_SetALEventMask()"" to ecatalv.c"<br>
V5.11 MBX2: "check in IP if enough dynamic memory is available to handle mailbox communication, if it is not the case the mbx error ""no memory"" is returned on any mbx request"<br>
V5.11 MBX3: set application triggered emergency and EoE data to pending if no mailbox queue is supported and another mailbox request is currently handled, Handle only one mailbox request at a time (in case that MAILBPX_QUEUE is disabled)<br>
V5.11 TEST4: add new mailbox test behaviour (the master mailbox cnt shall be incremented by 1 and the slave mailbox cnt is alternating)<br>
<br>Changes to version V5.01:<br>
V5.10 EOE3: Change local send frame pending indication variable to a global variable (it need to be resetted if the mailbox is stopped and a frame is pending)<br>
V5.10 ESC6: Update SyncManager register size (only for 8Bit ESC access)<br>
V5.10 MBX2: Allocate only memory for current configured mailbox size (max supported size was allocated before)<br>
<br>Changes to version V5.0:<br>
V5.01 ESC2: Add missed value swapping<br>
V5.01 MBX2: Return an error if length specified in mailbnox header is too large.<br>
<br>Changes to version V4.40:<br>
V5.0 ECAT3: Global dummy variables used for dummy ESC operations.<br>
V5.0 ESC1: ESC 32Bit Access added.<br>
V5.0 MBX2: Prevent double swapping (MBX repeat service) of MBX length field.<br>
V5.0 MBX4: Unlock SM buffer based on SM state.<br>
<br>Changes to version V4.11:<br>
V4.40 MBX8: Handle pending mailbox data even if mailbox queue is enabled<br>
V4.40 MBX7: Reset SyncManger 1 Repeat Acknowledge bit if local variable is reset<br>
V4.40 EMCY2: Send queued emergency messages<br>
V4.40 BOOT2: Support only FoE in Bootstrap state<br>
V4.40 MBX5: Prevent multiple free buffer<br>
V4.40 MBX6: change return value if no mailbox buffer is available<br>
<br>Changes to version V4.08:<br>
V4.11 MBX 1: free some dynamic memory to avoid memory leaks in the mailbox queue<br>
<br>Changes to version V4.07:<br>
V4.10 MBX 1: check Mailbox message buffer and print emergency messages<br>
               during state change Init -> PreOP<br>
<br>Changes to version V4.07:<br>
V4.08 MBX 1: The mailbox queue was added (switch MAILBOX_QUEUE), if the<br>
             mailbox should be handled in the ESC-Interrupt Service Routine<br>
             the defines ENTER_MBX_CRITICAL and LEAVE_MBX_CRITICAL could be used<br>
             to disable the interrupts or change the priority<br>
V4.08 AOE 1:    AoE was added<br>
<br>Changes to version V4.06:<br>
V4.07 ECAT 1: The sources for SPI and MCI were merged (in ecat_def.h<br>
                   set the switch MCI_HW to 1 when using the MCI,<br>
                   set the switch SPI_HW to 1 when using the SPI
*/


/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if MAILBOX_SUPPORTED

#include "ecatslv.h"

#if TEST_APPLICATION
#include "testappl.h"
#endif

#define    _MAILBOX_    1
#include "mailbox.h"
#undef _MAILBOX_
/* ECATCHANGE_START(V5.11) ECAT10*/
/*remove definition of _MAILBOX_ (#ifdef is used in mailbox.h)*/
/* ECATCHANGE_END(V5.11) ECAT10*/

#if SOE_SUPPORTED
#include "ecatsoe.h"
#endif
#if AOE_SUPPORTED
#include "ecataoe.h"
#endif
#if COE_SUPPORTED
#include "ecatcoe.h"
#endif
#if EOE_SUPPORTED
#include "ecateoe.h"
#endif
#if FOE_SUPPORTED
#include "ecatfoe.h"
#endif
#if EMERGENCY_SUPPORTED
#include "emcy.h"
#endif

/*--------------------------------------------------------------------------------------
------
------    internal Types and Defines
------
--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
------
------    internal Variables
------
--------------------------------------------------------------------------------------*/
/*variables are declared in ecatslv.c*/
#if ESC_32BIT_ACCESS
    extern VARVOLATILE UINT32    u32dummy;
#elif ESC_16BIT_ACCESS
    extern VARVOLATILE UINT16    u16dummy;
#else
    extern VARVOLATILE UINT8 u8dummy;
#endif

#if MAILBOX_QUEUE
/* ECATCHANGE_START(V5.11) MBX2*/
    BOOL bNoMbxMemoryAvailable; /**< \brief Indicates if enough dynamic memory is available to handle at least one mailbox datagram */
/* ECATCHANGE_END(V5.11) MBX2*/
#endif
/*--------------------------------------------------------------------------------------
------
------    internal functions
------
--------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
------
------    functions
------
---------------------------------------------------------------------------------------*/


#if MAILBOX_QUEUE
///////////////////////////////////////////////////////////////////////////////////////////
//
//    PutInMbxQueue
//

UINT8 PutInMbxQueue(TMBX MBXMEM * pMbx, TMBXQUEUE MBXMEM * pQueue)
{
    UINT16 lastInQueue;
    ENTER_MBX_CRITICAL;


    lastInQueue = pQueue->lastInQueue+1;
    if (lastInQueue == pQueue->maxQueueSize)
    {
        // Umbruch der Queue
        lastInQueue = 0;
    }

    if (pQueue->firstInQueue == lastInQueue)
    {
        // Ueberlauf der Queue -> letztes Element wieder herausnehmen
        LEAVE_MBX_CRITICAL;
        return MBXERR_NOMOREMEMORY;
    }

    pQueue->queue[pQueue->lastInQueue] = pMbx;
    pQueue->lastInQueue = lastInQueue;

    LEAVE_MBX_CRITICAL;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//    GetOutOfMbxQueue
//

TMBX MBXMEM * GetOutOfMbxQueue(TMBXQUEUE MBXMEM * pQueue)
{
    TMBX MBXMEM * pMbx;
    ENTER_MBX_CRITICAL;

    if (pQueue->firstInQueue != pQueue->lastInQueue)
    {
        // Queue ist nicht leer
        UINT16 firstInQueue = pQueue->firstInQueue;
        pMbx = pQueue->queue[firstInQueue];
        firstInQueue++;
        pQueue->firstInQueue = firstInQueue;
        if (pQueue->firstInQueue == pQueue->maxQueueSize)
        {
            // Umbruch der Queue
            pQueue->firstInQueue = 0;
        }
    }
    else
        pMbx = 0;


    LEAVE_MBX_CRITICAL;

    return pMbx;
}

#endif // MAILBOX_QUEUE
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    This function intialize the Mailbox Interface.
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_Init(void)
{
    u16ReceiveMbxSize = MIN_MBX_SIZE;
    u16SendMbxSize = MAX_MBX_SIZE;
    u16EscAddrReceiveMbx = MIN_MBX_WRITE_ADDRESS;
    u16EscAddrSendMbx = MIN_MBX_READ_ADDRESS;

#if MAILBOX_QUEUE
    sMbxReceiveQueue.firstInQueue    = 0;
    sMbxReceiveQueue.lastInQueue     = 0;
    sMbxReceiveQueue.maxQueueSize = MAX_MBX_QUEUE_SIZE;
    sMbxSendQueue.firstInQueue        = 0;
    sMbxSendQueue.lastInQueue         = 0;
    sMbxSendQueue.maxQueueSize     = MAX_MBX_QUEUE_SIZE;
    psWriteMbx  = NULL;
#else // MAILBOX_QUEUE
    psWriteMbx = &asMbx[0];
#endif // MAILBOX_QUEUE

#if EOE_SUPPORTED
    bEoESendFramePending = FALSE;
#endif

    psRepeatMbx = NULL;
    psReadMbx    = NULL;
    psStoreMbx    = NULL;

    bMbxRepeatToggle    = FALSE;
    /*Reset Repeat acknowledge bit of SyncManager1 (0x80F bit 2)*/
#if ESC_32BIT_ACCESS
     {
        UINT32 sm1Activate = 0;
        HW_EscReadDWord(sm1Activate,(ESC_SYNCMAN_CONTROL_OFFSET + SIZEOF_SM_REGISTER));
        sm1Activate &= SWAPDWORD(~0x02000000);
        HW_EscWriteDWord(sm1Activate,(ESC_SYNCMAN_CONTROL_OFFSET + SIZEOF_SM_REGISTER));
    }
#elif ESC_16BIT_ACCESS
    {
        UINT16 sm1Activate = 0;
        HW_EscReadWord(sm1Activate,(ESC_SYNCMAN_ACTIVE_OFFSET + SIZEOF_SM_REGISTER));
        sm1Activate &= SWAPWORD(~0x0200);
        HW_EscWriteWord(sm1Activate,(ESC_SYNCMAN_ACTIVE_OFFSET + SIZEOF_SM_REGISTER));
    }
#else
    {
        UINT8 sm1Activate = 0;
        HW_EscReadByte(sm1Activate,(ESC_SM_PDICONTROL_OFFSET + SIZEOF_SM_REGISTER));
        sm1Activate &= ~0x02;
        HW_EscWriteByte(sm1Activate,(ESC_SM_PDICONTROL_OFFSET + SIZEOF_SM_REGISTER));
    }
#endif
    bMbxRunning = FALSE;
    bSendMbxIsFull = FALSE;
    bReceiveMbxIsLocked = FALSE;
    u8MailboxSendReqStored    = 0;
    u8MbxWriteCounter = 0;
    u8MbxReadCounter    = 0;
#if !MAILBOX_QUEUE
/*ECATCHANGE_START(V5.11) MBX3*/
    bPendingMbxResponse = FALSE;
/*ECATCHANGE_END(V5.11) MBX3*/
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief     This function includes the state transition from INIT to
 \brief     PRE-OPERATIONAL in the EtherCAT Slave corresponding to
 \brief     local management service Start Mailbox Handler
 \brief     it is checked if the mailbox areas overlaps each other
 \brief     and the Sync Manager channels 0 and 1 are enabled.
 \brief     This function shall only be called if mailbox is supported.
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 MBX_StartMailboxHandler(void)
{
    UINT16 result = 0;
    /* get address of the receive mailbox sync manager (SM0) */
/*ECATCHANGE_START(V5.11) HW1*/
    TSYNCMAN ESCMEM * pSyncMan = (TSYNCMAN ESCMEM *)GetSyncMan(MAILBOX_WRITE);
/*ECATCHANGE_END(V5.11) HW1*/
#if ESC_32BIT_ACCESS
    /* store size of the receive mailbox */
    u16ReceiveMbxSize     = (UINT16) ((pSyncMan->AddressLength & SM_LENGTH_MASK) >>SM_LENGTH_SHIFT);
    /* store the address of the receive mailbox */
    u16EscAddrReceiveMbx = (UINT16) (pSyncMan->AddressLength & SM_ADDRESS_MASK);
#else
    /* store size of the receive mailbox */
    u16ReceiveMbxSize     = pSyncMan->Length;
    /* store the address of the receive mailbox */
    u16EscAddrReceiveMbx = pSyncMan->PhysicalStartAddress;
#endif

    /* get address of the send mailbox sync manager (SM1) */
/*ECATCHANGE_START(V5.11) HW1*/
    pSyncMan =(TSYNCMAN ESCMEM *) GetSyncMan(MAILBOX_READ);
/*ECATCHANGE_END(V5.11) HW1*/

#if ESC_32BIT_ACCESS
    /* store the size of the send mailbox */
    u16SendMbxSize = (UINT16) ((pSyncMan->AddressLength & SM_LENGTH_MASK) >>SM_LENGTH_SHIFT);
    /* store the address of the send mailbox */
    u16EscAddrSendMbx = (UINT16) (pSyncMan->AddressLength & SM_ADDRESS_MASK);
#else
    /* store the size of the send mailbox */
    u16SendMbxSize = pSyncMan->Length;
    /* store the address of the send mailbox */
    u16EscAddrSendMbx = pSyncMan->PhysicalStartAddress;
#endif

    // HBu 02.05.06: it should be checked if there are overlaps in the sync manager areas
    if ((u16EscAddrReceiveMbx + u16ReceiveMbxSize) > u16EscAddrSendMbx && (u16EscAddrReceiveMbx < (u16EscAddrSendMbx + u16SendMbxSize)))
    {
        return ALSTATUSCODE_INVALIDMBXCFGINPREOP;
    }
#if AOE_SUPPORTED
    u16AmsFragSize = u16ReceiveMbxSize;
    if ( u16AmsFragSize > u16SendMbxSize )
        u16AmsFragSize = u16SendMbxSize;

    u16AmsFragSize -= MBX_HEADER_SIZE;
#endif

    /* enable the receive mailbox sync manager channel */
/*ECATCHANGE_START(V5.11) HW1*/
    EnableSyncManChannel(MAILBOX_WRITE);
    /* enable the send mailbox sync manager channel */
    EnableSyncManChannel(MAILBOX_READ);
/*ECATCHANGE_END(V5.11) HW1*/

#if MAILBOX_QUEUE
/* ECATCHANGE_START(V5.11) MBX2*/
        psWriteMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer(u16ReceiveMbxSize);
        if(psWriteMbx == NULL)
        {
            bNoMbxMemoryAvailable = TRUE;

            //check if at least enough memory for an mailbox error is available (other wise stop the state transition)
            psWriteMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer(10); /* a mailbox error datagram length*/
            if(psWriteMbx == NULL)
            {
                result = ALSTATUSCODE_NOMEMORY;
            }
           
            APPL_FreeMailboxBuffer(psWriteMbx);
        }
        else
        {
            bNoMbxMemoryAvailable = FALSE;
            APPL_FreeMailboxBuffer(psWriteMbx);
        }
/* ECATCHANGE_END(V5.11) MBX2*/
#endif

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function includes the state transition from
 \brief    PRE-OPERATIONAL to INIT in the EtherCAT Slave corresponding to
 \brief  local management service Stop Mailbox Handler
 \brief  the Sync Manager channels 0 and 1 are disabled
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_StopMailboxHandler(void)
{
#if MAILBOX_QUEUE
    TMBX MBXMEM * pMbx;

#endif
    /* mailbox handler is stopped */
    bMbxRunning = FALSE;
    /* disable the receive mailbox sync manager channel */
/*ECATCHANGE_START(V5.11) HW1*/
    DisableSyncManChannel(MAILBOX_WRITE);
    /* disable the send mailbox sync manager channel */
    DisableSyncManChannel(MAILBOX_READ);
/*ECATCHANGE_END(V5.11) HW1*/
    /* initialize variables again */

#if MAILBOX_QUEUE

    if (psRepeatMbx != NULL)
        APPL_FreeMailboxBuffer(psRepeatMbx);

    if (psStoreMbx != NULL && psStoreMbx != psRepeatMbx)
        APPL_FreeMailboxBuffer(psStoreMbx);

    if (psReadMbx != NULL && psReadMbx != psRepeatMbx && psReadMbx != psStoreMbx)
        APPL_FreeMailboxBuffer(psReadMbx);
#else
    psWriteMbx = &asMbx[0];
#endif

#if EOE_SUPPORTED
    bEoESendFramePending = FALSE;
#endif

    psRepeatMbx = NULL;
    psReadMbx = NULL;
    psStoreMbx = NULL;

    bMbxRepeatToggle    = FALSE;
    /*Reset Repeat acknowledge bit of SyncManager1 (0x080F bit 2)*/
#if ESC_32BIT_ACCESS
     {
        UINT32 sm1Activate = 0;
        HW_EscReadDWord(sm1Activate,(ESC_SYNCMAN_CONTROL_OFFSET + SIZEOF_SM_REGISTER));
        sm1Activate &= SWAPDWORD(~0x02000000);
        HW_EscWriteDWord(sm1Activate,(ESC_SYNCMAN_CONTROL_OFFSET + SIZEOF_SM_REGISTER));
    }
#elif ESC_16BIT_ACCESS
    {
        UINT16 sm1Activate = 0;
        HW_EscReadWord(sm1Activate,(ESC_SYNCMAN_ACTIVE_OFFSET + SIZEOF_SM_REGISTER));
        sm1Activate &= SWAPWORD(~0x0200);
        HW_EscWriteWord(sm1Activate,(ESC_SYNCMAN_ACTIVE_OFFSET + SIZEOF_SM_REGISTER));
    }
#else
    {
        UINT8 sm1Activate = 0;
        HW_EscReadByte(sm1Activate,(ESC_SM_PDICONTROL_OFFSET + SIZEOF_SM_REGISTER));
        sm1Activate &= ~0x02;
        HW_EscWriteByte(sm1Activate,(ESC_SM_PDICONTROL_OFFSET + SIZEOF_SM_REGISTER));
    }
#endif
    bSendMbxIsFull         = FALSE;
    bReceiveMbxIsLocked = FALSE;
    u8MailboxSendReqStored    = 0;
    u8MbxWriteCounter         = 0;
    u8MbxReadCounter        = 0;

#if MAILBOX_QUEUE
    do
    {
        pMbx = GetOutOfMbxQueue(&sMbxReceiveQueue);
        if (pMbx)
        {
            APPL_FreeMailboxBuffer(pMbx);
            pMbx = NULL;
        }
    } while (pMbx != NULL);
    do
    {
        pMbx = GetOutOfMbxQueue(&sMbxSendQueue);
        if (pMbx)
        {
            APPL_FreeMailboxBuffer(pMbx);
            pMbx = NULL;
        }
    } while (pMbx != NULL);
#else
/*ECATCHANGE_START(V5.11) MBX3*/
    bPendingMbxResponse = FALSE;
/*ECATCHANGE_END(V5.11) MBX3*/
#endif

#if EMERGENCY_SUPPORTED
    EMCY_Init();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx        Pointer to the received Mailbox command from Master.

 \brief    The function checks the mailbox header for the requested service and calls the
 \brief    corresponding XXXX_ServiceInd-function
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 MailboxServiceInd(TMBX MBXMEM *pMbx)
{
    UINT8 result;

#if BOOTSTRAPMODE_SUPPORTED
    /*only FoE is allowed in Boot mode*/
    if(bBootMode == TRUE && (MBX_TYPE_FOE != ((pMbx->MbxHeader.Flags[MBX_OFFS_TYPE] & MBX_MASK_TYPE) >> MBX_SHIFT_TYPE )))
        return MBXERR_UNSUPPORTEDPROTOCOL;
#endif

    switch ( (pMbx->MbxHeader.Flags[MBX_OFFS_TYPE] & MBX_MASK_TYPE) >> MBX_SHIFT_TYPE )
    {
#if AOE_SUPPORTED
    case MBX_TYPE_AOE:
        /* AoE datagram received */
        result = AOE_ServiceInd(pMbx);
        break;

#endif /* AOE_SUPPORTED */
#if COE_SUPPORTED
    case MBX_TYPE_COE:
        /* CoE datagram received */
        result = COE_ServiceInd((TCOEMBX MBXMEM *) pMbx);
        break;

#endif
#if SOE_SUPPORTED
    case MBX_TYPE_SOE:
        /* SoE datagram received */
        result = SOE_ServiceInd(pMbx);
        break;

#endif /* SOE_SUPPORTED */
#if EOE_SUPPORTED
    case MBX_TYPE_EOE:
        /* EoE datagram received */
        result = EOE_ServiceInd(pMbx);
        break;

#endif /* EOE_SUPPORTED */
#if FOE_SUPPORTED
    case MBX_TYPE_FOE:
        /* FoE datagram received */
        result = FOE_ServiceInd((TFOEMBX MBXMEM *) pMbx);
        break;

#endif /* FOE_SUPPORTED */
#if VOE_SUPPORTED
    case MBX_TYPE_VOE:
        /* VoE datagram received */
        result = VOE_ServiceInd((TVOEMBX MBXMEM *) pMbx);
        break;

#endif /* VOE_SUPPORTED */
    default:

        result = MBXERR_UNSUPPORTEDPROTOCOL;
        break;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx        Pointer to the received Mailbox command from Master.

 \brief    This function is called when the Master has written the Receive-Mailbox.
 \brief    It will only be called if the send mailbox is empty, that a response for the
 \brief    mailbox service could be stored.
 \brief    The function checks the mailbox header for the requested service and calls the
 \brief    corresponding XXXX_ServiceInd-function
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_MailboxWriteInd(TMBX MBXMEM *pMbx)
{
    UINT8 result = 0;
    UINT8 mbxCounter = pMbx->MbxHeader.Flags[MBX_OFFS_COUNTER] >> MBX_SHIFT_COUNTER;
    UINT16 MbxLen = SWAPWORD(pMbx->MbxHeader.Length);

    if(MbxLen > MAX_MBX_SIZE)
    {
        /* Mailbox error response: size specified in mailbox header too large*/
        pMbx->MbxHeader.Length     = 4;
        pMbx->MbxHeader.Flags[MBX_OFFS_TYPE]        &= ~MBX_MASK_TYPE;
        pMbx->Data[0]                        = SWAPWORD(MBXSERVICE_MBXERRORCMD);
        pMbx->Data[1]                        = SWAPWORD(MBXERR_INVALIDSIZE);
        MBX_MailboxSendReq(pMbx, 0);
    }
    else
    /* if the mailbox datagram counter (Bit 4-6 of Byte 5 of the mailbox header) is unequal zero,
       the master supports the mailbox data link layer,
        in that case a repeated mailbox write request will be detected, if the counter is unequal zero
        and unchanged */
    if ( mbxCounter == 0 || mbxCounter != u8MbxWriteCounter )
    {
#if TEST_APPLICATION
/*ECATCHANGE_START(V5.11) TEST4*/
        if(IS_TEST_ACTIVE(Mbx_MasterMbxCntIncByOne))
        {
            if(mbxCounter != (u8MbxWriteCounter + 1))
            {
                /* the master mailbox counter is not incremented by one => return an error*/
                INC_TEST_CNT(Mbx_MasterMbxCntIncByOne);
                
                /* Mailbox error response: type 0 (mailbox service protocol) */
                pMbx->MbxHeader.Length     = 4;
                pMbx->MbxHeader.Flags[MBX_OFFS_TYPE]        &= ~MBX_MASK_TYPE;
                pMbx->Data[0]                        = SWAPWORD(MBXSERVICE_MBXERRORCMD);
                pMbx->Data[1]                        = SWAPWORD(MBXERR_SYNTAX);
                MBX_MailboxSendReq(pMbx, 0);
                return;
            }
        }
/*ECATCHANGE_END(V5.11) TEST4*/
#endif
        /* new mailbox service received */
        /* mbxCounter = 0: old EtherCAT master */
        /* new MBX service received, store the new mailbox counter */
        u8MbxWriteCounter = mbxCounter;

        {
            /* check the protocol type and call the XXXX_ServiceInd-function */

#if MAILBOX_QUEUE
            result = PutInMbxQueue(pMbx, &sMbxReceiveQueue);
#else
            result = MailboxServiceInd(pMbx);
#endif // else MAILBOX_QUEUE
        }

        if ( result != 0 )
        {
            /* Mailbox error response: type 0 (mailbox service protocol) */
            pMbx->MbxHeader.Length     = 4;
            pMbx->MbxHeader.Flags[MBX_OFFS_TYPE]        &= ~MBX_MASK_TYPE;
            pMbx->Data[0]                        = SWAPWORD(MBXSERVICE_MBXERRORCMD);
            pMbx->Data[1]                        = SWAPWORD(result);
            MBX_MailboxSendReq(pMbx, 0);
        }
    }
#if MAILBOX_QUEUE
    // the mailbox buffer has to be freed here
    else
    {
        APPL_FreeMailboxBuffer(pMbx);
        pMbx = NULL;
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief This function is called when the Master has read the Send-Mailbox.
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_MailboxReadInd(void)
{
    bSendMbxIsFull = FALSE;
    // HBu 02.05.06: the pointer psRepeatMbx is only free if there is no stored
    //               mailbox service from the last repeat
    if ( psRepeatMbx && psStoreMbx == NULL )
    {
    /* the last sent service is not stored for repeat any longer */
#if MAILBOX_QUEUE
        APPL_FreeMailboxBuffer(psRepeatMbx);
        psRepeatMbx = NULL;
#else
        psWriteMbx = psRepeatMbx;
#endif
    }

    /* the actual sent service has to be stored for repeat */
    psRepeatMbx = psReadMbx;

      if ( psStoreMbx )
      {
        /* there was a buffer stored */
        MBX_CopyToSendMailbox(psStoreMbx);
        /* no more buffer to be stored any more */
        psStoreMbx = NULL;
      }
      else
#if MAILBOX_QUEUE
    {
        TMBX MBXMEM *pMbx = GetOutOfMbxQueue(&sMbxSendQueue);
        if (pMbx)
        {
            MBX_CopyToSendMailbox(pMbx);
        }
    }
#endif // MAILBOX_QUEUE

      if ( u8MailboxSendReqStored )
    {
        /* there are mailbox services stored to be sent */
#if EMERGENCY_SUPPORTED
        if ( u8MailboxSendReqStored & EMCY_SERVICE )
        {
            /* call EMCY function that will send the stored Emergency service */
            EMCY_ContinueInd(psWriteMbx);
            if (EMCY_IsQueueEmpty())
            {
                u8MailboxSendReqStored &= ~EMCY_SERVICE;
            }
        }
        else
#endif
#if COE_SUPPORTED
        if ( u8MailboxSendReqStored & COE_SERVICE )
        {
           UINT8 result = 0;
            /* reset the flag indicating that CoE service to be sent was stored */
            u8MailboxSendReqStored &= ~COE_SERVICE;

            /* call CoE function that will send the stored CoE service */
            result = COE_ContinueInd(psWriteMbx);

            if (result != 0)
            {
                /*Set the pending CoE indication is an error occurred during the continue indication*/
                u8MailboxSendReqStored |= COE_SERVICE;
            }
        }
        else
#endif
#if SOE_SUPPORTED
        if ( u8MailboxSendReqStored & SOE_SERVICE )
        {
            /* reset the flag indicating that SoE service to be sent was stored */
            u8MailboxSendReqStored &= ~SOE_SERVICE;
            /* call CoE function that will send the stored SoE service */
            SOE_ContinueInd(psWriteMbx);
        }
        else
#endif
#if EOE_SUPPORTED
        if ( u8MailboxSendReqStored & EOE_SERVICE )
        {
            /* reset the flag indicating that EoE service to be sent was stored */
            u8MailboxSendReqStored &= ~EOE_SERVICE;
            /* call EoE function that will send the stored EoE service */
            EOE_ContinueInd(psWriteMbx);
        }
        else
#endif
#if FOE_SUPPORTED
        if ( u8MailboxSendReqStored & FOE_SERVICE )
        {
            /* reset the flag indicating that FoE service to be sent was stored */
            u8MailboxSendReqStored &= ~FOE_SERVICE;
            /* call FoE function that will send the stored FoE service */
            FOE_ContinueInd(psWriteMbx);
        }
        else
#endif
#if VOE_SUPPORTED
        if ( u8MailboxSendReqStored & VOE_SERVICE )
        {
            /* reset the flag indicating that VoE service to be sent was stored */
            u8MailboxSendReqStored &= ~VOE_SERVICE;
            /* call VoE function that will send the stored VoE service */
            VOE_ContinueInd(psWriteMbx);
        }
        else
#endif
        {
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief This function is called if the Master has requested a resending of the last
 \brief sent mailbox
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_MailboxRepeatReq(void)
{
    if (psRepeatMbx)
    {
        TMBX MBXMEM *pMbx = psRepeatMbx;
        /* send mailbox service stored for repeat */
        /* HBu 13.10.06: if a repeat request is received (again) before the previously repeated mailbox telegram
           was read from the master (psStoreMbx != NULL) the next mailbox telegram to be sent is still in the
            read mailbox so it has not to updated exchanged */
        ENTER_MBX_CRITICAL;

       if (bSendMbxIsFull && psStoreMbx == NULL)
        {
            /* mailbox is full, take the buffer off */
/*ECATCHANGE_START(V5.11) HW1*/
            DisableSyncManChannel(MAILBOX_READ);

            /* store the buffer to be sent next */
            psStoreMbx = psReadMbx;
            /* enable the mailbox again */
            EnableSyncManChannel(MAILBOX_READ);
/*ECATCHANGE_END(V5.11) HW1*/

            /* HBu 15.02.06: flag has to be reset otherwise the mailbox service
                             will not be copied by MBX_CopyToSendMailbox */
            bSendMbxIsFull = FALSE;
        }

#if BIG_ENDIAN_FORMAT
        /* mbx length field need to be swapped because it was already swapped in the previous "MBX_CopyToSendMailbox()" function call.
         * the field will be swapped again before copying to the SyncManager buffer*/
        pMbx->MbxHeader.Length = SWAPWORD(pMbx->MbxHeader.Length);
#endif

        MBX_CopyToSendMailbox(pMbx);
#if MAILBOX_QUEUE
        // HBu 17.06.06: psRepeatMbx has to be set to 0, when it was repeated, otherwise it would be returned twice
        // to the empty queue (MAILBOX_QUEUE=1) or a buffer get lost, if the the next repeat request will happen before
        // the repeated buffer was read
        psRepeatMbx = NULL;
#endif
        LEAVE_MBX_CRITICAL;
    }

    // Repeat was finished, toggle the acknowledge bit
    bMbxRepeatToggle = !bMbxRepeatToggle;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pMbx            Pointer to a Mailbox command to be sent (read by the Master)
 \param flags            Bit 0-6:    mailbox protocol type:
                                        0x01 - emergency service
                                        0x02 - CoE service
                                        0x04 - SoE service
                                        0x80 - EoE service
                                        0x10 - AoE service
                                        0x20 - VoE service
                             Bit 7:   0 - no more fragments to be sent for the requested mailbox service
                                       1 - additional fragments to be sent for the mailbox service, the
                                            corresponding XXXX_ContinueInd-function will be called to get
                                            the next fragment

 \return    0: Success - mailbox command could be stored in the send mailbox
            1: Failed - mailbox command could not be stored in the send mailbox, the
                            XXXX_ContinueInd service will be called when the mailbox was
                            read from the master to

 \brief        This function puts a new Mailbox service in the Send Mailbox
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 MBX_MailboxSendReq( TMBX MBXMEM * pMbx, UINT8 flags )
{
    UINT8 result = 0;

    /* HBu 06.02.06: in INIT-state a mailbox send request shall be refused */
    if ( (nAlStatus & STATE_MASK) == STATE_INIT )
        return ERROR_INVALIDSTATE;


    ENTER_MBX_CRITICAL;
    DISABLE_MBX_INT;

    /* the counter in the mailbox header has to be incremented with every new mailbox service to be sent
       if the mailbox data link layer is supported (software switch MAILBOX_REPEAT_SUPPORTED set)*/
    pMbx->MbxHeader.Flags[MBX_OFFS_COUNTER] &= ~MBX_MASK_COUNTER;
    /* HBu 13.02.06: Repeat-Counter was incremented too much if the mailbox service could not be sent */
    /* u8MbxCounter holds the actual counter for the mailbox header, only the values
       1-7 are allowed if the mailbox data link layer is supported  */
    if ( (u8MbxReadCounter & 0x07) == 0 )
        u8MbxReadCounter = 1;

    pMbx->MbxHeader.Flags[MBX_OFFS_COUNTER] |= u8MbxReadCounter << MBX_SHIFT_COUNTER;

    /* try to copy the mailbox command in the ESC */
    if ( MBX_CopyToSendMailbox(pMbx) != 0 )
    {
        /* no success, send mailbox was full, set flag  */
#if MAILBOX_QUEUE
        result = PutInMbxQueue(pMbx, &sMbxSendQueue);
        if (result != 0)
            flags |= FRAGMENTS_FOLLOW;
        else
            u8MbxReadCounter++;
#else
        flags |= FRAGMENTS_FOLLOW;
        result = 1;
#endif
    }
    /* HBu 13.02.06: Repeat-Counter was incremented too much if the mailbox service could not be sent */
    else
    {
        u8MbxReadCounter++;
    }

#if TEST_APPLICATION
/*ECATCHANGE_START(V5.11) TEST4*/
        if(IS_TEST_ACTIVE(Mbx_AlternatingMbxCntChange))
        {
            // get the low byte of the current timer as the new mbxcounter value
            UINT8 timerval = (UINT8)HW_GetTimer();

            timerval &= 0x3F;
            if(timerval == u8MbxReadCounter)
                u8MbxReadCounter ++;
            else
                u8MbxReadCounter = timerval;

            INC_TEST_CNT(Mbx_AlternatingMbxCntChange);
        }
/*ECATCHANGE_END(V5.11) TEST4*/
#endif
    if ( flags & FRAGMENTS_FOLLOW )
    {
        /* store the mailbox service that the corresponding XXX_ContinueInd function will
           be called when the send mailbox will have been read by the master because there
           are mailbox commands to be sent for this service */
        u8MailboxSendReqStored |= (flags & ((UINT8) ~FRAGMENTS_FOLLOW));
    }

    ENABLE_MBX_INT;
    LEAVE_MBX_CRITICAL;

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    This function is used to check if the received mailbox command can be processed.

        Also the contents of the Receive Mailbox will be copied in the variable sMbx.
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_CheckAndCopyMailbox( void )
{
    UINT16 mbxLen;

#if ESC_32BIT_ACCESS
    UINT32 tmpValue= 0;
    /* get the size of the received mailbox command and acknowledge the event*/
    HW_EscReadDWord(tmpValue,u16EscAddrReceiveMbx);

    /* the size has to be swapped here, all other bytes of the mailbox service will be swapped later */
    tmpValue = (SWAPDWORD(tmpValue) & 0x0000FFFF);
    mbxLen = (UINT16) tmpValue;
#else
    /* get the size of the received mailbox command and acknowledge the event*/
    HW_EscReadWord(mbxLen,u16EscAddrReceiveMbx);
    
    /* the size has to be swapped here, all other bytes of the mailbox service will be swapped later */
    mbxLen = SWAPWORD(mbxLen);
#endif

#if MAILBOX_QUEUE
/* ECATCHANGE_START(V5.11) MBX2*/
    if(bNoMbxMemoryAvailable == TRUE)
    {
        /* Return a no memory error in case of any mailbox request*/
        TMBX MBXMEM *pMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer(10); /* a mailbox error datagram length*/

        if(pMbx != NULL)
        {
            HMEMSET(pMbx,0x00,10);

            /* Mailbox error response: type 0 (mailbox service protocol) */
            pMbx->MbxHeader.Length     = 4;
            pMbx->MbxHeader.Flags[MBX_OFFS_TYPE]        &= ~(MBX_MASK_TYPE);
            pMbx->Data[0]                        = SWAPWORD(MBXSERVICE_MBXERRORCMD);
            pMbx->Data[1]                        = SWAPWORD(MBXERR_NOMOREMEMORY);
            MBX_MailboxSendReq(pMbx, 0);

            APPL_FreeMailboxBuffer(pMbx);
        }
    }
/* ECATCHANGE_END(V5.11) MBX2*/
#endif
    /* the length of the mailbox data is in the first two bytes of the mailbox,
       so the length of the mailbox header has to be added */
    mbxLen += MBX_HEADER_SIZE;

    /* in this example there are only two mailbox buffers available in the firmware (one for processing and
       one to stored the last sent response for a possible repeat request), so a
       received mailbox service can only be processed if a free buffer is available */
    if ( ( bSendMbxIsFull )                /* a received mailbox service will not be processed
                                                    as long as the send mailbox is still full
                                                    (waits to be read from the master) */
        ||( u8MailboxSendReqStored )    /* a mailbox service to be sent is still stored
                                                    so the received mailbox service will not be processed
                                                    until all stored mailbox services are sent */
#if !MAILBOX_QUEUE
/*ECATCHANGE_START(V5.11) MBX3*/
        || (bPendingMbxResponse == TRUE)
/*ECATCHANGE_END(V5.11) MBX3*/
#endif
        )
    {
        /* set flag that the processing of the mailbox service will be checked in the
            function MBX_Main (called from ECAT_Main) */
        bReceiveMbxIsLocked = TRUE;
    }
    else
    {
        /* received mailbox command can be processed, reset flag */
        bReceiveMbxIsLocked = FALSE;

        /* if the read mailbox size is too big for the buffer, set the copy size to the maximum buffer size, otherwise
           memory could be overwritten,
           the evaluation of the mailbox size will be done in the mailbox protocols called from MBX_WriteMailboxInd */

        if (mbxLen > u16ReceiveMbxSize)
            mbxLen = u16ReceiveMbxSize;

#if MAILBOX_QUEUE
        psWriteMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer(u16ReceiveMbxSize);

        /* if there is no more memory for mailbox buffer, the mailbox should not be read */
        if (psWriteMbx == NULL)
        {
            /* set flag that the processing of the mailbox service will be checked in the
                function MBX_Main (called from ECAT_Main) */
            bReceiveMbxIsLocked = TRUE;
            return;
        }
#endif
        /* copy the mailbox header and data*/
        HW_EscReadMbxMem((MEM_ADDR MBXMEM *) psWriteMbx,u16EscAddrReceiveMbx,mbxLen);

#if !MAILBOX_QUEUE
/*ECATCHANGE_START(V5.11) MBX3*/
        bPendingMbxResponse = TRUE;
/*ECATCHANGE_END(V5.11) MBX3*/
#endif

        {
        /*Read Control and Status of SyncManager 0 to check if the buffer is unlocked*/
#if ESC_32BIT_ACCESS
        VARVOLATILE UINT32 smstate = 0x00;
        HW_EscReadDWord(smstate,ESC_SYNCMAN_CONTROL_OFFSET);
/*ECATCHANGE_START(V5.11) ECAT7*/
        smstate = SWAPDWORD(smstate);
/*ECATCHANGE_END(V5.11) ECAT7*/
#elif ESC_16BIT_ACCESS
        VARVOLATILE UINT16 smstate = 0x00;
        HW_EscReadWord(smstate,ESC_SYNCMAN_CONTROL_OFFSET);
/*ECATCHANGE_START(V5.11) ECAT7*/
        smstate = SWAPWORD(smstate);
/*ECATCHANGE_END(V5.11) ECAT7*/
#else
        VARVOLATILE UINT8 smstate = 0x00;
        HW_EscReadByte(smstate,ESC_SYNCMAN_STATUS_OFFSET);
#endif

        if(smstate & SM_STATUS_MBX_BUFFER_FULL)
        {
            /*Unlock the mailbox SyncManger buffer*/
#if ESC_32BIT_ACCESS
            u32dummy = 0;
            HW_EscReadDWord(u32dummy,(u16EscAddrReceiveMbx + u16ReceiveMbxSize - 4));
#elif ESC_16BIT_ACCESS
            u16dummy = 0;
            HW_EscReadWord(u16dummy,(u16EscAddrReceiveMbx + u16ReceiveMbxSize - 2));
#else
            u8dummy = 0;
            HW_EscReadByte(u8dummy,(u16EscAddrReceiveMbx + u16ReceiveMbxSize - 1));
#endif

        }

        }

        /* in MBX_MailboxWriteInd the mailbox protocol will be processed */
        MBX_MailboxWriteInd( psWriteMbx );

    }
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx        Pointer to the Mailbox command to be send to the master.

 \brief    This function copies data to the Send Mailbox.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 MBX_CopyToSendMailbox( TMBX MBXMEM *pMbx )
{
    if ( (nAlStatus & STATE_MASK) == STATE_INIT)
    {
        /* the mailbox is disabled if the slave is in the INIT state */
        return( ERROR_INVALIDSTATE );
    }


    if ( !bMbxRunning )
    {
        /* the mailbox is disabled if the slave is in the INIT state */
        return( ERROR_INVALIDSTATE );
    }

    if ( bSendMbxIsFull )
    {
        /* mailbox service cannot be sent because the send mailbox is still full */
        return MBXERR_NOMOREMEMORY;
    }
    else
    {
        /* the variable mbxSize contains the size of the mailbox data to be sent */
        UINT16 mbxSize = pMbx->MbxHeader.Length;
#if BIG_ENDIAN_FORMAT
        /* the size has to be swapped here, all other bytes of the mailbox service were swapped before */
        pMbx->MbxHeader.Length = SWAPWORD(mbxSize);
#endif
        HW_EscWriteMbxMem((MEM_ADDR *)pMbx, u16EscAddrSendMbx, (mbxSize + MBX_HEADER_SIZE));
/* ECATCHANGE_HW(V5.10) HW1*/

        {
        /*Read Control and Status of SyncManager 1 to check if the buffer is still marked as empty*/
#if ESC_32BIT_ACCESS
        VARVOLATILE UINT32 smstate = 0x00;
        HW_EscReadDWord(smstate,(ESC_SYNCMAN_CONTROL_OFFSET + SIZEOF_SM_REGISTER));
/*ECATCHANGE_START(V5.11) ECAT7*/
        smstate = SWAPDWORD(smstate);
/*ECATCHANGE_END(V5.11) ECAT7*/
#elif ESC_16BIT_ACCESS
        VARVOLATILE UINT16 smstate = 0x00;
        HW_EscReadWord(smstate,(ESC_SYNCMAN_CONTROL_OFFSET + SIZEOF_SM_REGISTER));
/*ECATCHANGE_START(V5.11) ECAT7*/
        smstate = SWAPWORD(smstate);
/*ECATCHANGE_END(V5.11) ECAT7*/
#else
        VARVOLATILE UINT8 smstate = 0x00;
        HW_EscReadByte(smstate,(ESC_SYNCMAN_STATUS_OFFSET + SIZEOF_SM_REGISTER));
#endif

        if(!(smstate & SM_STATUS_MBX_BUFFER_FULL))
        {
#if ESC_32BIT_ACCESS || ESC_16BIT_ACCESS
            UINT8 BytesLeft = u16SendMbxSize - (mbxSize + MBX_HEADER_SIZE);
#endif

            /*Write last Byte to trigger mailbox full flag*/
#if ESC_32BIT_ACCESS
            /*Read last 4 Bytes and write them again (required if low 3 Bytes of the DWORD were written before)*/
            u32dummy = 0;
            if(BytesLeft < 4)
            {
                /*The last 4Bytes are overlapping the already written buffer*/

                /*Get the valid 32Bit address*/
                UINT32 LastDataAddress = ((mbxSize + MBX_HEADER_SIZE)/4)*4;
            
                /*Copy the buffer to overwrite*/
                MEMCPY((UINT32 *)&u32dummy,(((UINT8 *)pMbx) + LastDataAddress),(4 - BytesLeft));
            }
            
            HW_EscWriteDWord(u32dummy,(u16EscAddrSendMbx + u16SendMbxSize - 4));
#elif ESC_16BIT_ACCESS
            /*Read last 2 Bytes and write them again (required if low Byte of the WORD were written before)*/
            u16dummy = 0;
            if(BytesLeft < 2)
            {
                /*The last 2Bytes are overlapping the already written buffer*/
                                
                /*Get the valid 16Bit address*/
                UINT32 LastDataAddress = ((mbxSize + MBX_HEADER_SIZE)/2)*2;
            
                /*Copy the buffer to overwrite*/
                MEMCPY((UINT16 *)&u16dummy,(((UINT8 *)pMbx) + LastDataAddress),(2 - BytesLeft));
            }
            
            HW_EscWriteWord(u16dummy,(u16EscAddrSendMbx + u16SendMbxSize - 2));
#else
            u8dummy = 0;
            HW_EscWriteByte(u8dummy,(u16EscAddrSendMbx + u16SendMbxSize - 1));
#endif
        }
        }

#if !MAILBOX_QUEUE
    // return buffer only if it is not zero
    if ( pMbx != psWriteMbx && psWriteMbx != 0 )
    {
        /* MBX_MailboxSendReq was called with a buffer unequal asMbx[0] or asMbx[1],
           so the mailbox response has to copied for repeat */
        MBXMEMCPY(psWriteMbx, pMbx, mbxSize + MBX_HEADER_SIZE);
        pMbx = psWriteMbx;
    }
#endif
        /* store last send mailbox service for a possible repeat
            one buffer includes the last send service (psRepeatMbx),
            the other one the actual service to be sent (psReadMbx),
            there is no buffer available for a mailbox receive service
            until the last sent buffer was read from the master
            the exception is after the INIT2PREOP transition, in that
            case there is no last sent service (psReadMbx = 0) */
        if ( psReadMbx )
            psWriteMbx = NULL;
        else
        {
            /* only the first time after the INIT2PREOP-transition */
            psWriteMbx = &asMbx[1];
        }
        // HBu 17.06.06: psRepeatMbx was already updated in MBX_MailboxReadInd
        // psRepeatMbx = psReadMbx;
        psReadMbx = pMbx;

        /* set flag that send mailbox is full now */
        bSendMbxIsFull = TRUE;

#if !MAILBOX_QUEUE
/*ECATCHANGE_START(V5.11) MBX3*/
        /*Reset the pending mailbox response indication (a new request can be handled)*/
        bPendingMbxResponse = FALSE;
/*ECATCHANGE_END(V5.11) MBX3*/
#endif

        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    This function is called cyclically to check if a received Mailbox service was
             stored.
*////////////////////////////////////////////////////////////////////////////////////////

void MBX_Main(void)
{
#if MAILBOX_QUEUE
    TMBX MBXMEM *pMbx = NULL;

    do
    {
        UINT8 result = 0;

        pMbx = GetOutOfMbxQueue(&sMbxReceiveQueue);
        if ( pMbx )
            result = MailboxServiceInd(pMbx);

        if ( result != 0 )
        {
            /* Mailbox error response: type 0 (mailbox service protocol) */
            pMbx->MbxHeader.Length     = 4;
            pMbx->MbxHeader.Flags[MBX_OFFS_TYPE]        &= ~(MBX_MASK_TYPE);
            pMbx->Data[0]                        = SWAPWORD(MBXSERVICE_MBXERRORCMD);
            pMbx->Data[1]                        = SWAPWORD(result);
            MBX_MailboxSendReq(pMbx, 0);
        }
    }
    while ( pMbx != NULL );

#endif // MAILBOX_QUEUE

#if EMERGENCY_SUPPORTED
    //dump queued emcy message if send mailbox buffer is empty
    if(bMbxRunning && !bSendMbxIsFull && !EMCY_IsQueueEmpty())
    {
        TEMCYMESSAGE EMCYMEM *pEmcy = GetOutOfSendEmcyQueue();
        EMCY_SendEmergency(pEmcy);
    }
#endif

      if ( bReceiveMbxIsLocked )
      {
          /* the work on the receive mailbox is locked, check if it can be unlocked (if all
             mailbox commands has been sent */
          MBX_CheckAndCopyMailbox();
      }
}

/** @} */

#endif //#if MAILBOX_SUPPORTED

