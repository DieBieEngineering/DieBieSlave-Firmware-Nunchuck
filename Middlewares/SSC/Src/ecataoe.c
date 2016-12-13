/**
\addtogroup AoE ADS over EtherCAT
@{
*/

/**
\file ecataoe.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the AoE mailbox interface.

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version V5.01:<br>
V5.10 AOE1: Reset AoE response result<br>
V5.10 ESC1: Update address register offset for 32Bit ESC access<br>
V5.10 ESC5: Add missing swapping<br>
<br>Changes to version V5.0:<br>
V5.01 ESC2: Add missed value swapping<br>
<br>Changes to version V4.40:<br>
V5.0 ESC1: ESC 32Bit Access added.<br>
<br>Changes to version V4.20:<br>
V4.40 HW0: Generic hardware access functions<br>
<br>Changes to version V4.11:<br>
V4.20 MBX 1:    Free Mailbox queue buffer<br>
V4.11 AOE 1: Additional return "No memory available"<br>
<br>Changes to version V4.08:<br>
V4.10 AOE 1: The mailbox header of the AoE fragments was not initialized<br>
V4.10 AOE 2: The function AOEAPPL_AmsInd was not called
*/

/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if AOE_SUPPORTED

#include "esc.h"
#include "ecatslv.h"

#define    _ECATAOE_    1
#include "aoeappl.h"
#undef      _ECATAOE_
/* ECATCHANGE_START(V5.11) ECAT10*/
/*remove definition of _ECATAOE_ (#ifdef is used in aoeappl.h)*/
/* ECATCHANGE_END(V5.11) ECAT10*/

/*---------------------------------------------------------------------------------------
------
------    Internal Types and Defines
------
---------------------------------------------------------------------------------------*/
#define    MAX_ADS_QUEUE_SIZE       10 /**< \brief ADS queue*/
#define    MAX_NO_AMS_CMD_IN_WORK   10 /**< \brief Max parallel AMS commands*/
#define    AMS_FRAGMENT_TIMEOUT     10000 /**< \brief AMS fragment timeout*/


/**
 * \brief ADS queue 
 */
typedef struct
{
    UINT16    firstInQueue; /**< \brief First in queue*/
    UINT16    lastInQueue; /**< \brief last in queue*/
    UINT16    maxQueueSize; /**< \brief Queue size*/
    AmsCmd MBXMEM * queue[MAX_ADS_QUEUE_SIZE+1]; /**< \brief Queue buffer*/
} TADSQUEUE;


/**
 * \brief AMS fragment table
 */
typedef struct
{
    UINT16                packetNo; /**< \brief Packet number*/
    UINT16                fragmentNo; /**< \brief Fragment number*/
    AmsAddr                sender; /**< \brief Sender AMS address*/
    UINT32                actOffs; /**< \brief Actual offset*/
    UINT16                ttl; /**< \brief Ttl*/
    AmsCmd MBXMEM *    pFullCmd; /**< \brief AMS command pointer*/
} TAmsFragTable, *PTAmsFragTable;


/*---------------------------------------------------------------------------------------
------
------    Local Variables
------
---------------------------------------------------------------------------------------*/
TADSQUEUE MBXMEM        sAdsIndQueue; /**< \brief ADS queue variable*/
UINT16                    u16AmsFragAck; /**< \brief AMS fragment acknowledge*/
UINT16                    u16AmsPacketNo; /**< \brief AMS Packet number*/
UINT16                    u16AmsCmdsInWork; /**< \brief AMS command in work*/
TAmsFragTable HUGE    asAmsCmdsInWork[MAX_NO_AMS_CMD_IN_WORK]; /**< \brief Pending AMS commands*/


/*---------------------------------------------------------------------------------------
------
------    Local Functions
------
---------------------------------------------------------------------------------------*/
UINT16 AmsFragRes(AmsCmd MBXMEM *pCmd, UINT16 dataLen);
UINT16 InsertInFragTable(AmsCmd MBXMEM *pCmd);
AmsCmd MBXMEM * RemoveFromFragTable(UINT16 index);
AmsCmd MBXMEM * AoeGetOutOfQueue(void);
UINT16 AoePutInQueue(AmsCmd MBXMEM *pCmd);


/*---------------------------------------------------------------------------------------
------
------    functions
------
---------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function intialize the AoE Interface.
*////////////////////////////////////////////////////////////////////////////////////////

void AOE_Init(void)
{
    bAoeWaitForNetId = 1;
    bAoeRelativeNetIds = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pCmd        Pointer to the AMS commands
 \param    dataLen    length of the AMS command to be sent

 \brief    This function is called when an AMS response shall be sent
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 AmsFragRes(AmsCmd MBXMEM *pCmd, UINT16 dataLen)
{
    UINT16 result = ERR_NOERROR;
    UINT16 maxDataLen = u16AmsFragSize;

    if (maxDataLen < dataLen)
    {
        /* command has to fragmented because it doesn't fit in the mailbox */
        UINT8        bLoop = 1;
        UINT16    offset = 2 * SIZEOF(AmsAddr);
        UINT16    fragNo = 0;

        dataLen += (SIZEOF(AmsHead) - 2 * SIZEOF(AmsAddr));
        while (bLoop)
        {
            TMBX MBXMEM *pMbx;
            AmsCmd MBXMEM *pFragCmd;
            UINT16 size;

            if (dataLen > maxDataLen)
                size = maxDataLen;
            else
            {
                size = dataLen;
                bLoop = 0;
            }

            pMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer(size + SIZEOF(AmsHead) + MBX_HEADER_SIZE);
            if (pMbx)
            {
                /* the AMS command begins after the mailbox header */
                pFragCmd = (AmsCmd MBXMEM *) pMbx->Data;
                HMEMCPY((UINT8 MBXMEM *) pFragCmd, (UINT8 MBXMEM *) pCmd, 2*SIZEOF(AmsAddr));
                pFragCmd->head.cmdId = ROUTERCMD_FRAGMENT;
                pFragCmd->head.stateFlags = u16AmsFragAck;
                pFragCmd->head.cbData = size;
                pFragCmd->head.errCode = ERR_NOERROR;
                pFragCmd->head.hUser.frag.packetNo = u16AmsPacketNo;
                pFragCmd->head.hUser.frag.fragmentNo = fragNo;
                fragNo++;
                HMEMCPY(&pFragCmd[1], &((UINT8 MBXMEM *) pCmd)[offset], size);
                offset += size;
                dataLen -= size;

                if (MBX_MailboxSendReq(pMbx, AOE_SERVICE) != 0)
                {
                    bLoop = FALSE;
                    result = ADSERR_DEVICE_NOMEMORY;
#if MAILBOX_QUEUE
                    if (pMbx != NULL)
                        APPL_FreeMailboxBuffer(pMbx);
#endif
                }
            }
            else
            {
                bLoop = FALSE;
                result = ADSERR_DEVICE_NOMEMORY;
            }
        }

        u16AmsPacketNo++;

        FREEADSMEM(pCmd);
    }
    else
    {
        TMBX MBXMEM *pMbx;
        AmsCmd MBXMEM *pFragCmd;

        pMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer(dataLen + SIZEOF(AmsHead) + MBX_HEADER_SIZE);
        if (pMbx)
        {
            /* the AMS command begins after the mailbox header */
            pFragCmd = (AmsCmd MBXMEM *) pMbx->Data;
            HMEMCPY(pFragCmd, pCmd, dataLen + SIZEOF(AmsHead));
            FREEADSMEM(pCmd);
            pMbx->MbxHeader.Length    = dataLen + SIZEOF(AmsHead);
            pMbx->MbxHeader.Address    = 0;
            pMbx->MbxHeader.Flags[MBX_OFFS_TYPE] = MBX_TYPE_AOE<<MBX_SHIFT_TYPE;
            if (MBX_MailboxSendReq(pMbx, AOE_SERVICE) != 0)
            {
                result = ADSERR_DEVICE_NOMEMORY;
                APPL_FreeMailboxBuffer(pMbx);
            }
        }
        else
            result = ADSERR_DEVICE_NOMEMORY;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pCmd        Pointer to the AMS commands

 \brief    This function is called when a fragmented AMS command is received
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 InsertInFragTable(AmsCmd MBXMEM *pCmd)
{
    UINT16 result = ERR_NOERROR;

    if (pCmd->head.hUser.frag.fragmentNo == 0)
    {
        if (pCmd->head.cbData >= SIZEOF(AmsFirstFragmentHead))
        {
            AmsFirstFragmentHead MBXMEM *pHead = (AmsFirstFragmentHead MBXMEM *) pCmd;

            if (u16AmsCmdsInWork < MAX_NO_AMS_CMD_IN_WORK)
            {
                TAmsFragTable MBXMEM *pEntry = &asAmsCmdsInWork[u16AmsCmdsInWork];
                AmsCmd MBXMEM *pFullCmd = (AmsCmd MBXMEM *) ALLOCADSMEM(pHead->org.cbData + SIZEOF(AmsHead));

                if (pFullCmd)
                {
                    HMEMCPY((UINT8 MBXMEM *) pFullCmd, (UINT8 MBXMEM *) pCmd, 2*SIZEOF(AmsAddr));
                    HMEMCPY((UINT8 MBXMEM *) &pFullCmd->head.cmdId, (UINT8 MBXMEM *) &pHead->org, pCmd->head.cbData);
                    HMEMCPY((UINT8 MBXMEM *) &pEntry->sender, (UINT8 MBXMEM *) &pCmd->head.sender, SIZEOF(AmsAddr));

                    pEntry->packetNo         = pCmd->head.hUser.frag.packetNo;
                    pEntry->fragmentNo     = 1;
                    pEntry->actOffs         = (pCmd->head.cbData + 2*SIZEOF(AmsAddr));
                    pEntry->pFullCmd         = pFullCmd;
                    pEntry->ttl                = AMS_FRAGMENT_TIMEOUT;
                    u16AmsCmdsInWork++;
                }
                else
                {
                    result = ADSERR_DEVICE_NOMEMORY;
                }
            }
        }
        else
        {
            result = ADSERR_DEVICE_ERROR;
        }
    }
    else
    {
        result = ADSERR_DEVICE_ERROR;
    }

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index        Index of the requested AMS command

 \brief    This function is called when a fragmented AMS command is searched
*////////////////////////////////////////////////////////////////////////////////////////

AmsCmd MBXMEM * RemoveFromFragTable(UINT16 index)
{
    UINT16 i;
    AmsCmd MBXMEM * pCmd = asAmsCmdsInWork[index].pFullCmd;

    if (index < u16AmsCmdsInWork)
    {
        for (i = index; i < (u16AmsCmdsInWork - 1); i++)
        {

            HMEMCPY((UINT8 MBXMEM *) &asAmsCmdsInWork[i], (UINT8 MBXMEM *) &asAmsCmdsInWork[i + 1], SIZEOF(TAmsFragTable));
        }

        u16AmsCmdsInWork--;
    }

    return pCmd;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pCmd        Pointer to the AMS commands

 \brief    This function is called when a fragmented AMS command is received
*////////////////////////////////////////////////////////////////////////////////////////

AmsCmd MBXMEM * AOE_FragmentedCmdInd(AmsCmd MBXMEM *pCmd)
{
    UINT16 i;
    UINT16 result = ERR_NOERROR;
    UINT8 bFound = 0;
    UINT8 bAck;
    AmsCmd MBXMEM * pAck = 0;

    if (pCmd->head.stateFlags & AMSCMDSF_RESPONSE)
    {
        FREEMEM(pCmd);
        return NULL;
    }

    if ( (pCmd->head.stateFlags & AMSCMDSF_NORETURN) == 0 )
        bAck = 1;
    else
        bAck = 0;

    for (i = 0; (i < u16AmsCmdsInWork)&&(!bFound); i++)
    {
        TAmsFragTable MBXMEM *pEntry = &asAmsCmdsInWork[i];
        UINT8 NetIdEqual = AOE_AmsNetIdsEqual(&pCmd->head.sender.netId, &pEntry->sender.netId);
        if ((pCmd->head.hUser.frag.packetNo == pEntry->packetNo)
            && (NetIdEqual == 1)
          &&(pCmd->head.sender.port == pEntry->sender.port)
            )
        {
            /* AMS command found */
            if (pCmd->head.hUser.frag.fragmentNo == pEntry->fragmentNo)
            {
                AmsCmd MBXMEM * pFullCmd = pEntry->pFullCmd;
                if ( (pEntry->actOffs + pCmd->head.cbData) <= (pFullCmd->head.cbData + SIZEOF(AmsHead)) )
                {
                    bFound = 1;
                    HMEMCPY(&((UINT8 MBXMEM *)pFullCmd)[pEntry->actOffs], &pCmd[1], pCmd->head.cbData);
                    pEntry->actOffs += pCmd->head.cbData;
                    pEntry->fragmentNo++;
                    pEntry->ttl    = AMS_FRAGMENT_TIMEOUT;
                    if ( pEntry->actOffs == (pFullCmd->head.cbData + SIZEOF(AmsHead)) )
                    {
                        /* last fragment */
                        FREEADSMEM(pCmd);
                        pCmd = RemoveFromFragTable(i);
                    }
                    else
                    {
                        /* middle Fragment */
                        if (bAck)
                            pAck = pCmd;
                        else
                            FREEMEM(pCmd);
                        pCmd = NULL;
                    }
                }
                else
                {
                    result = ADSERR_DEVICE_ERROR;
                    bFound = 1;
                }
            }
            else
            {
                result = ADSERR_DEVICE_ERROR;
                bFound = 1;
            }
        }
    }

    if (bFound)
    {
        if (result != ERR_NOERROR)
        {
            /* fault in Fragment, delete from list */
            AmsCmd MBXMEM *p = RemoveFromFragTable(i);

            if (p)
                FREEADSMEM(p);

            FREEADSMEM(pCmd);
            return NULL;
        }
    }
    else
    {
        /* new AMS command */
        result = InsertInFragTable(pCmd);
        if (result == ERR_NOERROR)
        {
            if (bAck)
                pAck = pCmd;
            else
                FREEADSMEM(pCmd);
            pCmd = NULL;
        }
        else
        {
            FREEADSMEM(pCmd);
            return NULL;
        }
    }

    if (bAck)
    {
        /* fragment has to be acknowledged */
        AOE_AmsRes(pAck, ERR_NOERROR, 0);
    }

    return(pCmd);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pNetId1,pNetId2            Net-IDs to be compared

 \return 0 = Net-IDs different, 1 = Net-IDs equal

 \brief    This function is called when an AMS response shall be sent
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 AOE_AmsNetIdsEqual(AmsNetId MBXMEM *pNetId1, AmsNetId MBXMEM *pNetId2)
{
    int i = 0;

    for (i = 0; i < 6; i++)
    {
        if (pNetId1->b[i] != pNetId2->b[i])
            return 0;
    }

    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pCmd            Pointer to the AMS commands
 \param    amsErrCode    AMS Error Code
 \param    dataLen        AMS Data Length

 \brief    This function is called when an AMS response shall be sent
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 AOE_AmsRes(AmsCmd MBXMEM *pCmd, UINT16 amsErrCode, UINT16 dataLen)
{
    AmsAddr amsAddr;

    /* change Sender and Target */
    HMEMCPY((UINT8 MBXMEM *) &amsAddr, (UINT8 MBXMEM *) &pCmd->head.sender, SIZEOF(AmsAddr));
    HMEMCPY((UINT8 MBXMEM *) &pCmd->head.sender, (UINT8 MBXMEM *) &pCmd->head.target, SIZEOF(AmsAddr));
    HMEMCPY((UINT8 MBXMEM *) &pCmd->head.target, (UINT8 MBXMEM *) &amsAddr, SIZEOF(AmsAddr));

    pCmd->head.errCode         = amsErrCode;
    pCmd->head.cbData         = dataLen;
    pCmd->head.stateFlags    |= AMSCMDSF_RESPONSE;

    return AmsFragRes(pCmd, dataLen);
}

#if MAILBOX_QUEUE
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pCmd      Pointer to the AMS commands

 \brief    This function is called when a AMS indication is received
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 AoePutInQueue(AmsCmd MBXMEM *pCmd)
{
    UINT16 lastInQueue;

    ENTER_MBX_CRITICAL;
    lastInQueue = sAdsIndQueue.lastInQueue+1;
    if (lastInQueue == sAdsIndQueue.maxQueueSize)
    {
        lastInQueue = 0;
    }

    if (sAdsIndQueue.firstInQueue == lastInQueue)
    {
        /* Queue overflow */
        LEAVE_MBX_CRITICAL;
        return ADSERR_DEVICE_NOMEMORY;
    }

    sAdsIndQueue.queue[sAdsIndQueue.lastInQueue] = pCmd;
    sAdsIndQueue.lastInQueue = lastInQueue;

    LEAVE_MBX_CRITICAL;
    return ERR_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     pCmd      Pointer to the AMS commands

 \brief    This function is gets the oldest ADS indication from the queue
*////////////////////////////////////////////////////////////////////////////////////////

AmsCmd MBXMEM * AoeGetOutOfQueue(void)
{
    AmsCmd MBXMEM *pCmd = NULL;

    ENTER_MBX_CRITICAL;
    if (sAdsIndQueue.firstInQueue != sAdsIndQueue.lastInQueue)
    {
        UINT16 firstInQueue = sAdsIndQueue.firstInQueue;
        pCmd = sAdsIndQueue.queue[firstInQueue];
        firstInQueue++;
        sAdsIndQueue.firstInQueue = firstInQueue;
        if (sAdsIndQueue.firstInQueue == sAdsIndQueue.maxQueueSize)
        {
            sAdsIndQueue.firstInQueue = 0;
        }
    }
    else
        pCmd = 0;
    LEAVE_MBX_CRITICAL;

    return pCmd;
}

#endif
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx      Pointer to the received mailbox data from the master.

 \return    result of the operation (0 (success) or mailbox error code (MBXERR_.... defined in
            mailbox.h))

 \brief    This function is called when a AoE (ADS over EtherCAT) service is received from
             the master.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 AOE_ServiceInd(TMBX MBXMEM *pMbx)
{
    UINT16 mbxSize = SWAPWORD(pMbx->MbxHeader.Length);

    if (mbxSize)
    {
        AmsCmd MBXMEM * pCmd = (AmsCmd MBXMEM *) ALLOCADSMEM(mbxSize);

        TAdsReadRes MBXMEM *pRes = (TAdsReadRes MBXMEM *)pCmd;

        if (pCmd && mbxSize >= (SIZEOF(AmsHead)))
        {
            HMEMCPY((UINT8 MBXMEM *) pCmd, pMbx->Data, (mbxSize+1) & 0xFFFE);
#if MAILBOX_QUEUE
            APPL_FreeMailboxBuffer(pMbx);
#endif
            if ( pCmd->head.cmdId == ADSSRVID_WRITE && pCmd->head.cbData >= (SIZEOF(TAdsWriteReq)-SIZEOF(AmsHead)+SIZEOF(AmsNetId)) )
            {
                TAdsWriteReq MBXMEM * pReq = (TAdsWriteReq MBXMEM *) pCmd;
                UINT16 ecatAddr;
#if ESC_32BIT_ACCESS
                UINT32 TmpVal = 0;
                HW_EscReadDWord(TmpVal, ESC_SLAVE_ADDRESS_OFFSET);
                ecatAddr = (UINT16) ((SWAPDWORD(TmpVal) & ESC_SLAVE_ADDRESS_MASK));
#else
                HW_EscReadWord(ecatAddr, ESC_SLAVE_ADDRESS_OFFSET);
                ecatAddr = SWAPWORD(ecatAddr);
#endif
                if ( pReq->indexGroup == 1
                  && pReq->indexOffset == 3
                  && pReq->cbLength == SIZEOF(AmsNetId)
                  && pCmd->head.target.port == ecatAddr
                  && pCmd->head.sender.port == ecatAddr
                   )
                {
                    // HBu 30.09.06: mit dem richtigen Kommando wird die Net-ID immer ueberschrieben
                    bAoeWaitForNetId = 0;
                    HMEMCPY(&sRouterNetId, &pReq[1], SIZEOF(AmsNetId));
                    if (((UINT32 MBXMEM *) &pReq[1])[0] == 0)
                    {
                        // relative Net-ID
                        HMEMCPY(&sRouterNetId, pCmd->head.target.netId.b, 4);
                        bAoeRelativeNetIds = TRUE;
                    }
                    else
                    {
                        bAoeRelativeNetIds = FALSE;
                    }

                    pCmd->head.stateFlags |= AMSCMDSF_RESPONSE;
                    pRes->result = 0;

                    AOE_AmsRes( pCmd, 0, 4);
                    return 0;
                }
            }

            if (bAoeWaitForNetId)
            {
                // Hbu 30.09.06: aus Kompatibilitaetsgruenden wird die Net-ID aus dem ersten Kommando genommen
                HMEMCPY(&sRouterNetId, &pCmd->head.target.netId.b[0], SIZEOF(AmsNetId));        // take the first rdx NET ID
                bAoeWaitForNetId = 0;
            }

            AOEAPPL_AmsInd(pCmd);
        }
        else
        {
            return MBXERR_NOMOREMEMORY;
        }
    }

    return 0;
}

/** @} */

#endif /* AOE_SUPPORTED */


