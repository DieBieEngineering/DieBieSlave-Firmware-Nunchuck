/**
 * \addtogroup Diagnosis Object
 * @{
 */

/**
\file diag.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 DIAG3: update message code handling<br>
V5.11 DIAG4: change parameter handling in DIAG_CreateNewMessage()<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */
#ifndef _DIAG_H_
#define _DIAG_H_


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "objdef.h"


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
#define DIAG_MSG_TYPE_MASK           0xF /**< \brief Diagnosis message type mask*/
#define DIAG_MSG_TYPE_INFO           0x0 /**< \brief Diagnosis message type info*/
#define DIAG_MSG_TYPE_WARNING        0x1 /**< \brief Diagnosis message type warning*/
#define DIAG_MSG_TYPE_ERROR          0x2 /**< \brief Diagnosis message type error*/

#define DIAG_MSG_PARAM_TYPE_MASK    0xF000 /**< \brief Diagnosis parameter type mask*/
#define DIAG_MSG_PARAM_TYPE_OFFSET  12 /**< \brief Diagnosis parameter type offset*/
#define DIAG_MSG_PARAM_TYPE_DATA    0x0 /**< \brief Diagnosis parameter type data*/
#define DIAG_MSG_PARAM_TYPE_B_ARRY  0x1 /**< \brief Diagnosis parameter type Byte Array*/
#define DIAG_MSG_PARAM_TYPE_ASCII   0x2 /**< \brief Diagnosis parameter type ASCII*/
#define DIAG_MSG_PARAM_TYPE_UNICODE 0x3 /**< \brief Diagnosis parameter type UNICODE*/


/*0x1F03 SI5 flag values/masks*/
#define DIAG_SEND_AS_EMCY           0x1 /**< \brief Diagnosis flag : Send as emergency*/
#define DIAG_DISABLE_INFO_MSG       0x2 /**< \brief Diagnosis flag : Disable Info messages*/
#define DIAG_DISABLE_WARNING_MSG    0x4 /**< \brief Diagnosis flag : Disable Warning messages*/
#define DIAG_DISABLE_ERROR_MSG      0x8 /**< \brief Diagnosis flag : Disable Error messages*/
#define DIAG_OPERATION_MODE         0x10 /**< \brief Indicates the diagnosis history mode (0 "overwrite" Mode; 1 "acknowledge" Mode)*/
#define DIAG_OVERWRITE_DISCARD      0x20 /**< \brief Indicates if messages were overwritten ("overwrite" mode) or new messages were discard ("acknowledge" mode)*/

#define DIAG_MSG_DEFAULT_LEN        16 /**< \brief Default Length: DiagCode + Flags + TextID + TimeStamp*/


#define DIAG_CREATE_PARAM_FLAG(type,info)   ((UINT32)((((UINT16)(type))<<(DIAG_MSG_PARAM_TYPE_OFFSET)) | ((UINT16)(info)))) /**< \brief Marco merge "type" and "info" in message header*/


/*Defined Diagnosis Codes*/
/*ECATCHANGE_START(V5.11) DIAG3*/
#define DIAG_CODE_EMCY(Code)        (((UINT32)(Code))<< 16 | (UINT16)(0xE800)) /**< \brief Marco to create emergency code*/
/*ECATCHANGE_END(V5.11) DIAG3*/
#define DIAG_CODE_APPL_SYNC         0xE001 /**< \brief Diagnosis code application sync*/
#define DIAG_APPL_INFO              0xE002 /**< \brief Diag Code for application information*/

/*Text IDs*/
#define DIAG_TEXT_ID_STATE_CHANGE_INFO           0x01 /**< \brief State change Request From: %d To:%d*/
#define DIAG_TEXT_ID_DEC_NUMBER                  0x02 /**< \brief %d*/
#define DIAG_TEXT_ID_CALC_BUS_CYCLE_FAILED       0x0F /**< \brief Calculate bus cycle time failed (Local timer too slow)*/
#define DIAG_TEXT_ID_STATE_SM_ADDR               0x11 /**< \brief Sync Manager %d invalid address (%d)*/
#define DIAG_TEXT_ID_STATE_SM_SIZE               0x12 /**< \brief Sync Manager %d invalid size (%d)*/
#define DIAG_TEXT_ID_STATE_SM_SETTING            0x13 /**< \brief Sync Manager %d invalid settings (%d)*/
#define DIAG_TEXT_ID_INVALID_REG_981             0x20 /**< \brief DC activation register is invalid*/
#define DIAG_TEXT_ID_INVALID_SYNC_TYPE           0x21 /**< \brief Configured SyncType (0x1C32.1 or 0x1C33.1) not supported. Check DC registers and supported SyncTypes (0x1C32.4 and 0x1C33.4)*/


/**
 * \brief This structure handles parameter information passed to diagnosis message create function 
 */
typedef struct {
   UINT16   ParamFlags; /**< \brief Parameter flags*/
/* ECATCHANGE_START(V5.11) DIAG4*/
   MEM_ADDR *pParam; /**< \brief pointer to the corresponding parameter memory*/
/* ECATCHANGE_END(V5.11) DIAG4*/
} TDIAGMSGPARAMINFO;


/**
 * \brief Basic structure to handle message parameter
 */
typedef struct {
   UINT16   ParamFlags; /**< \brief Parameter flags*/
   UINT8    pData[1]; /**< \brief Parameter buffer*/
} TDIAGMSGPARAM;


/**
 * \brief Generic diagnosis message structure (0x10F3.SI6 : 0x10F3.MaxSubindex
 */
typedef struct {
   UINT32   u32DiagCode; /**< \brief Message code*/
   UINT16   u16Flags; /**< \brief Message flags*/
   UINT16   u16TextID; /**< \brief Text ID*/
   UINT32   TimeStamp[2]; /**< \brief Timestamp*/
   TDIAGMSGPARAM Parameter; /**< \brief Handler of the first parameter*/
} TDIAGMESSAGE;


/**
 * \brief Structure is used to handle allocated message memory
 */
typedef struct {
   UINT16    u16MessageLength; /**< \brief Message length*/
   TDIAGMESSAGE *pMessage; /**< \brief Message buffer*/
} TDIAGMSGHANDLE;


/**
 *  \brief Object 0x10F3 (Diagnosis History) data structure
 *
 * messages are handle by aDiagMessages[]
 */
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex0*/
   UINT8    u8MaximumMessages; /**< \brief Maximum Number of messages*/
   UINT8    u8NewestMessage; /**< \brief Newest message*/
   UINT8    u8NewestAckMessage; /**< \brief Newest acknowledged message*/
   UINT8    u8NewMessage; /**< \brief New Message*/
   UINT16   Flags; /**< \brief Flags*/
} OBJ_STRUCT_PACKED_END
TOBJ10F3;


#endif //_DIAG_H_

#ifdef _DIAG_
    #define PROTO
#else
    #define PROTO extern
#endif


#ifdef _OBJD_
/**
 *  \brief Object 0x10F3 (Diagnosis History) entry descriptions
 *
 * Subindex 000<br>
 * SubIndex 001: Maximum Messages<br>
 * SubIndex 002: Newest Message<br>
 * SubIndex 003: Newest Acknowledged Message<br>
 * SubIndex 004: New Message Available<br>
 * SubIndex 005: Flags
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x10F3[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ},
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ},
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READWRITE},
   {DEFTYPE_BOOLEAN, 0x1, (ACCESS_READ | OBJACCESS_TXPDOMAPPING)},
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READWRITE}};


/**
 *  \brief Object 0x10F3 (Diagnosis History) object and entry names
 */
OBJCONST UCHAR OBJMEM aName0x10F3[] = "Diagnosis History\000Maximum Messages\000Newest Message\000Newest Acknowledged Message\000New Message Available\000Flags\000Diagnosis Message\000\377";
#endif


/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/

/**
 *  \brief Message entry description
 *
 * default message length is 0, the individual length of each diag message will be written in the function OBJ_GetEntryDesc
 */
PROTO TSDOINFOENTRYDESC OBJMEM MessageEntryDesc
#ifdef _DIAG_
 = {DEFTYPE_OCTETSTRING, 0x00, ACCESS_READ} /* Diagnosis Message Description*/
#endif
;


/**
 * \brief Global Array to handle diagnosis messages
 */
PROTO TDIAGMSGHANDLE aDiagMessages[MAX_DIAG_MSG];


/**
 *  \brief Object 0x10F3 (Diagnosis History) object variable
 *
 */
PROTO TOBJ10F3 sDiagHistory
#ifdef _DIAG_
    = {5, MAX_DIAG_MSG,0,0,0,0} //Subindex 0 will be increment if new diagnosis messages are added
#endif
;


/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO UINT8 Write0x10F3( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 Read0x10F3( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO void Diag_InitMemory(void);
PROTO void Diag_CreateNewMessage(UINT32 DiagCode,UINT8 Type,UINT16 TextID,UINT8 NumParam, TDIAGMSGPARAMINFO *pParam);

#undef PROTO
/** @}*/
