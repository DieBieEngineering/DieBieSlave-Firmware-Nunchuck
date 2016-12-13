/**
\addtogroup EoE Ethernet over EtherCAT
@{
*/

/**
\file ecateoe.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the EoE mailbox interface

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 EOE1: update mailbox length calculation on EoE response<br>
V5.11 EOE2: fix deadlock in case that the last EoE fragment of a frame and not be copied to the mailbox buffer directly<br>
V5.11 MBX3: set application triggered emergency and EoE data to pending if no mailbox queue is supported and another mailbox request is currently handled, Handle only one mailbox request at a time (in case that MAILBPX_QUEUE is disabled)<br>
<br>Changes to version V5.0:<br>
V5.10 EOE1: Prevent memeory leaks on incomplete EoE sequences<br>
V5.10 EOE3: Change local send frame pending indication variable to a global variable (it need to be resetted if the mailbox is stopped and a frame is pending)<br>
V5.10 EOE4: Set frame buffer size to 1536 byte<br>
V5.10 EOE5: Support "Get IP Parameter" (4Byte EoE requests also valid<br>
            Change setting of EoE response flag)<br>
V5.10 ESC5: Add missing swapping<br>
<br>Changes to version V4.40:<br>
V5.0 EOE1: Support static Ethernet buffer.<br>
V5.0 EOE2: Support segmented EoE.<br>
V5.0 EOE3: Prevent sending EoE datagram if EoE buffer is full.<br>
<br>Changes to version V4.20:<br>
V4.40 EoE1: Prevent free empty frame buffer<br>
<br>Changes to version V4.11:<br>
V4.20 PIC24: Add EL9800_4 (PIC24) required source code<br>
V4.11 EOE 1: Additional memory allocation check<br>
<br>Changes to version V4.06:<br>
V4.07 ECAT 1: the sources for SPI and MCI were merged (in ecat_def.h),<br>
                   set the switch MCI_HW to 1 when using the MCI,<br>
                   set the switch SPI_HW to 1 when using the SPI<br>
V4.07 ECATEOE 1: For the PIC18 the size of the Ethernet frame has to be checked<br>
                       before copying it the receive buffer
*/

/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if EOE_SUPPORTED

#include "ecatslv.h"
#define    _ECATEOE_    1
#include "eoeappl.h"
#undef      _ECATEOE_
#define    _ECATEOE_    0



/*---------------------------------------------------------------------------------------
------
------    internal Types and Defines
------
---------------------------------------------------------------------------------------*/

#define     ECATEOE       0x4300
#define     ECATEOEMAX    0x02

/*---------------------------------------------------------------------------------------
------
------    static variables
------
---------------------------------------------------------------------------------------*/

UINT8               u8ReceiveFragmentNo;                /**< \brief stores the next expected fragment number
                                                            to be received, if the last frame was
                                                            received completely, u8ReceiveFragmentNo is 0 */
UINT16              u16EthernetReceiveSize;             /**< \brief stores the size of the actual received
                                                            Ethernet frame */
UINT16              u16EthernetReceiveOffset;           /**< \brief stores the actual offset where the received
                                                            fragment shall be copied to */
UINT16              u16ReceiveFrameNo;                  /**< \brief store the frameNo, which has to be the same
                                                            for all fragments of the whole Ethernet frame */
MEM_ADDR MBXMEM *   pEthernetReceiveFrame;              /**< \brief stores the buffer for actual received
                                                            Ethernet frame */
UINT8               u8SendFragmentNo;                   /**< \brief stores the fragment number of the next fragment to be sent */
UINT16              u16EthernetSendSize;                /**< \brief stores the size of the actual Ethernet frame to be sent */
UINT16              u16EthernetSendOffset;              /**< \brief stores the actual offset of the Ethernet frame, which
                                                            points to the next fragment to be sent */
UINT16              u16SendFrameNo;                     /**< \brief stores the frame number of the actual Ethernet frame to be sent */
MEM_ADDR MBXMEM *   pEthernetSendFrame;                 /**< \brief stores the buffer of the Ethernet frame to be sent */
TMBX MBXMEM *       pEoeSendStored;                     /**< \brief if the mailbox service could not be sent (or stored),
                                                            the EoE service will be stored in this variable
                                                            and will be sent automatically from the mailbox handler
                                                            (EOE_ContinueInd) when the send mailbox will be read
                                                            the next time from the master */
#if STATIC_ETHERNET_BUFFER
UINT8           aEthernetReceiveBuffer[ETHERNET_MAX_FRAMEBUF_LEN];     /**< \brief the PIC18 has not enough memory for dynamic memory
                                                                        management, so this buffer is used to send and receive
                                                                        Ethernet frames */
#endif


/*---------------------------------------------------------------------------------------
------
------    static functions
------
---------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
------
------    functions
------
---------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function intialize the EoE Interface.
*////////////////////////////////////////////////////////////////////////////////////////

void EOE_Init(void)
{
    pEoeSendStored = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx      Pointer to the received mailbox data from the master.

 \return    result of the operation (0 (success) or mailbox error code (MBXERR_.... defined in
            mailbox.h))

 \brief    This function is called when a EoE (Ethernet over EtherCAT) service is received from
             the master.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 EOE_ServiceInd(TMBX MBXMEM *pMbx)
{
    UINT16 result = 0;
    ETHERCAT_EOE_HEADER MBXMEM * pEoe = (ETHERCAT_EOE_HEADER MBXMEM *) pMbx->Data;
    ETHERCAT_EOE_INIT MBXMEM * pEoeInit;
    UINT16 mbxSize = SWAPWORD(pMbx->MbxHeader.Length);
    UINT16 expSize = ETHERCAT_EOE_HEADER_LEN;

    /* check, if mailbox header is correct for EoE */
    if ( mbxSize >= ETHERCAT_EOE_HEADER_LEN)
    {
        /* enough bytes were received, check EoE service */
        switch ( SWAPWORD(pEoe->Flags1) & EOEHEADER_TYPE )
        {
        case EOE_TYPE_FRAME_FRAG:
            /* EoE fragment received, check fragment number, the fragments are expected one after another
               and u8ReceiveFragmentNo stores the next expected fragmentNo to be received */
            if ( (SWAPWORD(pEoe->Flags2) & EOEHEADER_FRAGMENT) != u8ReceiveFragmentNo )
            {
                /* wrong fragment received, that means fragments got lost, we have to check if fragments
                   of an old Ethernet frame are still stored in the buffer pEthernetReceiveFrame */
                if ( u8ReceiveFragmentNo != 0 )
                {
                    /* expected fragmentNo was unequal 0, that means fragments of an old Ethernet frame were stored,
                       so we have to free the old buffer (because the old Ethernet frame was not received completely) */
#if !STATIC_ETHERNET_BUFFER
                    /* for the PIC18 (Eva-Board demo) there is only one buffer for an Ethernet frame available,
                       for all other microcontroller we have to free the buffer here to dynamic memory handling */
                    if (pEthernetReceiveFrame != NULL)
                    {
                        FREEMEM((MEM_ADDR MBXMEM *) pEthernetReceiveFrame);
                        pEthernetReceiveFrame = NULL;
                    }
#endif
                    /* next fragment which will be accepted is a the beginning of a new EoE frame (fragmentNo=0) */
                    u8ReceiveFragmentNo = 0;
                }

                /* ignore fragment if it is not the beginning of a new EoE frame */
                if ( SWAPWORD(pEoe->Flags2) & EOEHEADER_FRAGMENT )
                    /* fragmentNo != 0 -> ignore */
                    return 0;
            }

            if ( u8ReceiveFragmentNo == 0 )
            {
                /* fragmentNo = 0, the fragment is the beginning of a new frame, get buffer for the whole Ethernet frame */
                u16EthernetReceiveSize = ((SWAPWORD(pEoe->Flags2) & EOEHEADER_OFFSETBUFFER) >> EOEHEADERSHIFT_OFFSETBUFFER) << 5;    // * 32
                /* for the PIC18 (Eva-Board demo) there is only one buffer for an Ethernet frame available,
                   for all other microcontroller we have to get a buffer from the dynamic memory handling */

#if STATIC_ETHERNET_BUFFER
                pEthernetReceiveFrame = (MEM_ADDR *)aEthernetReceiveBuffer;
                if ( u16EthernetReceiveSize > ETHERNET_MAX_FRAME_LEN )
                    return MBXERR_NOMOREMEMORY;
#else
                if (pEthernetReceiveFrame != NULL)
                {
                    FREEMEM((MEM_ADDR MBXMEM *) pEthernetReceiveFrame);
                    pEthernetReceiveFrame = NULL;
                }

                pEthernetReceiveFrame = (MEM_ADDR MBXMEM *) ALLOCMEM( u16EthernetReceiveSize );
#endif
                if ( pEthernetReceiveFrame == NULL )
                    return MBXERR_NOMOREMEMORY;

                /* u16EthernetReceiveOffset stores the actual offset of the Ethernet frame,
                   where the next fragment is copied to */
                u16EthernetReceiveOffset = 0;
                /* store the frameNo, which has to be the same for all fragments of the whole Ethernet frame */
                u16ReceiveFrameNo = SWAPWORD(pEoe->Flags2) & EOEHEADER_FRAMENO;
            }
            else
            {
                /* fragment > 0, the next correct fragment is received,
                   get the offset of the fragment inside the Ethernet frame */
                UINT16 offset = ((SWAPWORD(pEoe->Flags2) & EOEHEADER_OFFSETBUFFER) >> EOEHEADERSHIFT_OFFSETBUFFER) << 5;    // * 32
                /* get the sent frameNo */
                UINT16 frameNo = SWAPWORD(pEoe->Flags2) & EOEHEADER_FRAMENO;

                /* check if offset is correct and frameNo match to the expected values */
                if ( offset != u16EthernetReceiveOffset || frameNo != u16ReceiveFrameNo )
                {
                    /* wrong offset or wrong frameNo -> free buffer, ignore fragment */
#if !STATIC_ETHERNET_BUFFER
                    if (pEthernetReceiveFrame != NULL)
                    {
                        FREEMEM((MEM_ADDR MBXMEM *) pEthernetReceiveFrame);
                        pEthernetReceiveFrame = NULL;
                    }
#endif
#if MAILBOX_QUEUE
                    if(pMbx != NULL)
                    {
                        /*free mailbox buffer*/
                        APPL_FreeMailboxBuffer(pMbx);
                        pMbx = NULL;
                    }
#endif

                    /* next fragment which will be accepted is a the beginning of a new EoE frame (fragmentNo=0) */
                    u8ReceiveFragmentNo = 0;
                    return 0;
                }
            }

            /* subtract the EoE header from the mailbox data size to get the size of the fragment */
            mbxSize -= 4;
            /* check if new fragment fits in the allocated buffer (u16EthernetReceiveSize stores the size
               of the whole Ethernet frame) */
            if ( (u16EthernetReceiveOffset + mbxSize) <= u16EthernetReceiveSize )
            {
                /* fragment fits in buffer, copy fragment data */

                MBXMEMCPY(&((UINT8*)pEthernetReceiveFrame)[u16EthernetReceiveOffset],&pEoe[1], mbxSize);

                /* increment the offset, where the next fragment has to be stored */

                u16EthernetReceiveOffset += mbxSize;
                /* increment the expected fragmentNo */
                u8ReceiveFragmentNo++;
            }
            else
            {
                /* too much data -> free buffer, ignore fragment */
#if !STATIC_ETHERNET_BUFFER
                if (pEthernetReceiveFrame != NULL)
                {
                    FREEMEM((MEM_ADDR MBXMEM *) pEthernetReceiveFrame);
                    pEthernetReceiveFrame = NULL;
                }
#endif

#if MAILBOX_QUEUE
                if(pMbx != NULL)
                {
                    /*free mailbox buffer*/
                    APPL_FreeMailboxBuffer(pMbx);
                    pMbx = NULL;
                }
#endif
                /* next fragment which will be accepted is a the beginning of a new EoE frame (fragmentNo=0) */
                u8ReceiveFragmentNo = 0;
                return 0;
            }

            if ( SWAPWORD(pEoe->Flags1) & EOEHEADER_LASTFRAGMENT )
            {
                /* Ethernet frame is completely received */
                if ( SWAPWORD(pEoe->Flags1) & EOEHEADER_TIMESTAMPAPPENDED )
                {
                    /* time stamp appended, ignore time stamp (only for gateways), u16EthernetReceiveOffset holds
                       the length of the received Ethernet frame, subtract the size of the time stamp */
                    u16EthernetReceiveOffset -= 4;
                }

                /* call application function with the received frame and size */
                EOEAPPL_ReceiveFrameInd((UINT8 *) pEthernetReceiveFrame, u16EthernetReceiveOffset );
                /* next fragment which will be accepted is a the beginning of a new EoE frame (fragmentNo=0) */
                u8ReceiveFragmentNo = 0;

                // Ethernet frame was forwarded free receive buffer 
#if !STATIC_ETHERNET_BUFFER
                if (pEthernetReceiveFrame != NULL)
                {
                    FREEMEM((MEM_ADDR MBXMEM *) pEthernetReceiveFrame);
                    pEthernetReceiveFrame = NULL;
                }
#endif
            }

#if MAILBOX_QUEUE
            if(pMbx != NULL)
            {
                /*free mailbox buffer*/
                APPL_FreeMailboxBuffer(pMbx);
                pMbx = NULL;
            }
#endif

            return 0;

        case EOE_TYPE_INIT_REQ:
            /* EoE datagram with IP settings received */
            pEoeInit = (ETHERCAT_EOE_INIT MBXMEM *) &pEoe[1];
            if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSMACADDR )
            {
                /* EoE datagram contains MAC Address, so the MAC Address is expected (6 bytes) */
                expSize += 6;
                /* check if enough bytes were received */
                if ( mbxSize < expSize )
                    return MBXERR_SIZETOOSHORT;
            }
            if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSIPADDR )
            {
                /* EoE datagram contains IP Address, so the IP Address is expected (4 bytes) */
                expSize += 4;
                /* check if enough bytes were received */
                if ( mbxSize < expSize )
                    return MBXERR_SIZETOOSHORT;
            }
            if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSSUBNETMASK )
            {
                /* EoE datagram contains Subnet Mask, so the Subnet Mask is expected (4 bytes) */
                expSize += 4;
                /* check if enough bytes were received */
                if ( mbxSize < expSize )
                    return MBXERR_SIZETOOSHORT;
            }
            if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSDEFAULTGATEWAY )
            {
                /* EoE datagram contains Default Gateway, so the Default Gateway is expected (4 bytes) */
                expSize += 4;
                /* check if enough bytes were received */
                if ( mbxSize < expSize )
                    return MBXERR_SIZETOOSHORT;
            }
            if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSDNSSERVER )
            {
                /* EoE datagram contains Dns Server, so the Dns Server is expected (4 bytes) */
                expSize += 4;
                /* check if enough bytes were received */
                if ( mbxSize < expSize )
                    return MBXERR_SIZETOOSHORT;
            }
            if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSDNSNAME )
            {
                /* EoE datagram contains DNS name, so the DNS name is expected (1-32 bytes) */
                /* check if enough bytes were received */
                if ( mbxSize < (expSize+1) )
                    return MBXERR_SIZETOOSHORT;
            }

            /* EoE datagram is correct, call application function to store the settings */
            result = EOEAPPL_SettingsInd(pEoeInit);

/* ECATCHANGE_START(V5.11) EOE1*/
            pMbx->MbxHeader.Length = ETHERCAT_EOE_HEADER_LEN;
            pMbx->Data[1] = SWAPWORD(result);
/* ECATCHANGE_END(V5.11) EOE1*/

            /* Update EoE type flag */
            pMbx->Data[0] = SWAPWORD(EOE_TYPE_INIT_RES);
            break;
        case EOE_TYPE_GET_IP_PARAM_REQ:
/* ECATCHANGE_START(V5.11) EOE1*/
            result = EOEAPPL_GetSettingsInd(((ETHERCAT_EOE_INIT MBXMEM *) &pEoe[1]),&pMbx->MbxHeader.Length);
/* ECATCHANGE_END(V5.11) EOE1*/

            /* Update EoE type flag */
            pMbx->Data[0] = SWAPWORD(EOE_TYPE_GET_IP_PARAM_RES);
/* ECATCHANGE_START(V5.11) EOE1*/
            pMbx->Data[1] = SWAPWORD(result);
/* ECATCHANGE_END(V5.11) EOE1*/

            break;
        case EOE_TYPE_INIT_RES: // not needed for slave
        case EOE_TYPE_TIMESTAMP_RES: // only supported by gateways
        case EOE_TYPE_MACFILTER_REQ: // only supported by gateways
        case EOE_TYPE_MACFILTER_RES: // only supported by gateways
        default:
            /* Update EoE type flag */
            pMbx->Data[0] = SWAPWORD(EOE_TYPE_INIT_RES);

/* ECATCHANGE_START(V5.11) EOE1*/
            pMbx->MbxHeader.Length = ETHERCAT_EOE_HEADER_LEN;
            pMbx->Data[1] = SWAPWORD(EOE_RESULT_UNSUPPORTED_TYPE);
/* ECATCHANGE_END(V5.11) EOE1*/
            break;
        }
    }
    else
        return MBXERR_SIZETOOSHORT;

    if ( MBX_MailboxSendReq(pMbx, EOE_SERVICE) != 0 )
    {
        /* if the mailbox service could not be sent (or stored), the response will be
           stored in the variable pEoeSendStored and will be sent automatically
            from the mailbox handler (EOE_ContinueInd) when the send mailbox will be read
            the next time from the master */
        pEoeSendStored = pMbx;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function sends an EoE fragment
*////////////////////////////////////////////////////////////////////////////////////////

void SendFragment(void)
{
    /* size contains the number of bytes which still has to be sent of the Ethernet frame */
    UINT16 size = u16EthernetSendSize - u16EthernetSendOffset;
/* ECATCHANGE_START(V5.11) EOE2*/
    UINT8 result = 0;
/* ECATCHANGE_END(V5.11) EOE2*/
    ETHERCAT_EOE_HEADER MBXMEM * pEoe;

    if ( (size + ETHERCAT_EOE_HEADER_LEN + MBX_HEADER_SIZE) > u16SendMbxSize )
        /* the remaining bytes cannot be sent with one mailbox service, so we have to
           send the next fragment which must be dividable by 32,
           the available mailbox size shall be rounded down to a value dividable by 32 */
        size = ((u16SendMbxSize - ETHERCAT_EOE_HEADER_LEN - MBX_HEADER_SIZE) >> 5) << 5;

#if MAILBOX_QUEUE
    /* get a mailbox buffer to be sent */
    psWriteMbx = (TMBX MBXMEM *) APPL_AllocMailboxBuffer((size + ETHERCAT_EOE_HEADER_LEN + MBX_HEADER_SIZE));
    if (psWriteMbx == NULL)
    {
        /* set flag that the processing of the mailbox service will be checked in the
            function MBX_Main (called from ECAT_Main) */
        bReceiveMbxIsLocked = TRUE;
        return;
    }
#endif
    /* size of the mailbox data */
    psWriteMbx->MbxHeader.Length = size + ETHERCAT_EOE_HEADER_LEN;
    /* initialize the mailbox header */
    psWriteMbx->MbxHeader.Address = 0;
    psWriteMbx->MbxHeader.Flags[0] = 0;
    psWriteMbx->MbxHeader.Flags[MBX_OFFS_TYPE] = (UINT16) (MBX_TYPE_EOE << MBX_SHIFT_TYPE);

    /* pEoe is a pointer to the EoE part of the mailbox service */
    pEoe = (ETHERCAT_EOE_HEADER MBXMEM *) psWriteMbx->Data;
    /* check if it is the last fragment */
    if ( size == (u16EthernetSendSize - u16EthernetSendOffset) )
        pEoe->Flags1 = SWAPWORD(EOEHEADER_LASTFRAGMENT);
    else
        pEoe->Flags1 = 0;

    /* store the actual fragment number in the mailbox buffer */
    pEoe->Flags2 = SWAPWORD(u8SendFragmentNo);
    if ( u8SendFragmentNo )
    {
        /* it is not the first fragment, store the offset of the actual fragment to be sent
           in the mailbox buffer */
        pEoe->Flags2 |= SWAPWORD((u16EthernetSendOffset >> 5) << EOEHEADERSHIFT_OFFSETBUFFER);
    }
    else
    {
        /* it is the first fragment, store the size of the Ethernet frame (in 32 bytes blocks)
           in the mailbox buffer */
        pEoe->Flags2 |= SWAPWORD(((u16EthernetSendSize+31) >> 5) << EOEHEADERSHIFT_OFFSETBUFFER);
        /* the frame number is a 4-bit-field (bit 12-15) so we increment it here */
        u16SendFrameNo += 0x1000;
    }
    /* store the frame number in the mailbox buffer */
    pEoe->Flags2 |= SWAPWORD(u16SendFrameNo);

    /* copy the actual fragment in the mailbox buffer */
    MBXMEMCPY((UINT8 *)&pEoe[1], &((UINT8 *)pEthernetSendFrame)[u16EthernetSendOffset], size);

/* ECATCHANGE_START(V5.11) EOE2*/
    result = MBX_MailboxSendReq(psWriteMbx, EOE_SERVICE);
    if ( result != 0 )
/* ECATCHANGE_END(V5.11) EOE2*/
    {
        /* if the mailbox service could not be sent (or stored), the response will be
           stored in the variable pEoeSendStored and will be sent automatically
            from the mailbox handler (EOE_ContinueInd) when the send mailbox will be read
            the next time from the master */
        pEoeSendStored = psWriteMbx;
    }

    if ( size == (u16EthernetSendSize - u16EthernetSendOffset) )
    {
        /* it was the last fragment, we can return the buffer to the memory management */
#if !STATIC_ETHERNET_BUFFER
        if(pEthernetSendFrame != NULL)
        {
            FREEMEM((void MBXMEM *) pEthernetSendFrame);
            pEthernetSendFrame = NULL;
        }
#endif
        /* next frame can be sent */
        bEoESendFramePending = FALSE;

/* ECATCHANGE_START(V5.11) EOE2*/
        if (result == 0)
        {
            /*clear the pending EoE flag only if the last EoE Fragment was successfully written to the mailbox buffer*/
            u8MailboxSendReqStored &= ~EOE_SERVICE;
        }
        else
        {
            /*Copy the last EoE fragment to the mailbox buffer failed. 
            Set the EoE pending indication to copy the fragment to the mailbox buffer when the buffer was read by the master*/
            u8MailboxSendReqStored |= EOE_SERVICE;
        }
/* ECATCHANGE_END(V5.11) EOE2*/
    }
    else
    {
        /* we have to increment the offset for the next fragment to be sent */
        u16EthernetSendOffset += size;
        /* increment the fragment number */
        u8SendFragmentNo++;
        u8MailboxSendReqStored |= EOE_SERVICE;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx      Pointer to the free mailbox buffer

 \brief    This function is called when the next mailbox fragment can be sent.
*////////////////////////////////////////////////////////////////////////////////////////

void EOE_ContinueInd(TMBX MBXMEM * pMbx)
{
    if ( pEoeSendStored )
    {
        /* send the stored EoE service which could not be sent before */
        MBX_MailboxSendReq(pEoeSendStored, 0);
        pEoeSendStored = 0;
    }
    else if ( bEoESendFramePending )
    {
        /* send the next fragment of the actual frame */
        SendFragment();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pFrame      Pointer to the Ethernet frame to be sent
 \param     frameSize  size of the Ethernet frame to be sent

 \return    0 = sending of frame started, 1 = frame could not be sent, try it later

 \brief    This function is called from the application to sent an Ethernet frame
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 EOE_SendFrameReq(UINT8 MBXMEM * pFrame, UINT16 frameSize)
{
    if ( !bEoESendFramePending && nAlStatus != STATE_INIT
        && (pEoeSendStored == NULL || pFrame == (UINT8 MBXMEM *)pEoeSendStored)
#if !MAILBOX_QUEUE        
/* ECATCHANGE_START(V5.11) MBX3*/
        || (bPendingMbxResponse == TRUE)
/* ECATCHANGE_END(V5.11) MBX3*/
#endif
        )
    {
        /* no Ethernet is sent yet, no datagram is currently stored and the slave is at least in PRE-OP,
           so we could sent the requested frame */
        DISABLE_MBX_INT;
        /* Ethernet frame is to be sent */
        bEoESendFramePending        = TRUE;
        /* store the size of the Ethernet frame to be sent */
        u16EthernetSendSize      = frameSize;
        /* store the buffer of the Ethernet frame to be sent */
        pEthernetSendFrame       = (MEM_ADDR MBXMEM *)pFrame;
        /* we start with the first fragment */
        u16EthernetSendOffset    = 0;
        u8SendFragmentNo         = 0;

        SendFragment();

        ENABLE_MBX_INT;
    }
    else
        /* frame could not be sent, try it later */
        return 1;

    return 0;
}

/** @} */

#endif /* EOE_SUPPORTED */


