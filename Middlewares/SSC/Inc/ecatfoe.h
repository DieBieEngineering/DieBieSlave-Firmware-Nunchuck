/**
 * \addtogroup FoE FileTransfer over EtherCAT
 * @{
 */

/**
\file ecatfoe.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 FOE1: handle busy on a read request, change OPMode to OPCode (same terms as in the spec)<br>
<br>Changes to version V5.01:<br>
V5.10 FOE1: Add new FoE error codes (0x800C,0x800D,0x800F)<br>
V5.10 FOE2: Update file handling to support File sizes greater than 0x8000 Bytes (change value of "FOE_MAXDATA")<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _ECATFOE_H_
#define _ECATFOE_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "esc.h"
#include "mailbox.h"

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

/*---------------------------------------------
-    Error Codes
-----------------------------------------------*/
#define    ECAT_FOE_ERRCODE_NOTDEFINED          0x8000 /**< \brief Not defined*/
#define    ECAT_FOE_ERRCODE_NOTFOUND            0x8001 /**< \brief File not found*/
#define    ECAT_FOE_ERRCODE_ACCESS              0x8002 /**< \brief No file access*/
#define    ECAT_FOE_ERRCODE_DISKFULL            0x8003 /**< \brief Disk is full*/
#define    ECAT_FOE_ERRCODE_ILLEGAL             0x8004 /**< \brief Illegal access*/
#define    ECAT_FOE_ERRCODE_PACKENO             0x8005 /**< \brief Invalid packet number*/
#define    ECAT_FOE_ERRCODE_EXISTS              0x8006 /**< \brief File already exists*/
#define    ECAT_FOE_ERRCODE_NOUSER              0x8007 /**< \brief No User*/
#define    ECAT_FOE_ERRCODE_BOOTSTRAPONLY       0x8008 /**< \brief Only in Bootstrap state*/
#define    ECAT_FOE_ERRCODE_NOTINBOOTSTRAP      0x8009 /**< \brief Downloaded file name is not valid in Bootstrap state*/
#define    ECAT_FOE_ERRCODE_NORIGHTS            0x800A /**< \brief No access rights*/
#define    ECAT_FOE_ERRCODE_PROGERROR           0x800B /**< \brief Program error*/
#define    ECAT_FOE_ERRCODE_INVALID_CHECKSUM    0x800C /**< \brief Wrong checksum */
#define    ECAT_FOE_ERRCODE_INVALID_FIRMWARE    0x800D /**< \brief Invalid firmware file*/
#define    ECAT_FOE_ERRCODE_NO_FILE             0x800F /**< \brief No file to read*/



/*---------------------------------------------
-    Services
-----------------------------------------------*/
/* ECATCHANGE_START(V5.11) FOE1*/
#define    ECAT_FOE_OPCODE_RRQ                  1 /**< \brief Read request*/
#define    ECAT_FOE_OPCODE_WRQ                  2 /**< \brief Write request*/
#define    ECAT_FOE_OPCODE_DATA                 3 /**< \brief Data datagram*/
#define    ECAT_FOE_OPCODE_ACK                  4 /**< \brief Acknowledge datagram*/
#define    ECAT_FOE_OPCODE_ERR                  5 /**< \brief Error datagram*/
#define    ECAT_FOE_OPCODE_BUSY                 6 /**< \brief Busy datagram*/
/* ECATCHANGE_END(V5.11) FOE1*/


/*---------------------------------------------
-    States
-----------------------------------------------*/
#define    FOE_READY                            0 /**< \brief Ready state*/
#define    FOE_WAIT_FOR_ACK                     1 /**< \brief Wait for acknowledgement state*/
#define    FOE_WAIT_FOR_DATA                    2 /**< \brief Wait for data state*/
#define    FOE_WAIT_FOR_RESET                   3 /**< \brief Wait for reset state*/
#define    FOE_WAIT_FOR_LAST_ACK                4 /**< \brief Wait for last acknowledgement state*/
#define    FOE_PROGRAMMING                      5 /**< \brief Programming state*/
#define    FOE_WAIT_FOR_LAST_DATA               6 /**< \brief Wait for last data state*/


/*CODE_INSERT_START (MBX_FILE_PACKED_START)*/
#if FC1100_HW
#pragma pack(push, 1)
#endif
/*CODE_INSERT_END*/


typedef struct MBX_STRUCT_PACKED_START
{
    UINT16    Cmd;
        #define    EFW_CMD_IGNORE               0
        #define    EFW_CMD_MEMORY_TRANSFER      1
        #define    EFW_CMD_WRCODE               2
        #define    EFW_CMD_CHK_DEVID            3
        #define    EFW_CMD_CHK_DEVICEID         3
        #define    EFW_CMD_CHKSUM               4
        #define    EFW_CMD_WRCODECHKSUM         5
        #define    EFW_CMD_SET_DEVID            6
        #define    EFW_CMD_CHKSUMCHKSUM         6
        #define    EFW_CMD_BOOTCHKSUM           7
        #define    EFW_CMD_SET_EEPROM           10
    UINT16    Size;
    UINT32    Address;
#if !CONTROLLER_16BIT && !CONTROLLER_32BIT
    UINT8       Data[BL_PAGE_SIZE];
#else
    UINT16      Data[BL_PAGE_SIZE>>1];
#endif
}MBX_STRUCT_PACKED_END
TEFWUPDATE;

#define FW_UPDATE_SIZE  SIZEOF(TEFWUPDATE)



/** \brief FoE header*/
typedef struct  MBX_STRUCT_PACKED_START
{
    UINT16        OpCode; /**< \brief OpCode
                             *
                             * 1 : RRQ<br>
                             * 2 : WRQ<br>
                             * 3 : DATA<br>
                             * 4 : ACK<br>
                             * 5 : ERR<br>
                             * 6 : BUSY*/
    union MBX_STRUCT_PACKED_START
    {
        UINT32        Password; /**< \brief Password (used in Read request and Write request). 0 if unknown*/
        UINT32        PacketNo; /**< \brief Packet number (used in DATA and ACK datagram)*/
        UINT32        ErrorCode; /**< \brief Error code (used in ERR datagram)*/
        struct MBX_STRUCT_PACKED_START
        {
            UINT16    Done; /**< \brief Done indication (used in BUSY datagram)*/
            UINT16    Entire;  /**< \brief Entire indication (used in BUSY datagram)*/
        }MBX_STRUCT_PACKED_END
        Busy; /**< \brief Busy variable*/
    }MBX_STRUCT_PACKED_END
    Cmd; /**< \brief Command field*/
}MBX_STRUCT_PACKED_END
TFOEHEADER;


#define     FOE_HEADER_SIZE     SIZEOF(TFOEHEADER) /**< \brief FoE header size*/

 /** \brief Complete FoE datagram*/
typedef struct MBX_STRUCT_PACKED_START
{
      TMBXHEADER        MbxHeader; /**< \brief Mailbox header*/
      TFOEHEADER        FoeHeader; /**< \brief FoE header*/
    UINT16              Data[((MAX_MBX_DATA_SIZE)-(FOE_HEADER_SIZE)) >> 1]; /**< \brief FoE Data buffer*/
}MBX_STRUCT_PACKED_END
TFOEMBX;


/*CODE_INSERT_START (MBX_FILE_PACKED_END)*/
#if FC1100_HW
#pragma pack(pop)
#endif
/*CODE_INSERT_END*/


/*---------------------------------------------
-    Results
-----------------------------------------------*/
#define    FOE_ERROR                            0x8000 /**< \brief Error result*/
#define    FOE_FINISHED                         ((FOE_ERROR)-1) /**< \brief Datagram finished*/
#define    FOE_FINISHED_NOACK                   ((FOE_FINISHED)-1) /**< \brief Datagram finished no acknowledgement*/
#define    FOE_ACK                              ((FOE_FINISHED_NOACK)-1) /**< \brief Acknowledgement*/
#define    FOE_ACKFINISHED                      ((FOE_ACK)-1) /**< \brief Acknowledgement finished*/
#define    FOE_WAIT                             ((FOE_ACKFINISHED)-1) /**< \brief Wait */
#define    FOE_MAXBUSY                          ((FOE_WAIT)-1) /**< \brief Max busy indication*/
#define    FOE_MAXBUSY_ZERO                     ((FOE_MAXBUSY)-100) /**< \brief Min busy indication*/
#define    FOE_MAXDATA                          ((FOE_MAXBUSY_ZERO)-1) /**< \brief Maximum data within one FoE Datagram*/

#endif //_ECATFOE_H_

#if _ECATFOE_
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/
PROTO    UINT32                                 u32PacketNo; /**< \brief Stores the actual packet number to be expected*/
PROTO    UINT32                                 u32FileOffset;  /**< \brief Stores the next file offset to be sent */
PROTO    UINT32                                 u32LastFileOffset; /**< \brief Stores the actual sent file offset */
PROTO    UINT16                                 u16FileAccessState; /**< \brief Stores the actual state of the file transmission sequence */
PROTO    TMBX MBXMEM *                          pFoeSendStored; /**<If the mailbox service could not be sent (or stored),
                                                                                the FoE service will be stored in this variable
                                                                                and will be sent automatically from the mailbox handler
                                                                                (FOE_ContinueInd) when the send mailbox will be read
                                                                                the next time from the master */
/*-----------------------------------------------------------------------------------------
------
------   Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO    void     FOE_Init(void);
PROTO    UINT8 FOE_ServiceInd(TFOEMBX MBXMEM * pFoeInd);
PROTO    void     FOE_ContinueInd(TMBX MBXMEM * pMbx);

#undef PROTO
/** @}*/
