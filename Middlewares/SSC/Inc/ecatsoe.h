/**
 * \addtogroup SoE Servo Drive Profile over EtherCAT
 * @{
 */

/**
\file ecatsoe.h
\author EthercatSSC@beckhoff.com
\brief SoE structures and defines

\version 5.01

<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _ECATSOE_H_
#define _ECATSOE_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "mailbox.h"

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

/*---------------------------------------------
-    Error Codes
-----------------------------------------------*/
#define    ERROR_SOEINVALIDSERVICE      0x1001 /**< \brief SoE Error Code : Invalid service*/
#define    ERROR_SOENOTSUPPORTED        0x1002 /**< \brief SoE Error Code : Not supported*/


/*---------------------------------------------
-    SOE services
-----------------------------------------------*/
#define    ECAT_SOE_OPCODE_RRQ        1 /**< \brief SoE Read Request*/
#define    ECAT_SOE_OPCODE_RRS        2 /**< \brief SoE Read Response*/
#define    ECAT_SOE_OPCODE_WRQ        3 /**< \brief SoE Write Request*/
#define    ECAT_SOE_OPCODE_WRS        4 /**< \brief SoE Write Response*/
#define    ECAT_SOE_OPCODE_NFC        5 /**< \brief SoE Notification Request*/
#define    ECAT_SOE_OPCODE_EMCY       6 /**< \brief SoE Emergency*/


/*---------------------------------------------
-    SOE Structures
-----------------------------------------------*/

/** \brief SoE Header*/
typedef struct MBX_STRUCT_PACKED_START
{
    union MBX_STRUCT_PACKED_START
    {
        UINT16 Word; /**< \brief Flags buffer*/
        #define    SOEFLAGS_OPCODE        0x0007 /**< \brief SoE Flags*/
                                        /**<
                                        * 0 = unused<br>
                                        * 1 = readReq<br>
                                        * 2 = readRes<br>
                                        * 3 = writeReq<br>
                                        * 4 = writeRes<br>
                                        * 5 = notification (command changed notification)*/
        #define    SOEFLAGS_INCOMPLETE    0x0008    /**< \brief more follows*/
        #define    SOEFLAGS_ERROR         0x0010    /**< \brief an error word follows*/
        #define    SOEFLAGS_DRIVENO       0x00E0    /**< \brief drive number*/

        #define    SOEFLAGS_DATASTATE     0x0100    /**< \brief Data state follows or requested*/
        #define    SOEFLAGS_NAME          0x0200    /**< \brief Name follows or requested*/
        #define    SOEFLAGS_ATTRIBUTE     0x0400    /**< \brief Attribute follows or requested*/
        #define    SOEFLAGS_UNIT          0x0800    /**< \brief Unit follows or requested*/
        #define    SOEFLAGS_MIN           0x1000    /**< \brief Min value follows or requested*/
        #define    SOEFLAGS_MAX           0x2000    /**< \brief Max value follows or requested*/
        #define    SOEFLAGS_VALUE         0x4000    /**< \brief Value follows or requested*/
        #define    SOEFLAGS_DEFAULT       0x8000    /**< \brief Default value*/
    }MBX_STRUCT_PACKED_END
    Flags;/**< \brief Flags variable*/

    UINT16        IDN_Frag;  /**< \brief IDN fragments 
                              * 
                              * if (InComplete==0) SERCOS IDN else FragmentsLeft
                              */
}MBX_STRUCT_PACKED_END
TSOEHEADER;

#define SOE_HEADER_SIZE     SIZEOF(TSOEHEADER) /**< \brief Sdo header size*/

/**
 *Complete SoE Datagram including mailbox header, SoE header data
 */
typedef struct MBX_STRUCT_PACKED_START
{
  TMBXHEADER        MbxHeader; /**< \brief Mailbox header*/
  TSOEHEADER        SoeHeader; /**< \brief SoE header*/
  UINT8             Data[(MAX_MBX_SIZE) - SOE_HEADER_SIZE]; /**< \brief buffer for SoE data*/
}MBX_STRUCT_PACKED_END
TSOEMBX;

#endif //_ECATSOE_H_

/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/

#if _ECATSOE_
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO    void SOE_Init(void);
PROTO    UINT8 SOE_ServiceInd(TMBX MBXMEM *pReceiveMbx);
PROTO    void SOE_ContinueInd(TMBX MBXMEM * pMbx);
PROTO    void SOE_ReadInd(TSOEMBX MBXMEM *pSoeMbx);
PROTO    void SOE_WriteInd(TSOEMBX MBXMEM *pSoeMbx);
PROTO    UINT16 SOE_NotificationReq(UINT16 idn, UINT16 channel, UINT16 dataState);

#undef PROTO
/** @}*/
