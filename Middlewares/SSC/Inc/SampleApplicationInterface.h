/**
 * \addtogroup SlaveLibrary EtherCAT Slave Library 
 * @{
 */

/**
\file SampleApplicationInterface.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 COE1: update invalid end entry in the object dictionaries (error with some compilers)<br>
<br>Changes to version V5.01:<br>
V5.10 ECAT9: Change slave library interface application<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */
#ifndef _SAMPLE_APPL_INTERFACE_H_
#define _SAMPLE_APPL_INTERFACE_H_

#define PDO_MAPPING_ENTIRES 22 /** < Maximum number of mapping entries. ((4kB DPRAM - 256 mailbox buffer) / 2 (input and output) / 3 SyncManager buffer mode = 640 / 30Byte (number of bytes per entry) =  22*/

/**
 * \addtogroup PdoMappingObjects PDO Mapping Objects
 * Input Counter PDO mapping : 0x1A00<br>
 * Ouput Counter PDO mapping : 0x1601
 * @{
 */

/** \brief 0x1601 (RxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[PDO_MAPPING_ENTIRES]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1601;


/** \brief 0x1A00 (TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[PDO_MAPPING_ENTIRES]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
/** @}*/


/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * SyncManager 2 : 0x1C12 
 * <br>SyncManager 3 : 0x1C13
 * @{
 */
/** \brief 0x1C12 (SyncManager 2 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT16   aEntries[1]; /**< \brief Entry Buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1C12;


/** \brief 0x1C13 (SyncManager 3 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT16   aEntries[1]; /**< \brief Entry Buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
/** @}*/

#endif //_SAMPLE_APPL_INTERFACE_H_

#ifdef _SAMPLE_APPLICATION_INTERFACE_
    #define PROTO
#else
    #define PROTO extern
#endif


#ifdef _OBJD_
/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */
/**
 * \brief Entry descriptions of SyncManager assign objects
 *
 * SubIndex0<br>
 * SubIndex1 (for all other entries the same description will be used (because the object code is ARRAY))
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asPDOAssignEntryDesc[] = {
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}};
#endif
/** @}*/


/**
* \addtogroup PdoMappingObjects PDO Mapping Objects
* @{
*/
/**
 * \brief Object 0x1601 (RxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1601[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};


/**
 * \brief Object 0x1601 (RxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */   
OBJCONST UCHAR OBJMEM aName0x1601[] = "RxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1601 (RxPDO) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x7010.0 32bit
 */
PROTO TOBJ1601 RxPDOMap
#ifdef _SAMPLE_APPLICATION_INTERFACE_
 = {1, {0x70100020}}
#endif
;


/**
 * \brief Object 0x1A00 (TxPDO) entry descriptions
 *
 * SubIndex 0<br>
 * SubIndex 1
*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};


/**
 * \brief Object 0x1A00 (TxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1A00[] = "TxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1A00 (TxPDO) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x6000.0 32bit
 */
PROTO TOBJ1A00 TxPDOMap
#ifdef _SAMPLE_APPLICATION_INTERFACE_
 = {1, {0x60000020}}
#endif
;
/** @}*/


/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */

/**
 * \brief 0x1C12 (SyncManager 2 assignment) object name 
 * 
 * No entry names defined because the object code is ARRAY and all entry names are "SubIndex 000"
 */
#ifdef _OBJD_
OBJCONST UCHAR OBJMEM aName0x1C12[] = "RxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief 0x1C12 (SyncManager 2 assignment) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x1601
 */
PROTO TOBJ1C12 sRxPDOassign
#ifdef _SAMPLE_APPLICATION_INTERFACE_
= {0x01, {0x1601}}
#endif
;


/**
 * \brief 0x1C13 (SyncManager 3 assignment) object name 
 * 
 * No entry names defined because the object code is ARRAY and all entry names are "SubIndex 000"
 */
#ifdef _OBJD_
OBJCONST UCHAR OBJMEM aName0x1C13[] = "TxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief 0x1C13 (SyncManager 3 assignment) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x1A00
 */
PROTO TOBJ1C13 sTxPDOassign
#ifdef _SAMPLE_APPLICATION_INTERFACE_
= {0x01, {0x1A00}}
#endif
;
/**@}*/


/**
 * \addtogroup ProcessDataObjects Process data objects
 * 
 * Including the following objects:<br>
 * 0x6000 : 32bit input counter (unsigned)<br>
 * 0x7010 : 32bit output counter (unsigned)
 * @{
 */

/**
 * \brief Object 0x6000 (input data) entry description
 *
 * This object is a variable so only one entry is defined
 */
#ifdef _OBJD_
TSDOINFOENTRYDESC    OBJMEM EntryDesc0x6000 = {DEFTYPE_OCTETSTRING, 0x20, ACCESS_READ | OBJACCESS_TXPDOMAPPING};


/** \brief Object 0x6000 (input data) name*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "Input Data";
#endif //#ifdef _OBJD_
/* memory pointer is stored in Slave Interface*/


/**
 * \brief Object 0x7010 (output data) entry description
 *
 * This object is a variable so only one entry is defined
 */
#ifdef _OBJD_
TSDOINFOENTRYDESC    OBJMEM EntryDesc0x7010 = {DEFTYPE_OCTETSTRING, 0x20, ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING};


/** \brief Object 0x7010 (output data) name*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "Output Data";
#endif //#ifdef _OBJD_
/* memory pointer is stored in Slave Interface*/


/** \brief Callback function to read the process data objects*/
PROTO UINT8 ReadProcessDataObject( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );

/** \brief Callback function to write the process data objects*/
PROTO UINT8 WriteProcessDataObject( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
/** @}*/


#ifdef _OBJD_
#if !STATIC_OBJECT_DIC
/**
 * \brief Object dictionary related to the Slave library application
 *
 * For every application related object an object description is added to the dictionary
 */
/*if _PIC18 is enabled the object dictionary is fixed defined in coeappl.c*/
TOBJECT    OBJMEM ApplicationObjDic[] = {
   /* Object 0x1601 */
   {NULL,NULL,  0x1601, {DEFTYPE_PDOMAPPING, PDO_MAPPING_ENTIRES | (OBJCODE_REC << 8)}, asEntryDesc0x1601, aName0x1601, &RxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1A00 */
   {NULL,NULL,   0x1A00, {DEFTYPE_PDOMAPPING, PDO_MAPPING_ENTIRES | (OBJCODE_REC << 8)}, asEntryDesc0x1A00, aName0x1A00, &TxPDOMap, NULL, NULL, 0x0000 },
    /* Object 0x1C12 */
   {NULL,NULL,   0x1C12, {DEFTYPE_UNSIGNED16, 1 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
     /* Object 0x1C13 */
    {NULL,NULL,   0x1C13, {DEFTYPE_UNSIGNED16, 1 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
   /* Object 0x6000 */
   {NULL,NULL,   0x6000, {DEFTYPE_OCTETSTRING, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x6000, aName0x6000, NULL,&ReadProcessDataObject, &WriteProcessDataObject, 0x0000 },
   /* Object 0x7010 */
   {NULL,NULL,   0x7010, {DEFTYPE_OCTETSTRING, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x7010, aName0x7010, NULL, &ReadProcessDataObject,&WriteProcessDataObject, 0x0000 },
/*ECATCHANGE_START(V5.11) COE1*/
   {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}};
/*ECATCHANGE_END(V5.11) COE1*/
#endif //#if !STATIC_OBJECT_DIC
#endif    //#ifdef _OBJD_


PROTO void APPL_Application(void);
#if EXPLICIT_DEVICE_ID
PROTO UINT16 APPL_GetDeviceID(void);
#endif

PROTO void   APPL_AckErrorInd(UINT16 stateTrans);
PROTO UINT16 APPL_StartMailboxHandler(void);
PROTO UINT16 APPL_StopMailboxHandler(void);
PROTO UINT16 APPL_StartInputHandler(UINT16 *pIntMask);
PROTO UINT16 APPL_StopInputHandler(void);
PROTO UINT16 APPL_StartOutputHandler(void);
PROTO UINT16 APPL_StopOutputHandler(void);

PROTO UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize);
PROTO void APPL_InputMapping(UINT16* pData);
PROTO void APPL_OutputMapping(UINT16* pData);

#undef PROTO
/**@}*/
