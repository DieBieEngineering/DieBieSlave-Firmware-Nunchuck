/**
 * \addtogroup EL9800Appl EL9800 application
 * @{
 * This application includes three functional modules:<br>
 * Digital Input<br>
 * Digital Outputs<br>
 * Analog Input<br>
 * The objects of the modules are defined according the ETG.5001.<br>
 */

/**
\file el9800appl.h
\author EthercatSSC@beckhoff.com
\brief EL9800 Application defines, typedefs and global variables

\version 5.01

<br>Changes to version - :<br>
V5.01 : Start file change log
 */
#ifndef _EL9800APPL_H_
#define _EL9800APPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecatappl.h"


/*-----------------------------------------------------------------------------------------
------
------    Defines and Typedef
------
-----------------------------------------------------------------------------------------*/

/**
 * \addtogroup PdoMappingObjects PDO Mapping Objects
 *
 * Digital Input PDO mapping : 0x1A00<br>
 * Digital Output PDO mapping : 0x1601<br>
 * Analog Input PDO mapping : 0x1A02
 * @{
 */
/** \brief 0x1601 (Digital output RxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
#if _PIC24
   UINT32   aEntries[9]; /**< \brief Entry buffer*/
#else
   UINT32   aEntries[5]; /**< \brief Entry buffer*/
#endif
} OBJ_STRUCT_PACKED_END
TOBJ1601;


/** \brief 0x1A00 (Digital input TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
#if _PIC24
   UINT32   aEntries[9]; /**< \brief Entry buffer*/
#else
    UINT32   aEntries[5]; /**< \brief Entry buffer*/
#endif
} OBJ_STRUCT_PACKED_END
TOBJ1A00;


/** \brief 0x1A02 (Analog input TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[8]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1A02;
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
   UINT16   aEntries[1]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1C12;


/** \brief 0x1C13 (SyncManager 3 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT16   aEntries[2]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
/** @}*/


/**
 * \addtogroup PDO Process Data Objects
 * Digital Inputs : 0x6000<br>
 * Analog Inputs : 0x6010<br> 
 * Digital Outputs : 0x7020
 * @{
 */
/** \brief 0x6000 (Digital input object) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   BOOLEAN(bSwitch1); /**< \brief Switch 1*/
   BOOLEAN(bSwitch2); /**< \brief Switch 2*/
   BOOLEAN(bSwitch3); /**< \brief Switch 3*/
   BOOLEAN(bSwitch4); /**< \brief Switch 4*/
#if _PIC24
   BOOLEAN(bSwitch5); /**< \brief Switch 5*/
   BOOLEAN(bSwitch6); /**< \brief Switch 6*/
   BOOLEAN(bSwitch7); /**< \brief Switch 7*/
   BOOLEAN(bSwitch8); /**< \brief Switch 8*/
   ALIGN8(SubIndex009) /**< \brief 8Bit alignment*/
#else
   ALIGN12(SubIndex005) /**< \brief 12Bit alignment*/
#endif
} OBJ_STRUCT_PACKED_END
TOBJ6000;


/** \brief 0x6020 (Analog input object) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   BOOLEAN(bUnderrange); /**< \brief (SI1) Analog input under range*/
   BOOLEAN(bOverrange); /**< \brief (SI2) Analog input over range*/
   BIT2(b2Limit1); /**< \brief (SI3) Analog input 1st limit*/
   BIT2(b2Limit2); /**< \brief (SI5) Analog input 2nd limit*/
   ALIGN2(SubIndex006) /**< \brief 2Bit alignment*/
   ALIGN6(SubIndex007) /**< \brief 2Bit alignment*/
   BOOLEAN(bTxPDOState); /**< \brief (SI15) TxPdo state*/
   BOOLEAN(bTxPDOToggle); /**< \brief (SI16) TxPdo toggle*/
   INT16   i16Analoginput; /**< \brief (SI17) Analog input value*/
} OBJ_STRUCT_PACKED_END
TOBJ6020;


/** \brief 0x7010 (Digital output object) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   BOOLEAN(bLED1); /**< \brief LED 1*/
   BOOLEAN(bLED2); /**< \brief LED 2*/
   BOOLEAN(bLED3); /**< \brief LED 3*/
   BOOLEAN(bLED4); /**< \brief LED 4*/
#if _PIC24
   BOOLEAN(bLED5); /**< \brief LED 5*/
   BOOLEAN(bLED6); /**< \brief LED 6*/
   BOOLEAN(bLED7); /**< \brief LED 7*/
   BOOLEAN(bLED8); /**< \brief LED 8*/
   ALIGN8(SubIndex008) /**< \brief 8Bit alignment*/
#else
   ALIGN12(SubIndex005) /**< \brief 12Bit alignment*/
#endif
} OBJ_STRUCT_PACKED_END
TOBJ7010;
/** @}*/


/**
 * \addtogroup ConfigObjects Configuration Objects
 * Analog input settings: 0x8020
 * @{
 */
/** \brief 0x8020 (Analog input settings object) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   BOOLEAN(bEnableuserscale); /**< \brief Enable user scaling*/
   BIT3(b3Presentation); /**< \brief Value presentation*/
   ALIGN2(SubIndex005) /**< \brief 2Bit Alignment*/
   BOOLEAN(bEnablelimit1); /**< \brief Enable limit 1*/
   BOOLEAN(bEnablelimit2); /**< \brief Enable limit 2*/
   ALIGN8(SubIndex009) /**< \brief 8Bit Alignment*/
   INT16   i16Offset; /**< \brief Value offset*/
   INT32   i32Gain; /**< \brief Value gain*/
   INT16   i16Limit1; /**< \brief Value limit 1*/
   INT16   i16Limit2; /**< \brief Value limit 2*/
} OBJ_STRUCT_PACKED_END
TOBJ8020;
/** @}*/


/**
 * \addtogroup DeviceParaObjects Device Parameter Objects
 * Modular Device Profile: 0xF000
 * @{
 */
/** \brief 0xF000 (Modular Device Profile) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex0*/
   UINT16   u16Moduleindexdistance; /**< \brief Module Index distance
                                     * 
                                     * Index distance between two modules (maximum number of objects per module and area)<br>
                                     * Default: 0x10*/
   UINT16   u16Maximumnumberofmodules; /**< \brief Maximum number of modules*/
} OBJ_STRUCT_PACKED_END
TOBJF000;


/** \brief 0xF010 (Module Profile List) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex0*/
   UINT32   aEntries[3]; /**< \brief Module profile information buffer
                          * 
                          * Bit 0..15: Profile number of the module on position 1<br>
                          * Bit 16..31: Profile specific*/
} OBJ_STRUCT_PACKED_END
TOBJF010;
/** @}*/

#endif //_EL9800APPL_H_

#ifdef _EVALBOARD_
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
/** @}*/

   
/**
 * \addtogroup EnumObjects Enum Objects
 * @{
 * Presentation (Signed/Unsigned) : 0x800
 */

/*---------------------------------------------
-    0x0800: ENUM (Signed/Unsigned Presentation)
-----------------------------------------------*/
CHAR sEnum0800_Value00[] = "\000\000\000\000Signed presentation"; /**< \brief Value = 0x00, Text = Signed presentation */
CHAR sEnum0800_Value01[] = "\001\000\000\000Unsigned presentation"; /**< \brief Value = 0x01, Text = Unsigned presentation */
CHAR *apEnum0800[2] = { sEnum0800_Value00, sEnum0800_Value01};/**< \brief List of Enum (0x800) values*/

/**
 * \brief Enum entry description
 *
 * SubIndex0<br>
 * Enum (Signed Presentation)
 * enum (Unsigned Presentation)
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x0800[] =
   {{DEFTYPE_UNSIGNED8, 8, ACCESS_READ | OBJACCESS_NOPDOMAPPING},
    {DEFTYPE_OCTETSTRING, 8*SIZEOF(sEnum0800_Value00), ACCESS_READ | OBJACCESS_NOPDOMAPPING},
   {DEFTYPE_OCTETSTRING, 8*SIZEOF(sEnum0800_Value01), ACCESS_READ | OBJACCESS_NOPDOMAPPING}};
/** @}*/


/**
* \addtogroup PdoMappingObjects PDO Mapping Objects
* @{
*/
/**
 * \brief Object 0x1601 (Digital output RxPDO) entry descriptions
 * 
 * SubIndex 0 : read only<br>
 * SubIndex x : read only<br>
 *  (x > 0)
 */
OBJCONST TSDOINFOENTRYDESC	OBJMEM asEntryDesc0x1601[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
#if _PIC24
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
#endif
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

/**
 * \brief Object 0x1601 (Digital output RxPDO) name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1601[] = "DO RxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1601 (Digital output RxPDO) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x7010.1 1bit (Reference to LED1)<br>
 * SubIndex 2 : 0x7010.1 1bit (Reference to LED2)<br>
 * SubIndex 3 : 0x7010.1 1bit (Reference to LED3)<br>
 * SubIndex 4 : 0x7010.1 1bit (Reference to LED4)<br>
 * SubIndex 5 : 0x7010.1 1bit (Reference to LED5), only for PIC24<br>
 * SubIndex 6 : 0x7010.1 1bit (Reference to LED6), only for PIC24<br>
 * SubIndex 7 : 0x7010.1 1bit (Reference to LED7), only for PIC24<br>
 * SubIndex 8 : 0x7010.1 1bit (Reference to LED8), only for PIC24
 */
PROTO TOBJ1601 sDORxPDOMap
#ifdef _EVALBOARD_
#if _PIC24
 = {9, {0x70100101, 0x70100201, 0x70100301, 0x70100401,0x70100501, 0x70100601, 0x70100701, 0x70100801, 0x08}}
#else
 = {5, {0x70100101, 0x70100201, 0x70100301, 0x70100401, 0x0C}}
#endif
#endif
;
/** @}*/


/**
 * \addtogroup PdoParameter PDO Parameter
 * @{
 *
 * Parameter for PDO mapping object 0x1A02 : 0x1802
 */
#ifdef _OBJD_
/**
 * \brief Entry descriptions of TxPDO Parameter object (0x1802)
 * 
 * Subindex 0<br>
 * SubIndex 1 - 5 : not defined<br>
 * SubIndex 6 : Exclude TxPDOs<br>
 * SubIndex 7 : TxPDO State<br>
 * SubIndex 8 : not defined<br>
 * SubIndex 9 : TxPDO Toggle
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1802[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {0, 0x0, 0},
   {0, 0x0, 0},
   {0, 0x0, 0},
   {0, 0x0, 0},
   {0, 0x0, 0},
   {DEFTYPE_OCTETSTRING, 0x00, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {0, 0x0, 0},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}};

/**
 * \brief Object 0x1802 (TxPDO Parameter) object and entry names
 */
OBJCONST UCHAR OBJMEM aName0x1802[] = "TxPDO Parameter\000\000\000\000\000\000Exclude TxPDOs\000TxPDOState\000\000TxPDO Toggle\000\377";
#endif //#ifdef _OBJD_



/**
 * \brief Object 0x1802 (TxPDO Parameter) variable to handle object data
 * 
 * Only Subindex0 for this Object is stored here (all values are stored in other structures, see "ReadObject0x1802" for more details)
 */
PROTO UINT16 TxPDO1802Subindex0
#ifdef _EVALBOARD_
    = 9
#endif
    ;
/** @}*/


/**
 * \addtogroup PdoMapping PDO Mapping Objects
 * @{
 */
/**
 * \brief Object 0x1A00 (Digital input TxPDO) entry descriptions
 *
 * SubIndex 0 : read only<br>
 * SubIndex x : read only<br>
 *  (x > 0)
*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC	OBJMEM asEntryDesc0x1A00[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
#if _PIC24
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
#endif
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};


/**
 * \brief Object 0x1A00 (Digital input TxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1A00[] = "DI TxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1A00 (Digital Input TxPDO) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x6000.1 1bit (Reference to SWITCH1)<br>
 * SubIndex 2 : 0x6000.1 1bit (Reference to SWITCH2)<br>
 * SubIndex 3 : 0x6000.1 1bit (Reference to SWITCH3)<br>
 * SubIndex 4 : 0x6000.1 1bit (Reference to SWITCH4)<br>
 * SubIndex 5 : 0x6000.1 1bit (Reference to SWITCH5), only for PIC24<br>
 * SubIndex 6 : 0x6000.1 1bit (Reference to SWITCH6), only for PIC24<br>
 * SubIndex 7 : 0x6000.1 1bit (Reference to SWITCH7), only for PIC24<br>
 * SubIndex 8 : 0x6000.1 1bit (Reference to SWITCH8), only for PIC24
 */
PROTO TOBJ1A00 sDITxPDOMap
#ifdef _EVALBOARD_
#if _PIC24
 = {9, {0x60000101, 0x60000201, 0x60000301, 0x60000401,0x60000501, 0x60000601, 0x60000701, 0x60000801, 0x08}}
#else
 = {5, {0x60000101, 0x60000201, 0x60000301, 0x60000401, 0x0C}}
#endif
#endif
;


/**
 * \brief Object 0x1A02 (Analog input TxPDO) entry descriptions
 *
 * SubIndex 0 : read only<br>
 * SubIndex x : read only<br>
 *  (x > 0)
*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC	OBJMEM asEntryDesc0x1A02[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ }, /* Subindex 000 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 001: SubIndex 001 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 002: SubIndex 002 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 003: SubIndex 003 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 004: SubIndex 004 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 005: SubIndex 005 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 006: SubIndex 006 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}, /* SubIndex 007: SubIndex 007 */
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}}; /* SubIndex 008: SubIndex 008 */


/**
 * \brief Object 0x1A02 (Analog input TxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1A02[] = "AI TxPDO-Map\000\377";
#endif //#ifdef _OBJD_



/**
 * \brief Object 0x1A02 (Analog Input TxPDO) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x6020.1 1bit (Reference to Analog under range)<br>
 * SubIndex 2 : 0x6020.2 1bit (Reference to Analog over range)<br>
 * SubIndex 3 : 0x6020.3 2bit (Reference to limit 1)<br>
 * SubIndex 4 : 0x6020.5 2bit (Reference to limit 2)<br>
 * SubIndex 5 : 8Bit Alignment<br>
 * SubIndex 6 : 0x1802.7 1bit (Reference to TxPDO State (stored in analog input struct))<br>
 * SubIndex 7 : 0x1802.9 1bit (Reference to TxPDO Toggle (stored in analog input struct))<br>
 * SubIndex 8 : 0x6020.17 16bit (Reference to Analog input value)
 */
PROTO TOBJ1A02 sAITxPDOMap
#ifdef _EVALBOARD_
= {8, {0x60200101, 0x60200201, 0x60200302, 0x60200502, 0x08, 0x18020701, 0x18020901, 0x60201110}}
#endif
;
/** @}*/


/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */
#ifdef _OBJD_
/**
 * \brief 0x1C12 (SyncManager 2 assignment) object name 
 * 
 * No entry names defined because the object code is ARRAY and all entry names are "SubIndex 000"
 */
OBJCONST UCHAR OBJMEM aName0x1C12[] = "RxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief 0x1C12 (SyncManager 2 assignment) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x1601
 */
PROTO TOBJ1C12 sRxPDOassign
#ifdef _EVALBOARD_
= {0x01, {0x1601}}
#endif
;


#ifdef _OBJD_
/**
 * \brief 0x1C13 (SyncManager 3 assignment) object name 
 * 
 * No entry names defined because the object code is ARRAY and all entry names are "SubIndex 000"
 */
OBJCONST UCHAR OBJMEM aName0x1C13[] = "TxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief 0x1C13 (SyncManager 3 assignment) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x1A00<br>
 * SubIndex 2 : 0x1A02
 */
PROTO TOBJ1C13 sTxPDOassign
#ifdef _EVALBOARD_
= {0x02, {0x1A00, 0x1A02}}
#endif
;
/** @}*/


/**
 * \addtogroup PDO Process Data Objects
 * @{
 */
#ifdef _OBJD_
/**
 * \brief Object 0x6000 (Digital input object) entry descriptions
 *
 * SubIndex 0 : read only<br>
 * SubIndex x : (One description for each switch) read only and TxPdo mappable<br>
 *  (x > 0)
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ }, /* Subindex 000 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 001: Switch 1 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 002: Switch 2 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 003: Switch 3 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 004: Switch 4 */
#if _PIC24
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 005: Switch 5 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 006: Switch 6 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 007: Switch 7 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING}, /* SubIndex 008: Switch 8 */
   {0x0000, 0x08, 0}}; /* Subindex 009 for align */
#else
   {0x0000, 0x0C, 0}}; /* Subindex 005 for align */
#endif

/**
 * \brief 0x6000 (Digital input object) object and entry names
 */
#if _PIC24
OBJCONST UCHAR OBJMEM aName0x6000[] = "DI Inputs\000Switch 1\000Switch 2\000Switch 3\000Switch 4\000Switch 5\000Switch 6\000Switch 7\000Switch 8\000\000\377";
#else
OBJCONST UCHAR OBJMEM aName0x6000[] = "DI Inputs\000Switch 1\000Switch 2\000Switch 3\000Switch 4\000\000\377";
#endif
#endif //#ifdef _OBJD_


/**
 * \brief 0x6000 (Digital input object) variable to handle object data
 * 
 * SubIndex 0 : 1<br>
 * SubIndex x : every switch value is 0 by default
 */
PROTO TOBJ6000 sDIInputs
#ifdef _EVALBOARD_
#if _PIC24
= {8, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0}
#else
 = {4, 0x00, 0x00, 0x00, 0x00, 0, 0}
#endif
#endif
;


#ifdef _OBJD_
/**
 * \brief Object 0x6020 (Analog input object) entry descriptions
 *
 * SubIndex 0 : read only<br>
 * SubIndex 001: Underrange<br>
 * SubIndex 002: Overrange<br>
 * SubIndex 003: Limit 1<br>
 * Subindex 004: doesn't exist<br>
 * SubIndex 005: Limit 2<br>
 * Subindex 006: 2Bit Alignment<br>
 * Subindex 007: 6Bit Alignment<br>
 * Subindex 008: doesn't exist<br>
 * Subindex 009: doesn't exist<br>
 * Subindex 010: doesn't exist<br>
 * Subindex 011: doesn't exist<br>
 * Subindex 012: doesn't exist<br>
 * Subindex 013: doesn't exist<br>
 * Subindex 014: doesn't exist<br>
 * SubIndex 015: TxPDO State<br>
 * SubIndex 016: TxPDO Toggle<br>
 * SubIndex 017: Analog input
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6020[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {DEFTYPE_BIT2, 0x02, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {0x0000, 0, 0},
   {DEFTYPE_BIT2, 0x02, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {0x0000, 0x02, 0},
   {0x0000, 0x06, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_TXPDOMAPPING},
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READ | OBJACCESS_TXPDOMAPPING}};


/**
 * \brief 0x6020 (Analog input object) object and entry names
 */
OBJCONST UCHAR OBJMEM aName0x6020[] = "AI Inputs\000Underrange\000Overrange\000Limit 1\000\000Limit 2\000\000\000\000\000\000\000\000\000\000TxPDO State\000TxPDO Toggle\000Analog input\000\377";
#endif //#ifdef _OBJD_



/**
 * \brief 0x6020 (Analog input object) variable to handle object data
 * 
 */
PROTO TOBJ6020 sAIInputs
#ifdef _EVALBOARD_
= {17, 0x00, 0x00, 0x00, 0x00, 0, 0, 0x00, 0x00, 0x7FFF}
#endif
;



/**
 * \brief Object 0x7010 (Digital output object) entry descriptions
 *
 * SubIndex 0 : read only<br>
 * SubIndex x : (One description for each led) read only and RxPdo mappable<br>
 *  (x > 0)
*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7010[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ }, /* Subindex 000 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 001: LED 1 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 002: LED 2 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 003: LED 3 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 004: LED 4 */
#if _PIC24
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 005: LED 5 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 006: LED 6 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 007: LED 7 */
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ | OBJACCESS_RXPDOMAPPING}, /* SubIndex 008: LED 8 */
   {0x0000, 0x08, 0}}; /* Subindex 008 for align */
#else
   {0x0000, 0x0C, 0}}; /* Subindex 005 for align */
#endif


/**
 * \brief 0x6000 (Digital input object) object and entry names
 */
#if _PIC24
OBJCONST UCHAR OBJMEM aName0x7010[] = "DO Outputs\000LED 1\000LED 2\000LED 3\000LED 4\000LED 5\000LED 6\000LED 7\000LED 8\000\000\377";
#else
OBJCONST UCHAR OBJMEM aName0x7010[] = "DO Outputs\000LED 1\000LED 2\000LED 3\000LED 4\000\000\377";
#endif
#endif //#ifdef _OBJD_


/**
 * \brief 0x6020 (Analog input object) variable to handle object data
 * 
 */
PROTO TOBJ7010 sDOOutputs
#ifdef _EVALBOARD_
#if _PIC24
= {8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0}
#else
 = {4, 0x00, 0x00, 0x00, 0x00, 0, 0}
#endif
#endif
;
/** @}*/



/**
 * \addtogroup ConfigObjects Configuration Objects
 * @{
 */
/**
 * \brief Object 0x8020 (Analog input settings) entry descriptions
 *
 * SubIndex 0<br>
 * SubIndex 001: Enable user scale<br>
 * SubIndex 002: Presentation<br>
 * Subindex 003 doesn't exist<br>
 * Subindex 004 doesn't exist<br>
 * Subindex 005 for align<br>
 * Subindex 006 doesn't exist<br>
 * SubIndex 007: Enable limit 1<br>
 * SubIndex 008: Enable limit 2<br>
 * Subindex 009 for align<br>
 * Subindex 010 doesn't exist<br>
 * Subindex 011 doesn't exist<br>
 * Subindex 012 doesn't exist<br>
 * Subindex 013 doesn't exist<br>
 * Subindex 014 doesn't exist<br>
 * Subindex 015 doesn't exist<br>
 * Subindex 016 doesn't exist<br>
 * SubIndex 017: Offset<br>
 * SubIndex 018: Gain<br>
 * SubIndex 019: Limit 1<br>
 * SubIndex 020: Limit 2
 */
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x8020[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {0x0800, 0x03, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0x02, 0},
   {0x0000, 0, 0},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {0x0000, 0x08, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {0x0000, 0, 0},
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {DEFTYPE_INTEGER32, 0x20, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS},
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS}};

/** \brief Object 0x8020 (Analog input settings) object and entry names*/
OBJCONST UCHAR OBJMEM aName0x8020[] = "AI Settings\000Enable user scale\000Presentation\000\000\000\000\000Enable limit 1\000Enable limit 2\000\000\000\000\000\000\000\000\000Offset\000Gain\000Limit 1\000Limit 2\000\377";
#endif //#ifdef _OBJD_

/** \brief Object 0x8020 (Analog input settings) variable*/
PROTO TOBJ8020 sAISettings
#ifdef _EVALBOARD_
= {20, 0x00, 0x00, 0, 0x00, 0x00, 0, 0x00, 0x00, 0x00, 0x00}
#endif
;
/** @}*/


/**
 * \addtogroup DeviceParaObjects Device Parameter Objects
 * @{
 */
#ifdef _OBJD_
/** 
 * \brief 0xF000 (Modular Device Profile) entry descriptions
 *
 * Subindex 000<br>
 * SubIndex 001: Module index distance<br>
 * SubIndex 002: Maximum number of modules<br>
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF000[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ},
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}};


/** \brief 0xF000 (Modular Device Profile) object and entry names*/
OBJCONST UCHAR OBJMEM aName0xF000[] = "Modular device profile\000Module index distance\000Maximum number of modules\000\377";
#endif


/**
 * \brief 0xF000 (Modular Device Profile) variable to handle the object data
 *
 * SubIndex 0
 * SubIndex 1 (Module Index distance) : 0x10
 * SubIndex 2 (Maximum number of Modules) : 3
 */
/******************************************************************************
*                    Object 0xF000
******************************************************************************/
PROTO TOBJF000 sModulardeviceprofile
#ifdef _EVALBOARD_
= {2, 0x10, 0x03}
#endif
;


#ifdef _OBJD_
/** 
 * \brief 0xF010 (Module profile list) entry descriptions
 *
 * Subindex 0<br>
 * SubIndex x
 * (x > 0)
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xF010[] = {
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

/**
 * \brief 0xF010 (Module profile list) object name
 *
 * no entry names defined because the object is an array (for every entry > 0 "SubIndex xxx" is returned)
 */
OBJCONST UCHAR OBJMEM aName0xF010[] = "Module profile list";
#endif


/**
 *\brief 0xF010 (Module profile list) variable to handle object data
 * 
 * SubIndex 0 : 3
 * SubIndex 1 : 100 (Digital Input SubProfile)
 * SubIndex 2 : 200 (Digital Output SubProfile)
 * SubIndex 3 : 300 (Analog Input SubProfile)
 */
PROTO TOBJF010 sModulelist
#ifdef _EVALBOARD_
= {0x03,{0x64, 0xC8, 0x12C}}
#endif
;
/** @}*/


#ifdef _OBJD_
PROTO UINT8 ReadObject0x1802( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );

#if !STATIC_OBJECT_DIC
/*if _PIC18 is enabled the object dictionary is fixed defined in coeappl.c*/

/**
 *\brief EL9800 Application specific object dictionary
 * 
 */
TOBJECT    OBJMEM ApplicationObjDic[] = {
   /* Enum 0x0800 */
   {NULL,NULL, 0x0800, {DEFTYPE_ENUM, 0x02 | (OBJCODE_REC << 8)}, asEntryDesc0x0800, 0, apEnum0800 },
   /* Object 0x1601 */
   {NULL,NULL,  0x1601, {DEFTYPE_PDOMAPPING, 9 | (OBJCODE_REC << 8)}, asEntryDesc0x1601, aName0x1601, &sDORxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1802 */
   {NULL,NULL,  0x1802, {DEFTYPE_RECORD, 9 | (OBJCODE_REC << 8)}, asEntryDesc0x1802, aName0x1802,&TxPDO1802Subindex0, ReadObject0x1802, NULL, 0x0000 },
   /* Object 0x1A00 */
   {NULL,NULL,   0x1A00, {DEFTYPE_PDOMAPPING, 9 | (OBJCODE_REC << 8)}, asEntryDesc0x1A00, aName0x1A00, &sDITxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1A02 */
   {NULL,NULL,   0x1A02, {DEFTYPE_PDOMAPPING, 8 | (OBJCODE_REC << 8)}, asEntryDesc0x1A02, aName0x1A02, &sAITxPDOMap, NULL, NULL, 0x0000 },
    /* Object 0x1C12 */
   {NULL,NULL,   0x1C12, {DEFTYPE_UNSIGNED16, 1 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
   /* Object 0x1C13 */
   {NULL,NULL,   0x1C13, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
   /* Object 0x6000 */
   {NULL,NULL,   0x6000, {DEFTYPE_RECORD, 8 | (OBJCODE_REC << 8)}, asEntryDesc0x6000, aName0x6000, &sDIInputs, NULL, NULL, 0x0000 },
   /* Object 0x6020 */
   {NULL,NULL,   0x6020, {DEFTYPE_RECORD, 17 | (OBJCODE_REC << 8)}, asEntryDesc0x6020, aName0x6020, &sAIInputs, NULL, NULL, 0x0000 },
   /* Object 0x7010 */
   {NULL,NULL,   0x7010, {DEFTYPE_RECORD, 8 | (OBJCODE_REC << 8)}, asEntryDesc0x7010, aName0x7010, &sDOOutputs, NULL, NULL, 0x0000 },
    /* Object 0x8020 */
    {NULL,NULL,   0x8020, {DEFTYPE_RECORD, 20 | (OBJCODE_REC << 8)}, asEntryDesc0x8020, aName0x8020, &sAISettings, NULL, NULL, 0x0008 },
    /* Object 0xF000 */
   {NULL,NULL,   0xF000, {DEFTYPE_RECORD, 2 | (OBJCODE_REC << 8)}, asEntryDesc0xF000, aName0xF000, &sModulardeviceprofile, NULL, NULL, 0x0000 },
   /* Object 0xF010 */
   {NULL,NULL,   0xF010, {DEFTYPE_UNSIGNED32, 3 | (OBJCODE_ARR << 8)}, asEntryDesc0xF010, aName0xF010, &sModulelist, NULL, NULL, 0x0000 },
   {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif //#if !STATIC_OBJECT_DIC
#endif    //#ifdef _OBJD_

#if EXPLICIT_DEVICE_ID
PROTO UINT16 APPL_GetDeviceID(void);
#endif
PROTO void APPL_Application(void);

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
/** @}*/
