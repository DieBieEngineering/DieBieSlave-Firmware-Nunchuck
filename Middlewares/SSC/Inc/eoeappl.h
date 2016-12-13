/**
 * \addtogroup EoE Ethernet over EtherCAT
 * @{
 */

/**
\file eoeappl.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 EOE1: update mailbox length calculation on EoE response<br>
<br>Changes to version V5.01:<br>
V5.10 EOE5: Support "Get IP Parameter" (4Byte EoE requests also valid<br>
            Change setting of EoE response flag)<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */
#ifndef _EOEAPPL_H_

#define _EOEAPPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecateoe.h"


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

#define    ETHERNET_MAX_FRAME_LEN         1514 /**< \brief Max Ethernet frame length*/
#define    ETHERNET_MAX_VLAN_FRAME_LEN    1518 /**< \brief Max Ethernet VLAN frame length*/
#define    ETHERNET_MAX_FRAMEBUF_LEN      1536 /**< \brief Max Ethernet frame buffer size*/

#define    ARP_HW_ADDR_SPACE_ETHERNET_SW  0x0100 /**< \brief Hardware Address Space: 1 = Ethernet*/
#define    ETHERNET_FRAME_TYPE_IP_SW      0x0008 /**< \brief EtherType IP*/
#define    ETHERNET_FRAME_TYPE_ARP1_SW    0x0608 /**< \brief EtherType ARP*/

/*CODE_INSERT_START (MBX_FILE_PACKED_START)*/
#if FC1100_HW
#pragma pack(push, 1)
#endif
/*CODE_INSERT_END*/


/**
 * \brief Ethernet header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    ETHERNET_ADDRESS    Destination; /**< \brief Destination MAC address*/
    ETHERNET_ADDRESS    Source; /**< \brief Source MAC address*/
    UINT16              FrameType; /**< \brief EtherType (in host-order)*/
}MBX_STRUCT_PACKED_END
ETHERNET_FRAME;


#define    ETHERNET_FRAME_LEN        SIZEOF(ETHERNET_FRAME) /**< \brief Ethernet header size*/
#define    ETHERNET_FRAMETYPE_LEN    SIZEOF(UINT16) /**< \brief Ethernet Type size*/


/**
 * \brief ARP/IP Header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    UINT16              hwAddrSpace;  /**< \brief Hardware Address Space: 1 = Ethernet*/
    UINT16              protAddrSpace;  /**< \brief ETHERNET_FRAME_TYPE_IP*/
    UINT8               lengthHwAddr;  /**< \brief Length of Hardware address (6)*/
    UINT8               lengthProtAddr;  /**< \brief Length of Port address (4)*/
    UINT16              opcode;  /**< \brief 1 = request, 2 = reply*/
    ETHERNET_ADDRESS    macSource; /**< \brief Source MAC*/
    UINT32              ipSource; /**< \brief Source IP*/
    ETHERNET_ADDRESS    macDest; /**< \brief Destination MAC*/
    UINT32              ipDest; /**< \brief Destination IP*/
}MBX_STRUCT_PACKED_END
ARP_IP_HEADER;

#define    ARP_IP_REPLY_PATTERN_LEN         18 /**< \brief ARP Reply pattern length*/
#define    ARP_AMS_REPLY_PATTERN_LEN        14 /**< \brief AMS Reply pattern length*/

#define    ARP_IP_HEADER_LEN                SIZEOF(ARP_IP_HEADER) /**< \brief ARP/IP Header length*/
#define    ARP_IP_REPLY_HEADER_LEN          (ARP_IP_HEADER_LEN + ARP_IP_REPLY_PATTERN_LEN) /**< \brief ARP Reply header length*/

#define    ARP_OPCODE_REQUEST_SW            0x0100 /**< \brief ARP Request opcode*/
#define    ARP_OPCODE_REPLY_SW              0x0200 /**< \brief ARP Reply opcode*/


/**
 * \brief ARP/IP Data
 */
typedef struct MBX_STRUCT_PACKED_START
{
    ETHERNET_FRAME       ether;  /**< \brief Ethernet header*/
    ARP_IP_HEADER        arp; /**< \brief ARP/IP header*/
}MBX_STRUCT_PACKED_END
ARP_IP_DATA;


/**
 * \brief IP Header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    UINT8        x;  /**< \brief Version and header length*/
    UINT8        tos; /**< \brief Type of service*/
    UINT16       length; /**< \brief Total length*/
    UINT16       identifier; /**< \brief Identification*/
#define IP_MF 0x2000 /**< \brief More fragments (stored in "fragment")*/
    UINT16       fragment; /**< \brief Flags and Fragment offset*/
    UINT8        ttl; /**< \brief Time to live*/
    UINT8        protocol; /**< \brief following protocol*/
    UINT16       cksum; /**< \brief Checksum*/
    UINT32       src; /**< \brief Source IP*/
    UINT32       dest; /**< \brief Destination IP*/
}MBX_STRUCT_PACKED_END
IP_HEADER;


#define    IP_HEADER_DEFAULT_X      0x45 /**< \brief Default Version and header (version 4, header length 20)*/
#define    IP_HEADER_DEFAULT_TTL    0x80 /**< \brief DEfault time to live*/
#define    IP_HEADER_DEFAULT_TOS    0x00/**< \brief Default Type of service*/
#define    IP_HEADER_DEFAULT_FRAG   0x0000/**< \brief DEfault fragments*/

#define    IP_HEADER_MINIMUM_LEN    20/**< \brief Minimum IP header length*/

#define    IP_PROTOCOL_ICMP         1 /**< \brief Protocol ICMP*/
#define    IP_PROTOCOL_TCP          6 /**< \brief Protocol TCP*/
#define    IP_PROTOCOL_UDP          17 /**< \brief Protocol UDP*/


/**
 * \brief UDP Header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    UINT16    src_port;  /**< \brief Source port*/
    UINT16    dest_port; /**< \brief Destination port*/
    UINT16    length;  /**< \brief Length (including this header)*/
    UINT16    checksum; /**< \brief Checksum*/
}MBX_STRUCT_PACKED_END
UDP_HEADER;


#define    UDP_HEADER_LEN            SIZEOF(UDP_HEADER) /**< \brief UDP header length*/

#define    UDP_PORT_BKHF              0x88A4 /**< \brief EtherCAT UDP Port*/



/**
 * \brief TCP Header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    UINT16        portSource; /**< \brief Source port*/
    UINT16        portDest; /**< \brief Destination port*/
    UINT32        seqNo; /**< \brief Sequence number*/
    UINT32        ackNo; /**< \brief Acknowledge number*/
    UINT16        flags; /**< \brief Flags*/
/*    struct
    {
        UINT16        dataOffset    : 4;
        UINT16        reserved        : 6;
        UINT16        urg            : 1;
        UINT16        ack            : 1;
        UINT16        psh            : 1;
        UINT16        rst            : 1;
        UINT16        syn            : 1;
        UINT16        fin            : 1;
    };*/
    UINT16        window; /**< \brief Window*/
    UINT16        checksum; /**< \brief Checksum*/
    UINT16        urgentPtr; /**< \brief Urgent*/
    UINT32        options; /**< \brief Options*/
/*    struct
    {
        UINT32        options        : 24;
        UINT32        padding        : 8;
    };*/

}MBX_STRUCT_PACKED_END
TCP_HEADER;


/**
 * \brief ICMP Header
 */
typedef struct MBX_STRUCT_PACKED_START
{
    UINT8       type; /**< \brief Type*/
    UINT8       code; /**< \brief Code*/
    UINT16      checksum; /**< \brief Checksum*/
    union
    {
        struct
        {
            UINT8        pointer;  /**< \brief Pointer*/
        } p;
        struct
        {
            UINT16    id; /**< \brief ID*/
            UINT16    seqNo; /**< \brief Sequence number*/
        } i;
        UINT32    gatewayAddr; /**< \brief Gateway address*/
    } u; /**< \brief Data*/
}MBX_STRUCT_PACKED_END
ICMP_HEADER;

#define    ICMP_HEADER_LEN                  8 /**< \brief ICMP header length*/


#define    ICMP_TYPE_ECHO_REPLY             0 /**< \brief Echo Reply*/
#define    ICMP_TYPE_DEST_UNREACHABLE       3 /**< \brief Destination Unreachable*/
#define    ICMP_TYPE_SOURCE_QUENCH          4 /**< \brief Source Quench*/
#define    ICMP_TYPE_REDIRECT               5 /**< \brief Redirect*/
#define    ICMP_TYPE_ECHO                   8 /**< \brief Echo*/
#define    ICMP_TYPE_TIME_EXCEEDED          11 /**< \brief Time Exceeded*/
#define    ICMP_TYPE_PARA_PROBLEM           12 /**< \brief Parameter Problem*/
#define    ICMP_TYPE_TIMESTAMP              13 /**< \brief Timestamp*/
#define    ICMP_TYPE_TIMESTAMP_REPLY        14 /**< \brief Timestamp Reply*/
#define    ICMP_TYPE_INFO_REQUEST           15 /**< \brief Information Request*/
#define    ICMP_TYPE_INFO_REPLY             16 /**< \brief Information Reply*/


/**
 * \brief IP Frame
 */
typedef struct MBX_STRUCT_PACKED_START
{
    ETHERNET_FRAME          Ether; /**< \brief Ethernet header*/
    IP_HEADER               Ip; /**< \brief IP header*/
    union
    {
        UDP_HEADER          Udp; /**< \brief UDP header*/
        TCP_HEADER          Tcp; /**< \brief TCP header*/
        ICMP_HEADER         Icmp; /**< \brief ICMP header*/
        UINT8               Data[(ETHERNET_MAX_FRAME_LEN)-(ETHERNET_FRAME_LEN)-(IP_HEADER_MINIMUM_LEN)]; /**< \brief payload*/
    } IpData; /**< \brief IP data*/
}MBX_STRUCT_PACKED_END
ETHERNET_IP_MAX_FRAME;


/*CODE_INSERT_START (MBX_FILE_PACKED_END)*/
#if FC1100_HW
#pragma pack(pop)
#endif
/*CODE_INSERT_END*/

#endif //_EOEAPPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/

/* ECATCHANGE_START(V5.11) ECAT10*/
#if defined(_EOEAPPL_) && (_EOEAPPL_ == 1)
/* ECATCHANGE_END(V5.11) ECAT10*/
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO    void EOEAPPL_Init(void);
PROTO    void EOEAPPL_ReceiveFrameInd(UINT8 MBXMEM * pFrame, UINT16 frameSize);
PROTO    UINT16 EOEAPPL_CalcCheckSum (UINT16 MBXMEM *pWord, UINT16 nLen);
PROTO    UINT16 EOEAPPL_SettingsInd(ETHERCAT_EOE_INIT MBXMEM *pEoeInit);
/* ECATCHANGE_START(V5.11) EOE1*/
UINT16 EOEAPPL_GetSettingsInd(ETHERCAT_EOE_INIT MBXMEM *pEoeInit,UINT16 *pMbxLength);
/* ECATCHANGE_END(V5.11) EOE1*/

#undef PROTO
/** @}*/
