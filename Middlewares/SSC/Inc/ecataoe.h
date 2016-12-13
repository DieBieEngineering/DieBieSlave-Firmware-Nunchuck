/**
 * \addtogroup AoE ADS over EtherCAT
 * @{
 */

/**
\file ecataoe.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _ECATAOE_H_

#define _ECATAOE_H_


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
-    ADS Service Ids
-----------------------------------------------*/
#define     ADSSRVID_READ                   0x02 /**< \brief ADS read service*/
#define     ADSSRVID_WRITE                  0x03 /**< \brief ADS write service*/
#define     ADSSRVID_READWRITE              0x09 /**< \brief ADS read/write service*/
#define     ROUTERCMD_FRAGMENT              0x902 /**< \brief Fragmented ADS service*/

/*---------------------------------------------
-    ADS error codes
-----------------------------------------------*/
#define     ERR_ADSERRS                     0x700 /**< \brief ADS Device Error*/

#define     ERR_NOERROR                     0x00 /**< \brief No ADS Error*/
#define     ERR_TARGETPORTNOTFOUND          0x06 /**< \brief Target port not found*/
#define     ERR_TARGETMACHINENOTFOUND       0x07 /**< \brief Target machine not found*/

#define     ADSERR_DEVICE_ERROR             (0x00+(ERR_ADSERRS)) /**< \brief Error class < device error >*/
#define     ADSERR_DEVICE_SRVNOTSUPP        (0x01+(ERR_ADSERRS)) /**< \brief Service is not supported by server*/
#define     ADSERR_DEVICE_INVALIDGRP        (0x02+(ERR_ADSERRS)) /**< \brief Invalid indexGroup*/
#define     ADSERR_DEVICE_INVALIDOFFSET     (0x03+(ERR_ADSERRS)) /**< \brief Invalid indexOffset*/
#define     ADSERR_DEVICE_INVALIDACCESS     (0x04+(ERR_ADSERRS)) /**< \brief Reading/writing not permitted*/
#define     ADSERR_DEVICE_INVALIDSIZE       (0x05+(ERR_ADSERRS)) /**< \brief Parameter size not correct*/
#define     ADSERR_DEVICE_INVALIDDATA       (0x06+(ERR_ADSERRS)) /**< \brief Invalid parameter value(s)*/
#define     ADSERR_DEVICE_NOTREADY          (0x07+(ERR_ADSERRS)) /**< \brief Device is not in a ready state*/
#define     ADSERR_DEVICE_BUSY              (0x08+(ERR_ADSERRS)) /**< \brief Device is busy*/
#define     ADSERR_DEVICE_NOMEMORY          (0x0A+(ERR_ADSERRS)) /**< \brief Out of memory*/
#define     ADSERR_DEVICE_INVALIDPARM       (0x0B+(ERR_ADSERRS)) /**< \brief Invalid parameter value(s)*/
#define     ADSERR_DEVICE_NOTFOUND          (0x0C+(ERR_ADSERRS)) /**< \brief Not found (files, ...)*/
#define     ADSERR_DEVICE_SYNTAX            (0x0D+(ERR_ADSERRS)) /**< \brief Syntax error in comand or file*/
#define     ADSERR_DEVICE_INCOMPATIBLE      (0x0E+(ERR_ADSERRS)) /**< \brief Objects do not match*/
#define     ADSERR_DEVICE_EXISTS            (0x0F+(ERR_ADSERRS)) /**< \brief Object already exists*/
#define     ADSERR_DEVICE_INVALIDSTATE      (0x12+(ERR_ADSERRS)) /**< \brief Server is in invalid state*/
#define     ADSERR_DEVICE_NOTINIT           (0x18+(ERR_ADSERRS)) /**< \brief Device not initialized*/
#define     ADSERR_DEVICE_TIMEOUT           (0x19+(ERR_ADSERRS)) /**< \brief Device has a timeout*/


/*---------------------------------------------
-    ADS index groups
-----------------------------------------------*/
#define     ADSIGRP_CANOPEN_SDO              0xF302 /**< \brief SDO access*/
                                                    /**< 
                                                     * Index = HIWORD(iOffs)<br>
                                                     * subIndex = LOBYTE(LOWORD(iOffs))*/
#define     ADSIGRP_CANOPEN_SDO_INFO_LIST    0xF3FC /**< \brief ListType = HIWORD(iOffs)*/
#define     ADSIGRP_CANOPEN_SDO_INFO_OBJ     0xF3FD /**< \brief index = HIWORD(iOffs)*/
#define     ADSIGRP_CANOPEN_SDO_INFO_ENTRY   0xF3FE /**< \brief index = HIWORD(iOffs), subIndex = LOBYTE(LOWORD(iOffs))*/


/*---------------------------------------------
-    ADS index offsets
-----------------------------------------------*/
#define     ADSIOFFS_ECAT_COE_COMPLETEACCESS    0x00000100 /**< \brief SDO complete access mask(Group 0xF302)*/


/*---------------------------------------------
-    ADS replacements
-----------------------------------------------*/
#ifndef    ALLOCADSMEM
    #define    ALLOCADSMEM                    ALLOCMEM /**< \brief Macro to allocate ADS memory*/
#endif

#ifndef    FREEADSMEM
    #define    FREEADSMEM                     FREEMEM /**< \brief Macro to free ADS memory*/
#endif


/*---------------------------------------------
-    State flags
-----------------------------------------------*/
#define    AMSCMDSF_RESPONSE                  0x0001
#define    AMSCMDSF_NORETURN                  0x0002
#define    AMSCMDSF_ADSCMD                    0x0004


/*CODE_INSERT_START (MBX_FILE_PACKED_START)*/
#if FC1100_HW
#pragma pack(push, 1)
#endif
/*CODE_INSERT_END*/

/** \brief AMS Net ID*/
typedef struct MBX_STRUCT_PACKED_START
{
    UINT8            b[6]; /*"< \brief buffer to store AMS Net ID*/
} MBX_STRUCT_PACKED_END
AmsNetId;


/** \brief AMS Address*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsNetId    netId; /**< \brief AMS Net ID*/
    UINT16      port; /**< \brief AMS Port*/
}MBX_STRUCT_PACKED_END 
AmsAddr;


/** \brief AMS datagram header*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsAddr     target; /**< \brief Target AMS address*/
    AmsAddr     sender; /**< \brief Sender AMS address*/
    UINT16      cmdId; /**< \brief Command id*/
    UINT16      stateFlags; /**< \brief State flags*/
    UINT32      cbData; /**< \brief Count UINT8s for actual command (excl. AmsHead)*/
    long        errCode; /**< \brief Error code*/
    union
    {
        long    invokeId; /**< \brief InvokeId*/
        struct
        {
            UINT16    fragmentNo; /**< \brief Increasing no*/
            UINT16    packetNo; /**< \brief Unique no of all fragments in a packet*/
        } frag; /**< \brief Fragment info*/
    } hUser; /**< \brief User handle*/
//    long            hUser;            //    user handle
} MBX_STRUCT_PACKED_END
    AmsHead;

/** \brief AMS First Fragment header*/
typedef struct MBX_STRUCT_PACKED_START
{
     /** \brief AMS addresses*/
    struct addr
    {
        AmsAddr        target; /**< \brief Target address*/
        AmsAddr        sender; /**< \brief Sender address*/
    } addr;

     /** \brief Fragment info*/
    struct frag
    {
        UINT16        cmdId; /**< \brief Command id*/
        UINT16        stateFlags; /**< \brief State flags*/
        UINT32        cbData; /**< \brief Count UINT8s for actual command (excl. AmsHead)*/
        UINT32        errCode; /**< \brief Error code*/
        UINT16        fragmentNo; /**< \brief Increasing no*/
        UINT16        packetNo; /**< \brief Unique no of all fragments in a packet*/
    } frag;

    /** \brief Org*/
    struct org
    {
        UINT16        cmdId; /**< \brief Command id*/
        UINT16        stateFlags; /**< \brief State flags*/
        UINT32        cbData; /**< \brief Count UINT8s for actual command (excl. AmsHead)*/
        UINT32        errCode; /**< \brief Error code*/
        UINT32        hUser; /**< \brief User handle*/
    } org;
} MBX_STRUCT_PACKED_END
    AmsFirstFragmentHead;


/** \brief AMS Command*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Head for the string command*/
} MBX_STRUCT_PACKED_END
    AmsCmd;

/** \brief ADS Read Request / Read Indication*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Datagram header*/
    UINT32    indexGroup; /**< \brief Index group*/
    UINT32    indexOffset; /**< \brief Index Offset*/
    UINT32    cbLength; /**< \brief Length*/
} MBX_STRUCT_PACKED_END
    TAdsReadReq, TAdsReadInd;

/** \brief ADS Read Response / Read Confirmation*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Datagram header*/
    UINT32    result; /**< \brief Result*/
    UINT32    cbLength; /**< \brief Length*/
} MBX_STRUCT_PACKED_END
    TAdsReadRes, TAdsReadCon;


#define    SIZEOF_TAdsReadRes    ((SIZEOF(TAdsReadRes))-(SIZEOF(AmsHead))) /**< \brief ADS Read Response Size*/


/** \brief ADS Write Request / Write Indication*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Datagram header*/
    UINT32    indexGroup; /**< \brief Index group*/
    UINT32    indexOffset; /**< \brief Index offset*/
    UINT32    cbLength; /**< \brief Length*/
} MBX_STRUCT_PACKED_END
    TAdsWriteReq, TAdsWriteInd;


/** \brief ADS Write Response / Write Confirmation*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Datagram header*/
    UINT32    result; /**< \brief Result*/
} MBX_STRUCT_PACKED_END
    TAdsWriteRes,TAdsWriteCon;


#define    SIZEOF_TAdsWriteRes    ((SIZEOF(TAdsWriteRes))-(SIZEOF(AmsHead))) /**< \brief ADS Write Response size*/


/** \brief ADS Read/Write Request / ReadWrite Indication*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Datagram header*/
    UINT32    indexGroup; /**< \brief Index group*/
    UINT32    indexOffset; /**< \brief Index offset*/
    UINT32    cbReadLength; /**< \brief Read length*/
    UINT32    cbWriteLength; /**< \brief Write length*/
} MBX_STRUCT_PACKED_END
    TAdsReadWriteReq, TAdsReadWriteInd;


/** \brief ADS Read/Write Response / Read/Write Confirmation*/
typedef struct MBX_STRUCT_PACKED_START
{
    AmsHead    head; /**< \brief Datagram header*/
    UINT32    result; /**< \brief Result*/
    UINT32    cbLength; /**< \brief Length*/
} MBX_STRUCT_PACKED_END
    TAdsReadWriteRes, TAdsReadWriteCon;


/*CODE_INSERT_START (MBX_FILE_PACKED_END)*/
#if FC1100_HW
#pragma pack(pop)
#endif
/*CODE_INSERT_END*/


#endif //_ECATCOE_H_

/* ECATCHANGE_START(V5.11) ECAT10*/
#if defined(_ECATAOE_) && (_ECATAOE_ == 1)
/* ECATCHANGE_END(V5.11) ECAT10*/
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/

PROTO    BOOL            bAoeWaitForNetId; /**< \brief Indicates if AMS Net ID was not set yet*/
PROTO    BOOL            bAoeRelativeNetIds; /**< \brief AMS Net ID was overwritten*/
PROTO    AmsNetId        sRouterNetId; /**< \brief AMS Net ID of the Router*/
PROTO    UINT16          u16AmsFragSize; /**< \brief AMS Fragment size*/


/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/
PROTO   void AOE_Init(void);
PROTO   AmsCmd MBXMEM * AOE_FragmentedCmdInd(AmsCmd MBXMEM *pCmd);
PROTO   UINT8 AOE_AmsNetIdsEqual(AmsNetId MBXMEM *pNetId1, AmsNetId MBXMEM *pNetId2);
PROTO   UINT16 AOE_AmsRes(AmsCmd MBXMEM *pCmd, UINT16 amsErrCode, UINT16 dataLen);
PROTO   UINT8 AOE_ServiceInd(TMBX MBXMEM * pMbx);

#undef PROTO
/** @}*/
