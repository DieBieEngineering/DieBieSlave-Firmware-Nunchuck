/**
\addtogroup EoE Ethernet over EtherCAT
@{
*/

/**
\file eoeappl.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains an example how to use the EoE services

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 EOE1: update mailbox length calculation on EoE response<br>
<br>Changes to version V5.01:<br>
V5.10 EOE1: Prevent memeory leaks on incomplete EoE sequences<br>
V5.10 EOE5: Support "Get IP Parameter" (4Byte EoE requests also valid<br>
            Change setting of EoE response flag)<br>
<br>Changes to version V5.0:<br>
V5.01 EOE1: Return an error in case that DHCP handling is required.<br>
V5.01 EOE2: Add Ethernet frame buffer in case that static Ethernet buffer is used.<br>
<br>Changes to version V4.08:<br>
V5.0 EOE1: Support static Ethernet buffer.<br>
<br>Changes to version V4.06:<br>
V4.07 EOEAPPL 1: The memory define (for far or huge memory) was missing<br>
V4.07 EOEAPPL 2: The pointer pARP was not needed<br>
*/

/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if EOE_SUPPORTED

#include "ecatslv.h"

#define    _EOEAPPL_    1
#include "eoeappl.h"
#undef      _EOEAPPL_
/* ECATCHANGE_START(V5.11) ECAT10*/
/*remove definition of _EOEAPPL_ (#ifdef is used in eoeappl.h)*/
/* ECATCHANGE_END(V5.11) ECAT10*/
/*---------------------------------------------------------------------------------------
------
------    internal Types and Defines
------
---------------------------------------------------------------------------------------*/

#define    ECATEOE        0x4300
#define    ECATEOEMAX    0x02

/*---------------------------------------------------------------------------------------
------
------    static variables
------
---------------------------------------------------------------------------------------*/

/*Create broadcast Ethernet address*/
const    UINT8    BroadcastEthernetAddress[6]={0xff,0xff,0xff,0xff,0xff,0xff};

UINT8 aIpAdd[4];
UINT8 aMacAdd[6];

#if STATIC_ETHERNET_BUFFER
UINT8 			aEthernetSendBuffer[ETHERNET_MAX_FRAME_LEN];
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

 \brief    This function intialize the EoE application Interface.
*////////////////////////////////////////////////////////////////////////////////////////

void EOEAPPL_Init(void)
{
}


/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function calculates a checksum (only for an even number of bytes).
 \brief Note that if you are going to checksum a checksummed packet that includes the checksum,
 \brief you have to compliment the output.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 EOEAPPL_CalcCheckSum (UINT16 MBXMEM *pWord, UINT16 nLen)
{
    UINT32 crc;
    UINT32 CrcLo;
    UINT32 CrcHi;
    UINT16 RetCrc;

    crc = 0;
    while (nLen > 1)
    {
        crc += SWAPWORD(*pWord);
          pWord++;
          nLen -= 2;
    }
    if ( nLen == 1 )                          // if nLen odd
          crc += *((UINT8*)pWord);
    CrcLo = LOWORD(crc);
    CrcHi = HIWORD(crc);
    crc = CrcLo + CrcHi;

    CrcHi = HIWORD(crc);
    crc += CrcHi;
    if (crc == 0xFFFF)                     // remove the -0 ambiguity
          crc = 0;

    RetCrc = (UINT16)crc;
    RetCrc = ~RetCrc;
    return(RetCrc);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pFrame      Pointer to the received Ethernet frame (must be returned with
                            FREEMEM)
 \param     frameSize   Size of the received Ethernet frame

 \brief    This function is called when an Ethernet frame is received over EoE
*////////////////////////////////////////////////////////////////////////////////////////

void EOEAPPL_ReceiveFrameInd(UINT8 MBXMEM * pFrame, UINT16 frameSize)
{
    switch ( ((ETHERNET_FRAME *) pFrame)->FrameType )
    {
    case ETHERNET_FRAME_TYPE_ARP1_SW:
        {
#if STATIC_ETHERNET_BUFFER
            ETHERNET_FRAME MBXMEM * pSendFrame = (ETHERNET_FRAME MBXMEM *)aEthernetSendBuffer;
#else
            ETHERNET_FRAME MBXMEM * pSendFrame = (ETHERNET_FRAME MBXMEM *) ALLOCMEM(frameSize);
#endif
            ARP_IP_HEADER MBXMEM    * pArpIp = (ARP_IP_HEADER MBXMEM    *) &pSendFrame[1];

            /*Copy Receive Frame to create ARP Reply*/
            MBXMEMCPY(pSendFrame,pFrame,frameSize);
            if ( ( MBXMEMCMP(BroadcastEthernetAddress, pSendFrame->Destination.b, 4) == 0 )
                &&( pArpIp->hwAddrSpace == SWAPWORD(ARP_HW_ADDR_SPACE_ETHERNET_SW) )
                &&( pArpIp->lengthHwAddr == ETHERNET_ADDRESS_LEN )
                &&( pArpIp->protAddrSpace == SWAPWORD(ETHERNET_FRAME_TYPE_IP_SW) )
                &&( pArpIp->lengthProtAddr == SIZEOF(UINT32) )
                &&( pArpIp->opcode == SWAPWORD(ARP_OPCODE_REQUEST_SW) )
                )
            {
                MBXMEMCPY(pSendFrame->Destination.b, pSendFrame->Source.b, 6);
                MBXMEMCPY(pSendFrame->Source.b, &aMacAdd[0], 6);

                MBXMEMCPY(pArpIp->macDest.b, pArpIp->macSource.b, 6);
                MBXMEMCPY(pArpIp->macSource.b, &aMacAdd[0], 6);

                MBXMEMCPY( &pArpIp->ipDest, &pArpIp->ipSource, 4);
                MBXMEMCPY( &pArpIp->ipSource, &aIpAdd[0], 4);

                pArpIp->opcode = SWAPWORD(ARP_OPCODE_REPLY_SW);

                EOE_SendFrameReq((UINT8 MBXMEM *) pSendFrame, ARP_IP_HEADER_LEN + ETHERNET_FRAME_LEN);
            }
        }
        break;
    case ETHERNET_FRAME_TYPE_IP_SW:
        {
#if STATIC_ETHERNET_BUFFER
            ETHERNET_IP_MAX_FRAME MBXMEM * pIPHeader = (ETHERNET_IP_MAX_FRAME MBXMEM *) aEthernetSendBuffer;
#else
            ETHERNET_IP_MAX_FRAME MBXMEM * pIPHeader = (ETHERNET_IP_MAX_FRAME MBXMEM *) ALLOCMEM(frameSize);			
#endif

            /*Copy Receive Frame to create ICMP Reply*/
            MBXMEMCPY(pIPHeader,pFrame,frameSize);

            if (  ( pIPHeader->Ip.protocol == IP_PROTOCOL_ICMP )
                &&( pIPHeader->IpData.Icmp.type == ICMP_TYPE_ECHO )
                &&( MBXMEMCMP(&pIPHeader->Ip.dest, aIpAdd, 4) == 0 )
                )
            {
                // ping requested
                UINT16 length;
#if !BIG_ENDIAN_FORMAT
                UINT16 lo = 0;
                UINT16 hi = 0;
#endif
                UINT32 tmp;

#if BIG_ENDIAN_FORMAT
                length = pIPHeader->Ip.length;
#else
                // length is in BigEndian format -> swap bytes
                lo = (( pIPHeader->Ip.length) & 0xff) << 8;
                hi = pIPHeader->Ip.length >> 8;
                length = hi + lo;
#endif
                // swap src and dest ip address
                tmp = pIPHeader->Ip.src;
                pIPHeader->Ip.src = pIPHeader->Ip.dest;
                pIPHeader->Ip.dest = tmp;

                // set ping reply command
                pIPHeader->IpData.Icmp.type = ICMP_TYPE_ECHO_REPLY;

                // swap src and dest mac address
                MBXMEMCPY(pIPHeader->Ether.Destination.b, pIPHeader->Ether.Source.b, 6);
                MBXMEMCPY(pIPHeader->Ether.Source.b, aMacAdd, 6);

                // calculate ip checksum
                pIPHeader->Ip.cksum = 0;
                pIPHeader->Ip.cksum = SWAPWORD(EOEAPPL_CalcCheckSum((UINT16 MBXMEM *) &pIPHeader->Ip, IP_HEADER_MINIMUM_LEN));
                // calculate icmp checksum
                pIPHeader->IpData.Icmp.checksum = 0;
                /* type cast because of warning was added */
                pIPHeader->IpData.Icmp.checksum = SWAPWORD(EOEAPPL_CalcCheckSum((UINT16 MBXMEM *) &pIPHeader->IpData.Icmp, (UINT16) (length - 20)));
                /* type cast because of warning was added */
                EOE_SendFrameReq((UINT8 MBXMEM *) pIPHeader, (UINT16) (ETHERNET_FRAME_LEN + length));
            }
            else
            {
                //protocol not supported => free allocated buffer
                if(pIPHeader != NULL)
                {
                    FREEMEM(pIPHeader);
                    pIPHeader = NULL;
                }
            }
        }
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pEoeInit   Pointer to store the IP settings
 \param     pMbxLength Pointer to length field in the mailbox header

 \return    0          Setting information were written

 \brief    This function is called when the IP get settings were received over EoE
*////////////////////////////////////////////////////////////////////////////////////////
/* ECATCHANGE_START(V5.11) EOE1*/
UINT16 EOEAPPL_GetSettingsInd(ETHERCAT_EOE_INIT MBXMEM *pEoeInit,UINT16 *pMbxLength)
{
    /* Clear include Flags */
    pEoeInit->Flags1 = 0;
    pEoeInit->Flags2 = 0;

    /* set initial get IP parameter response length */
    *pMbxLength = 10;

     /* set MAC Address */
    MBXMEMCPY(&pEoeInit->MacAddr,aMacAdd, 6);
    pEoeInit->Flags1 |= EOEINIT_CONTAINSMACADDR;
    *pMbxLength += 6;

    // set IP Address
    ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[3] = aIpAdd[0];
    ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[2] = aIpAdd[1];
    ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[1] = aIpAdd[2];
    ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[0] = aIpAdd[3];
    pEoeInit->Flags1 |= EOEINIT_CONTAINSIPADDR;
    *pMbxLength += 4;

    pEoeInit->Flags1 = SWAPWORD(pEoeInit->Flags1);
    
    return 0;
}
/* ECATCHANGE_END(V5.11) EOE1*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pEoeInit   Pointer to the received ip settings

 \return               0 (IP settings were correct), != 0 (IP settings were not correct)

 \brief    This function is called when the IP settings were received over EoE
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 EOEAPPL_SettingsInd(ETHERCAT_EOE_INIT MBXMEM *pEoeInit)
{
    UINT16 result = 0;

    if ( SWAPWORD(pEoeInit->Flags1) & EOEINIT_CONTAINSMACADDR )
    {
        // set MAC Address
        MBXMEMCPY(aMacAdd, &pEoeInit->MacAddr, 6);
    }
    if(!(pEoeInit->Flags1 & EOEINIT_CONTAINSIPADDR)
        ||((pEoeInit->Flags1 & EOEINIT_CONTAINSIPADDR) && (pEoeInit->IpAddr == 0)))
    {
        /*no IP defined (IP is assigned via DHCP) => not supported yet*/
        result = EOE_RESULT_NO_DHCP_SUPPORT;
    }
    else
    {
        // set IP Address
        aIpAdd[0] = ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[3];
        aIpAdd[1] = ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[2];
        aIpAdd[2] = ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[1];
        aIpAdd[3] = ((UINT8 MBXMEM *) &pEoeInit->IpAddr)[0];
    }

    return result;
}


/** @} */


#endif /* EOE_SUPPORTED */


