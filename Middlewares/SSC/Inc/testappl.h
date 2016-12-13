/**
 * \addtogroup TestAppl Tests Application
 * @{
 */

/**
\file testappl.h
\author EthercatSSC@beckhoff.com
\brief test application object definitions

\version 5.11

<br>Changes to version V5.10.1:<br>
V5.11 TEST1: add object 0x300B including bytes arrays with odd word length<br>
V5.11 TEST2: add test 0x300C object (entry bitlength > 65535)<br>
V5.11 TEST3: update enum test object (definition) to force enum values on an odd word/dword address<br>
V5.11 TEST4: add new mailbox test behaviour (the master mailbox cnt shall be incremented by 1 and the slave mailbox cnt is alternating)<br>
V5.11 TEST5: send a FoE busy on a FoE read request<br>
V5.11 TEST6: add test object to trigger Slave-to-Slave communication<br>
V5.11 TEST7: add test behaviour to send an emergency on every SDO request (in SafeOP)<br>
V5.11 TEST9: "add behaviour 0x2020.7 (SDO requests on 0x3006.0 are set to pending until an FoE read request on ""UnlockSdoResp"" is received or in case that no mbx queue is supported when a new mbx request was received)"<br>
<br>Changes to version V5.01:<br>
V5.10 TEST2: Add 0x3004.14 (Access to this entry will always rejected with the code "cannot be acceesd because of local control")<br>
V5.10 TEST3: Add object 0x3008 to test correct handling of BITARRxx base data types<br>
V5.10 TEST4: Add CoE test (0x2020.5) to return always max object data on complete access (not limited to value of SI0)<br>
V5.10 TEST5: test 0x2020.1 change limit from 10 to 16 Byte <br>
             Add test object 0x3009/0x300A (huge array and record objects)<br>
V5.10 TEST6: Add Bit3Array (0x3007) test object<br>
V5.10 TEST8: Add ESM variables Object 0xA000 and map to process data (0x1AFF)<br>
V5.10 TEST9: Add option to prevent SM3 unlock during PS<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */
#ifndef _TEST_APPL_H_
#define _TEST_APPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecatappl.h"

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
#if COE_SUPPORTED
#if EL9800_HW
#define TESTS_ENABLED          SWITCH_1 /**< \brief Test behaviour is enabled or disabled by switch 1*/
#define TEST_CONTROL_BITS      7/**< \brief Use switch 2 to 8 to control enable/disable tests */
#else
#define TESTS_ENABLED          1 /**< \brief Test behaviour is enabled*/
#define TEST_CONTROL_BITS      16 /**< \brief Use GPO register (0xF10) to enable/disable tests*/
#endif



/**
 * \addtogroup TestObjects Test Objects
 * 
 * 0x2000 to 0x2FFF
 * @{
 */
#define TEST_ENTRY_BIT_LENGTH   0x10 /**< \brief Bit length of a test object entry*/

#define IS_TEST_OBJECT(index)  ((index) >= 0x2000 && (index) <= 0x2FFF) /**< \brief Check if an index is within the test object area (0x2000 to 0x2FFF)*/

#define IS_TEST_ACTIVE(TestEntry)   ((((TestEntry).Control & 0x0001) == 0x0001) && (TESTS_ENABLED == 1))  /**< \brief Test is active if Bit0 is set and the global tests enable is set*/

#define INC_TEST_CNT(TestEntry)     {if((TestEntry).Counter < 0xFF)\
                                    {(TestEntry).Counter++;}} /**< \brief Increment the Test execution counter*/

#define ESC_TESTID_REGISTER     0x0F80 /**< \brief Test ID Register*/

#define DUMMY_OD_LENGTH     1000 /**< \brief Number of objects for the dummy object dictionary list handling*/


/**
 * \brief Control structure for a single test
 */
typedef struct OBJ_STRUCT_PACKED_START {
   OBJCONST CHAR OBJMEM *pName; /**< \brief Name of the test*/
   UINT16   Control; /**< \brief Control word of the test*/
   UINT16   Counter; /**< \brief Test execution counter*/
} OBJ_STRUCT_PACKED_END
TTESTENTRY;


/**
 * \brief Object including the list of all tests
 */
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex0*/
   TTESTENTRY  *paEntries[255]; /**< \brief Test list buffer*/
} OBJ_STRUCT_PACKED_END
TTESTOBJ;


/**
 * \brief Object 0x2FFF (Test control object) entry data structure
 */
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   TestIndex; /**< \brief Test index*/
   UINT8    TestSubindex; /**< \brief Test Subindex*/
   TTESTENTRY   *pTest; /**< \brief Pointer to test control structure*/
} OBJ_STRUCT_PACKED_END
TTESTCONTROLENTRY;


/**
 * \brief Object 0x2FFF (Test control object) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   TTESTCONTROLENTRY   aEntries[TEST_CONTROL_BITS]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ2FFF;
/** @}*/



/**
 * \addtogroup ApplObjects Application Objects
 * 
 * @{
 */

/**
 * \brief Object 0x3003 (ByteAndLessEntryObject) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START {
    UINT16   u16SubIndex0; /**< \brief SubIndex0*/
    BOOLEAN(Boolean); /**< \brief SubIndex1 : Boolean*/
    BIT1(Bit1); /**< \brief SubIndex2 : Bit1*/
    BIT2(Bit2); /**< \brief SubIndex3 : Bit2*/
    BIT3(Bit3); /**< \brief SubIndex4 : Bitt3*/
    ALIGN1(Align_1Bit_SI5) /**< \brief SubIndex5 : 1Bit Align*/
    BIT4(Bit4); /**< \brief SubIndex6 : Bit4*/
    ALIGN4(Align_4Bit_SI7) /**< \brief SubIndex7 : 4Bit Align*/
    BIT5(Bit5); /**< \brief SubIndex8 : Bit5*/
    ALIGN3(Align_3Bit_SI9) /**< \brief SubIndex9 : 3Bit Align*/
    BIT6(Bit6); /**< \brief SubIndex10 : Bit6*/
    ALIGN2(Align_2Bit_SI11) /**< \brief SubIndex11 : 2Bit Align*/
    BIT7(Bit7); /**< \brief SubIndex12 : Bit7*/
    ALIGN1(Align_1Bit_SI13) /**< \brief SubIndex13 : 1Bit ALign*/
    BIT8(Bit8); /**< \brief SubIndex14 : Bit8*/
    UINT8    Byte1; /**< \brief SubIndex15 : UINT8*/
    INT8     Byte2; /**< \brief SubIndex16 : INT8*/
} OBJ_STRUCT_PACKED_END
TOBJ3003;


/**
 * \brief Object 0x3004 (Aligned/Access Test Object) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START
{
    UINT16    subindex0;
    UINT16    u16_ro;                //SI 1
    UINT16    u16_rw;                //SI 2
    UINT16    u16_wo;                //SI 3
    BIT8(u8_rw);                     //SI 4
    ALIGN8(SubIndex005)              //SI 5
                                     //SI 6
    ALIGN8(SubIndex007)              //SI 7
    BIT8(u8_ro);                     //SI 8
    BIT2(u2_wo);                     //SI 9
    BIT2(u2_rw);                     //SI10
    ALIGN2(SubIndex011)              //SI11
    BIT2(u2_r0);                     //SI12
    BOOLEAN(Bool_rw);                //SI13
    BIT7(BlockEntry);                //SI14
}OBJ_STRUCT_PACKED_END             
TOBJ0x3004;


/**
 * \brief Object 0x3005 (Enum TestObject) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   BIT3(b3Presentation);
   BIT3(bBoolean);
   ALIGN10(Subindex003)
} OBJ_STRUCT_PACKED_END
TOBJ3005;


/**
 * \brief Object 0x3006 (Huge VAR object) buffer size data structure
 *
 * Set size to upload the object with one normal and two segmented transfers - 1Byte (to test unlock operation), 128 Bytes MBX size
 */
#define HUGE_OBJ_BYTE_SIZE 349

#if !_PIC24
/*ECATCHANGE_START(V5.11) TEST2*/
#define OBJ_300C_ENTRY_BITSIZE  65552
/*ECATCHANGE_END(V5.11) TEST2*/
#endif //#if !_PIC24

/**
 * \brief Object 0x3007 (Bit3Array) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START
{
    UINT16    subindex0;
    UINT16    Value; /**< \brief Buffer to store 5Entries*/
}OBJ_STRUCT_PACKED_END
TOBJ0x3007;


/**
 * \brief Object 0x3008 (Basedatatype Arrays Object) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START
{
    UINT16    subindex0;
    UINT32    UintArr;
    UINT32    ByteArr;
    UINT32    BitArr32;
    UINT16    BitArr16;
    UINT8     BitArr8;
}OBJ_STRUCT_PACKED_END
TOBJ0x3008;


/**
 * \brief Object 0x3009/0x300A: (Huge Array/Record Object) data structure
 */
typedef struct OBJ_STRUCT_PACKED_START
{
    UINT16    subindex0;
    UINT16    BitSize;
    UINT16    Value;
}OBJ_STRUCT_PACKED_END
TOBJ0x3009;
/** @}*/


/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */
/** \brief 0x1C12 (SyncManager 2 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT16   aEntries[2];
} OBJ_STRUCT_PACKED_END
TOBJ1C12;


/** \brief 0x1C13 (SyncManager 3 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT16   aEntries[2];
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
/** @}*/


/**
 * \addtogroup PdoMappingObjects PDO Mapping Objects
 * PDO mapping : 0x1A00<br>
 * PDO mapping : 0x1601<br>
 * PDO mapping : 0x1A02<br>
 * PDO mapping : 0x1603
 * @{
 */
/** \brief 0x1601 (RxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT32   aEntries[1];
} OBJ_STRUCT_PACKED_END
TOBJ1601;

/** \brief 0x1603 (RxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT32   aEntries[1];
} OBJ_STRUCT_PACKED_END
TOBJ1603;

/** \brief 0x1A00 (TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT32   aEntries[1];
} OBJ_STRUCT_PACKED_END
TOBJ1A00;


/** \brief 0x1A02 (TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT32   aEntries[1];
} OBJ_STRUCT_PACKED_END
TOBJ1A02;
/** @}*/

/** \brief 0x1AFF (TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0;
   UINT32   aEntries[28];
} OBJ_STRUCT_PACKED_END
TOBJ1AFF;
/** @}*/

/** \brief 0x8000 (Stack configuration object) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
    CHAR   *pName;
    BOOLEAN(bValue);
} OBJ_STRUCT_PACKED_END
TSLV_CONFIG;


#if !SDO_RES_INTERFACE
#define  WriteObject0x3006   NULL
#define  ReadObject0x3006    NULL
#define  WriteObject0x3002   NULL
#define  ReadObject0x3002    NULL
#endif

#endif //COE_SUPPORTED


#endif //_TEST_APPL_H_


#ifdef _TESTAPPL_
    #define PROTO
#else
    #define PROTO extern
#endif

#if COE_SUPPORTED
#ifdef _OBJD_
/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * SyncManager 2 : 0x1C12<br>
 * SyncManager 3 : 0x1C13
 * @{
 */
/** \brief SyncManager assignment entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asPDOAssignEntryDesc[] = {
   {DEFTYPE_UNSIGNED8, 0x08, (ACCESS_READ | ACCESS_WRITE_PREOP)},
   {DEFTYPE_UNSIGNED16, 0x10, (ACCESS_READ | ACCESS_WRITE_PREOP)}};
/** @}*/


/**
 * \addtogroup TestObjects Test Objects
 * @{
 */
/** \brief Test object entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asTestObjEntryDesc[] = {
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED16, 0x10, (ACCESS_READ | ACCESS_WRITE | OBJACCESS_SETTINGS)}};
#endif //#ifdef _OBJD_


/*---------------------------------------------
-    0x2000 - 0x2000F : ESM behaviour
-----------------------------------------------*/

/** \brief Test : abort IP transition with error code 0x16*/
PROTO TTESTENTRY ESM0_IP0x16 
#ifdef _TESTAPPL_
    ={"IP 0x0016 (invalid mailbox SyncManger)",0,0}
#endif
;

/** \brief Test : abort PS transition with error code 0x1E*/
PROTO TTESTENTRY ESM0_PS0x1E
#ifdef _TESTAPPL_
    ={"PS 0x001E (invalid process data INPUT SyncManger)",0,0}
#endif
;

/** \brief Test : abort PS transition with error code 0x1D*/
PROTO TTESTENTRY ESM0_PS0x1D
#ifdef _TESTAPPL_
    ={"PS 0x001D (invalid process data OUTPUT SyncManger)",0,0}
#endif
;

/** \brief Test : Don't unlock SM3 buffer during PS*/
PROTO TTESTENTRY ESM0_PS_NO_INPUT_UNLOCK
#ifdef _TESTAPPL_
    ={"Do not unlock the SM3 buffer during PS",0,0}
#endif
;

/** \brief 0x2000 (ESM Test object)*/
PROTO TTESTOBJ sESM0x2000
#ifdef _TESTAPPL_
= {0x04, {&ESM0_IP0x16,&ESM0_PS0x1D,&ESM0_PS0x1E,&ESM0_PS_NO_INPUT_UNLOCK}}
#endif
;

#ifdef _OBJD_
/** \brief 0x2000 (ESM Test object) object name*/
OBJCONST UCHAR OBJMEM aName0x2000[] = "ESM behaviour group 01\000\377";
#endif


/*---------------------------------------------
-    Objects 0x2010 - 0x2001F : Mailbox behaviour
-----------------------------------------------*/
/*ECATCHANGE_START(V5.11) TEST4*/
/** \brief Test : Check if the master mailbox counter sequence is incremented by one (otherwise return an error)*/
PROTO TTESTENTRY Mbx_MasterMbxCntIncByOne
#ifdef _TESTAPPL_
    ={"Master mailbox counter shall be incremented by one",0,0}
#endif
;

/** \brief Test : Alternating change of the mailbox counter*/
PROTO TTESTENTRY Mbx_AlternatingMbxCntChange
#ifdef _TESTAPPL_
    ={"Change the mailbox counter in alternating order",0,0}
#endif
;

/** \brief 0x2010 MBX Test object variable*/
PROTO TTESTOBJ sMbx0x2010
#ifdef _TESTAPPL_
= {0x02, {&Mbx_MasterMbxCntIncByOne,&Mbx_AlternatingMbxCntChange}}
#endif
;
#ifdef _OBJD_
/** \brief 0x2010 MBX Test object name*/
OBJCONST UCHAR OBJMEM aName0x2010[] = "Mailbox behaviour group 01\000\377";
#endif
/*ECATCHANGE_END(V5.11) TEST4*/

/*---------------------------------------------
-    Objects 0x2020 - 0x2002F : CoE behaviour
-----------------------------------------------*/
/** \brief Test : start SDO segmentation for mailbox data >= 16Byte (even if the mailbox buffer is larger)*/
PROTO TTESTENTRY CoE0_SegSdoByte
#ifdef _TESTAPPL_
    ={"Limit SDO upload size to 16Byte",0,0}
#endif
;

/** \brief Test : Create a diagnosis message on every state change*/
PROTO TTESTENTRY CoE0_ESM_Diag
#ifdef _TESTAPPL_
    ={"Create diagnosis message on state change",0,0}
#endif
;


/** \brief Test : Create a diagnosis message on every application cycle*/
PROTO TTESTENTRY CoE0_Diag_Test
#ifdef _TESTAPPL_
    ={"Create diag messages with each application cycle",0,0}
#endif
;

/** \brief Test : Create a dummy object dictionary with 1000 Entries*/
PROTO TTESTENTRY SDOInfo_DummyOD_Test
#ifdef _TESTAPPL_
    ={"Creates a dummy object dictionary with 1000 entries (DUMMY_OD_LENGTH)",0,0}
#endif
;


/** \brief Test : Return always the complete object buffer on complete access (even if SI is smaller)*/
PROTO TTESTENTRY SDOFullObjectCompleteAccess_Test
#ifdef _TESTAPPL_
    ={"SDO upload Complete Access returns always #Bytes based on MAX Subindex (ignore SI)",0,0}
#endif
;

/*ECATCHANGE_START(V5.11) TEST7*/
/** \brief Test : Send emergency on any SDO request*/
PROTO TTESTENTRY EmergencyOnSdoRequest_Test
#ifdef _TESTAPPL_
    ={"Send an emergency on any SDO request in SafeOP",0,0}
#endif
;
/*ECATCHANGE_END(V5.11) TEST7*/

/*ECATCHANGE_START(V5.11) TEST9*/
/** \brief Test : Lock SDO response on object 0x3006 (unlocked by FoE upload request)*/
PROTO TTESTENTRY DelaySdoResponse0x3006
#ifdef _TESTAPPL_
= { "Lock SDO response on object 0x3006 (unlocked by FoE upload request)", 0, 0 }
#endif
;

/** \brief 0x2020 CoE Test object variable*/
PROTO TTESTOBJ sCoE0x2020
#ifdef _TESTAPPL_
= { 0x07, { &CoE0_SegSdoByte, &CoE0_ESM_Diag, &CoE0_Diag_Test, &SDOInfo_DummyOD_Test, &SDOFullObjectCompleteAccess_Test, &EmergencyOnSdoRequest_Test, &DelaySdoResponse0x3006 } }
#endif
;
/*ECATCHANGE_END(V5.11) TEST9*/
#ifdef _OBJD_
/** \brief 0x2020 CoE Test object name*/
OBJCONST UCHAR OBJMEM aName0x2020[] = "CoE behaviour group 01\000\377";
#endif

/* ECATCHANGE_START(V5.11) TEST5*/
/*---------------------------------------------
-    Objects 0x2030 - 0x2003F : FoE behaviour
-----------------------------------------------*/
/** \brief Test : Return a Busy request on a read request */
PROTO TTESTENTRY FoE0_ReadBusy
#ifdef _TESTAPPL_
    ={"Return a Busy on a received read request",0,0}
#endif
;


/** \brief 0x2030 FoE Test object variable*/
PROTO TTESTOBJ sFoE0x2030
#ifdef _TESTAPPL_
= {0x01, {&FoE0_ReadBusy}}
#endif
;

#ifdef _OBJD_
/** \brief 0x2030 FoE Test object name*/
OBJCONST UCHAR OBJMEM aName0x2030[] = "FoE behaviour group 01\000\377";
#endif
/* ECATCHANGE_END(V5.11) TEST5*/

#ifdef _OBJD_
/** \brief 0x2FFF (Test control object) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x2FFF[] = {
   {DEFTYPE_UNSIGNED8, 0x08, (ACCESS_READ)},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE}};

   
/** \brief 0x2FFF (Test control object) object name*/
OBJCONST UCHAR OBJMEM aName0x2FFF[] = "TestControl";
#endif //#ifdef _OBJD_


/** \brief 0x2FFF (Test control object) variable to handle object data*/
PROTO TOBJ2FFF sTestControlObj;
/** @}*/



/**
 * \addtogroup ApplObjects Application Objects
 * 
 * @{
 */
#ifdef _OBJD_
CHAR sEnum0800_Value00[] = "\000\000\000\000Signed presentation"; /**< \brief Enum 0x800 value 0
                                                                   * 
                                                                   * Value = 0x00, Text = Signed presentation */
CHAR sEnum0800_Value01[] = "\001\000\000\000Unsigned presentation";/**< \brief Enum 0x800 value 1
                                                                   *
                                                                   *Value = 0x01, Text = Unsigned presentation */
/** \brief 0x800 (signed/unsigned) Enum value buffer*/
CHAR *apEnum0800[2] = { sEnum0800_Value00, sEnum0800_Value01};

/** \brief 0x800 (signed/unsigned) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x0800[] =
   {{DEFTYPE_UNSIGNED8, 8, ACCESS_READ | OBJACCESS_NOPDOMAPPING},
    {DEFTYPE_OCTETSTRING, 8*SIZEOF(sEnum0800_Value00), ACCESS_READ | OBJACCESS_NOPDOMAPPING},
   {DEFTYPE_OCTETSTRING, 8*SIZEOF(sEnum0800_Value01), ACCESS_READ | OBJACCESS_NOPDOMAPPING}};


/*ECATCHANGE_START(V5.11) TEST3*/
/* Use alternative enum definition to force the second enum on an odd word address */
CHAR sEnum0801_Value[] = "\000\000\000\000FALSE\001\000\000\000TRUE";

/** \brief 0x801 (Boolean) Enum value buffer*/
CHAR *apEnum0801[2] = { sEnum0801_Value, (sEnum0801_Value+9)};

/** \brief 0x801 (Boolean) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x0801[] =
   {{DEFTYPE_UNSIGNED8, 8, ACCESS_READ | OBJACCESS_NOPDOMAPPING},
    {DEFTYPE_OCTETSTRING, 72, ACCESS_READ | OBJACCESS_NOPDOMAPPING},
   {DEFTYPE_OCTETSTRING, 64, ACCESS_READ | OBJACCESS_NOPDOMAPPING}};
#endif
/*ECATCHANGE_END(V5.11) TEST3*/


#ifdef _OBJD_
/** \brief 0x3001 (Real32 object) entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x3001 = {DEFTYPE_REAL32, 0x20, ACCESS_READWRITE};

/** \brief 0x3001 (Real32 object) object name*/
OBJCONST UCHAR OBJMEM aName0x3001[] = "Real32 object\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3001 (Real32 object) object variable*/
PROTO UINT32   r32Obj
#ifdef _TESTAPPL_
    = 0xBABABABA
#endif
    ;


#ifdef _OBJD_
/** \brief 0x3002 (UINT32 object) entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x3002 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE};

/** \brief 0x3002 (UINT32 object) object name*/
OBJCONST UCHAR OBJMEM aName0x3002[] = "UINT32 object\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3002 (UINT32 object) object variable*/
PROTO UINT32   u32Obj
#ifdef _TESTAPPL_
    = 0xBABABABA
#endif
    ;


#ifdef _OBJD_
/** \brief 0x3003 (ByteAndLessEntryObject) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x3003[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },      /* SubIndex 000 */
   {DEFTYPE_BOOLEAN, 0x1, ACCESS_READWRITE },   /* SubIndex 001 : BOOLEAN */
   {DEFTYPE_BIT1, 0x1, ACCESS_READWRITE },      /* SubIndex 002 : BIT1*/
   {DEFTYPE_BIT2, 0x2, ACCESS_READWRITE },      /* SubIndex 003 : BIT2*/
   {DEFTYPE_BIT3, 0x3, ACCESS_READWRITE },      /* SubIndex 004 : BIT3*/
   {0x0000, 0x1, 0},                            /* SubIndex 005 : 1BIT ALIGN*/
   {DEFTYPE_BIT4, 0x4, ACCESS_READWRITE },      /* SubIndex 006 : BIT4*/
   {0x0000, 0x4, 0},                            /* SubIndex 007 : 4BIT ALIGN*/
   {DEFTYPE_BIT5, 0x5, ACCESS_READWRITE },      /* SubIndex 008 : BIT5*/
   {0x0000, 0x3, 0},                            /* SubIndex 009 : 3BIT ALIGN*/   
   {DEFTYPE_BIT6, 0x6, ACCESS_READWRITE },      /* SubIndex 010 : BIT6*/
   {0x0000, 0x2, 0},                            /* SubIndex 011 : 2BIT ALIGN*/
   {DEFTYPE_BIT7, 0x7, ACCESS_READWRITE },      /* SubIndex 012 : BIT7*/
   {0x0000, 0x1, 0},                            /* SubIndex 013 : 1BIT ALIGN*/
   {DEFTYPE_BIT8, 0x8, ACCESS_READWRITE },      /* SubIndex 014 : BIT8*/
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READWRITE }, /* SubIndex 015 : BYTE1*/
   {DEFTYPE_INTEGER8, 0x8, ACCESS_READWRITE}};  /* SubIndex 016 : BYTE2*/

/** \brief 0x3003 (ByteAndLessEntryObject) object and entry names*/
OBJCONST UCHAR OBJMEM aName0x3003[] ="ByteAndLessEntryObject\000Boolean(def TRUE)\000Bit1(def 0x00)\000Bit2(def 0x3)\000Bit3(def 0x5)\0001Bit alignment\000Bit4(def 0xA)\0004Bit alignment\000Bit5(def 0x1A)\0003Bit alignment\000Bit6(def 0x2A)\0002Bit align\000Bit7(def 0x6A)\0001Bit align\000Bit8(def 0xFF)\000UnsignedByte(def 0xAA)\000SignedByte(def 0xBB)\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3003 (ByteAndLessEntryObject) object variable*/
PROTO TOBJ3003 ByteAndLessObject
#ifdef _TESTAPPL_
= {16, 0x1, 0x0, 0x3, 0x5, 0/*1Bit alignment*/, 0xA,0/*4Bit alignment*/,  0x1A, 0/*3Bit alignment*/, 0x2A, 0/*2Bit alignment*/, 0x6A,0/*1Bit alignment*/,  0xFF, 0xAA, 0xBB}
#endif
;
/** \brief 0x3004 (Aligned/Access Test Object) entry descriptions*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x3004[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READWRITE }, /* SubIndex 000 */
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex 001*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READWRITE}, /* SubIndex 002*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_WRITE}, /* SubIndex 003*/
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READWRITE}, /* SubIndex 004*/
   {0x0000, 0x08, 0}, /* SubIndex 005: 8Bit Alignment*/
   {0x0000, 0, 0}, /* SubIndex 006: empty*/
   {0x0000, 0x08, 0}, /* SubIndex 007: 8Bit Alignment*/
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ}, /* SubIndex 008*/
   {DEFTYPE_BIT2, 0x02, ACCESS_WRITE}, /* SubIndex 009*/
   {DEFTYPE_BIT2, 0x02, ACCESS_READWRITE}, /* SubIndex 010*/
   {0x0000, 0x02, 0}, /* SubIndex 0011: 2Bit Alignment*/
   {DEFTYPE_BIT2, 0x02, ACCESS_READ}, /* SubIndex 012*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READWRITE}, /* SubIndex 013*/
   {DEFTYPE_BIT7, 0x07, ACCESS_READWRITE}}; /* SubIndex 014*/

/** \brief 0x3004 (Aligned/Access Test Object) object and entry names*/
OBJCONST UCHAR OBJMEM aName0x3004[] = "Alignment and Access Test Object\000/*SI1*/UINT16_ro(0x1122)\000/*SI2*/UINT16_rw(0x3344)\000/*SI3*/UINT16_wo(0x5566)\000/*SI4*/UINT8_rw(0x77)\000/*SI5*/8Bit Alignment\000/*SI6*/\000/*SI7*/8Bit Alignment\000/*SI8*/UINT8_ro(0x88)\000/*SI9*/2BIT_wo(0x01)\000/*SI10*/2BIT_rw(0x2)\000/*SI11*/2Bit Alignment\000/*SI12*/2BIT_ro(0x03)\000/*SI13*/BOOLEAN_rw(TRUE)\000/*SI14*/Block Entry\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3004 (Aligned/Access Test Object) object variable*/
PROTO TOBJ0x3004 sObj3004
#ifdef _TESTAPPL_
= {0x0E,0x1122,0x3344,0x5566,0x77,0/*8Bit Align*/,0/*8Bit Align*/,0x88,0x1,0x2,0/*2Bit Align*/,0x3,TRUE,0};
#endif
;


#ifdef _OBJD_
/** \brief 0x3005 (Enum TestObject) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x3005[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ }, /* Subindex 000 */
   {0x0800, 0x03, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS}, /* SubIndex 001: Presentation */
   {0x0801, 0x03, ACCESS_READWRITE | OBJACCESS_BACKUP | OBJACCESS_SETTINGS}, /* SubIndex 002: Boolean*/
   {0, 0xA, 0}}; /* Subindex 003 */

/** \brief 0x3005 (Enum TestObject) object and entry names*/
OBJCONST UCHAR OBJMEM aName0x3005[] = "Enum Test Object\000Presentation\000Boolean\000Alignment\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3005 (Enum TestObject) object variable*/
PROTO TOBJ3005 sObj3005
#ifdef _TESTAPPL_
= {2, 0,1,0}
#endif
;


/** \brief 0x3006 (Huge VAR object) entry description*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x3006= {DEFTYPE_OCTETSTRING, (HUGE_OBJ_BYTE_SIZE *8), (ACCESS_READ | ACCESS_WRITE)};

/** \brief 0x3006 (Huge VAR object) object name*/
OBJCONST UCHAR OBJMEM aName0x3006[] = "Huge VAR Object\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3006 (Huge VAR object) object variable*/
PROTO UINT8   HugeObj[HUGE_OBJ_BYTE_SIZE];


#ifdef _OBJD_
/** \brief 0x3007 (Bit3Array) entry descriptions*/
TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x3007[] = {
    {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READWRITE }, /* SubIndex 000 */
    {DEFTYPE_BIT3, 0x03, ACCESS_READWRITE}};

/** \brief 0x3007 (Bit3Array) object name*/
OBJCONST UCHAR OBJMEM aName0x3007[] = "Bit3 Array Object\000\377";
#endif //#ifdef _OBJD_


/** \brief 0x3007 (Bit3Array) object variable*/
PROTO TOBJ0x3007 sObj3007
#ifdef _TESTAPPL_
= {5,0x36DB};
#endif
;
/** \brief 0x3008 (Basedatatype Arrays Object) entry descriptions*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x3008[] = {
    {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READWRITE }, /* SubIndex 000 */
    {DEFTYPE_UNICODE_STRING, 0x20, ACCESS_READWRITE },
    {DEFTYPE_OCTETSTRING, 0x20, ACCESS_READWRITE },
    {DEFTYPE_BITARR32, 0x20, ACCESS_READWRITE },
    {DEFTYPE_BITARR16, 0x10, ACCESS_READWRITE },
    {DEFTYPE_BITARR8, 0x08, ACCESS_READWRITE }};

/** \brief 0x3008 (Basedatatype Arrays Object) object and entry names*/
OBJCONST UCHAR OBJMEM aName0x3008[] = "Basedatatype Arrays Object\000UintArr\000ByteArr\000BitArr32\000BitArr16\000BitArr8\000\377";
#endif //#ifdef _OBJD_


/** \brief 0x3008 (Basedatatype Arrays Object) object variable*/
PROTO TOBJ0x3008 sObj3008
#ifdef _TESTAPPL_
= {5,0xAAAAAAAA,0xBBBBBBBB,0xCCCCCCCC,0xDDDD,0xEE};
#endif
;

#ifdef _OBJD_
/** \brief 0x3009/0x300A (Huge Array/Record Object) entry descriptions*/
TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x3009[] = {
    {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READWRITE }, /* SubIndex 000 */
#if EL9800_HW
    {DEFTYPE_OCTETSTRING, 0x10, ACCESS_READWRITE }};
#else
    {DEFTYPE_OCTETSTRING, 0x400, ACCESS_READWRITE }};
#endif

/** \brief 0x3009 (Huge Array/Record Object) object name*/
OBJCONST UCHAR OBJMEM aName0x3009[] = "Huge Array Object\000\377";

/** \brief 0x300A (Huge Array/Record Object) entry descriptions*/
OBJCONST UCHAR OBJMEM aName0x300A[] = "Huge Record Object\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x3009/0x300A (Huge Array/Record Object) object variable*/
PROTO TOBJ0x3009 sObj3009
#ifdef _TESTAPPL_
#if EL9800_HW
= {254,0x10,0xBABA};
#else
= {254,0x400,0xBABA};
#endif
#endif
;

/* ECATCHANGE_START(V5.11) TEST1*/
/** \brief 0x300B (Byte Arrays with odd word length) entry descriptions*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC OBJMEM asEntryDesc0x300B[] = {
   {DEFTYPE_UNSIGNED8,      0x8, ACCESS_READ },
   {DEFTYPE_VISIBLESTRING, 0x28, ACCESS_READWRITE},
   {DEFTYPE_UNSIGNED8,      0x8, ACCESS_READ},
   {DEFTYPE_OCTETSTRING, 0x28, ACCESS_READWRITE},
   {DEFTYPE_UNSIGNED8,      0x8, ACCESS_READWRITE}
};
OBJCONST UCHAR OBJMEM aName0x300B[] =
   "Byte Arrays with odd word length\000"
   "String (def: '12345')\000"
   "SubIndex2 (def: 0x66)\000"
   "ByteArray (def: 0x778899AABB)\000"
   "SubIndex4 (def: 0xCC)\000"
   "\377";
#endif //#ifdef _OBJD_

typedef struct OBJ_STRUCT_PACKED_START {
   UINT16 u16SubIndex0;
   UINT8  String[5];
   UINT8  u8SubIndex2;
   UINT8  ByteArray[5];
   UINT8  u8SubIndex4;
} OBJ_STRUCT_PACKED_END
TOBJ300B;
PROTO TOBJ300B sObj300B
#ifdef _TESTAPPL_
= {4, {0x31, 0x32, 0x33, 0x34, 0x35, }, 0x66, {0x77, 0x88, 0x99, 0xAA, 0xBB, }, 0xCC }
#endif
;
/* ECATCHANGE_END(V5.11) TEST1*/

/*ECATCHANGE_START(V5.11) TEST2*/
#if !_PIC24
/** \brief 0x300C (Max entry size object) entry description*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC    OBJMEM sEntryDesc0x300C= {DEFTYPE_OCTETSTRING, 0xFFFF, ACCESS_READ};

/** \brief 0x300C (Max entry size object) object name*/
OBJCONST UCHAR OBJMEM aName0x300C[] = "Max entry size object\000\377";
#endif //#ifdef _OBJD_
#endif //#if !_PIC24
/*ECATCHANGE_END(V5.11) TEST2*/
/** @}*/

/*ECATCHANGE_START(V5.11) TEST6*/
/** \brief 0x300D (Slave-to-Slave test object) entry descriptions*/
#ifdef _OBJD_
OBJCONST TSDOINFOENTRYDESC OBJMEM asEntryDesc0x300D[] = {
   {DEFTYPE_UNSIGNED8,  0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READWRITE},
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READWRITE},
   {DEFTYPE_UNSIGNED8, 0x08, ACCESS_READ},
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}
};
OBJCONST UCHAR OBJMEM aName0x300D[] = 
   "Slave-to-Slave test object\000"
   "Destination slave address\000"
   "Command\000"
   "Status\000"
   "IdentValue\000"
   "\377";
#endif //#ifdef _OBJD_

typedef struct OBJ_STRUCT_PACKED_START {
   UINT16 u16SubIndex0;
   UINT16 DstSlv;
   UINT8 Command;
   UINT8 Status;
   UINT32 IdentData;
} OBJ_STRUCT_PACKED_END
TOBJ300D;
PROTO TOBJ300D sObj300D_S2S
#ifdef _TESTAPPL_
    = {4, 0,0,0,0}
#endif
;
/*ECATCHANGE_END(V5.11) TEST6*/

/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */
#ifdef _OBJD_
/**
 * \brief Object 0x1C12 (SM2 assignment) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1C12[] = "RxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1C12 (SM2 assignment) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex1 : 0x1601<br>
 * SubIndex2 : 0
 */
PROTO TOBJ1C12 sRxPDOassign
#ifdef _TESTAPPL_
#if MAX_PD_OUTPUT_SIZE > 0
= {0x01, {0x1601,0}}
#else
= {0x00, {0,0}}
#endif
#endif
;


#ifdef _OBJD_
/**
 * \brief Object 0x1C13 (SM3 assignment) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1C13[] = "TxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1C13 (SM3 assignment) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex1 : 0x1A00<br>
 * SubIndex2 : 0x1AFF
 */
PROTO TOBJ1C13 sTxPDOassign
#ifdef _TESTAPPL_
#if MAX_PD_INPUT_SIZE > 0
= {0x01, {0x1A00,0x0000}}
#else
= {0x00, {0,0}}
#endif
#endif

;
/** @}*/


/**
* \addtogroup PdoMappingObjects PDO Mapping Objects
* @{
*/
#ifdef _OBJD_

/**
 * \brief Object 0x1601 (RxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1601[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

   
/**
 * \brief Object 0x1601 (RxPDO) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1601[] = "RxPDO-Map\000\377";
#endif //#ifdef _OBJD_

/**
 * \brief Object 0x1601 (RxPDO) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex1 : 0x7010.0 32Bit
 */
PROTO TOBJ1601 RxPDOMap
#ifdef _TESTAPPL_
 = {1, {0x70100020}}
#endif
;



#ifdef _OBJD_
/**
 * \brief Object 0x1603 (RxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1603[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

   
/**
 * \brief Object 0x1603 (RxPDO) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1603[] = "RxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1603 (RxPDO) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex1 : 0x7030.0 32Bit
 */
PROTO TOBJ1603 RxPDOMap1
#ifdef _TESTAPPL_
 = {1, {0x70300020}}
#endif
;


#ifdef _OBJD_
/**
 * \brief Object 0x1A00 (TxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

   
/**
 * \brief Object 0x1A00 (TxPDO) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1A00[] = "TxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1A00 (TxPDO) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex1 : 0x6000.0 32Bit
 */
PROTO TOBJ1A00 TxPDOMap
#ifdef _TESTAPPL_
 = {1, {0x60000020}}
#endif
;


#ifdef _OBJD_
/**
 * \brief Object 0x1A02 (TxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A02[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

   
/**
 * \brief Object 0x1A02 (TxPDO) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1A02[] = "TxPDO-Map\000\377";
#endif //#ifdef _OBJD_



/**
 * \brief Object 0x1A02 (TxPDO) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex1 : 0x6020.0 16Bit
 */
PROTO TOBJ1A02 TxPDOMap1
#ifdef _TESTAPPL_
 = {1, {0x60200010}}
#endif
;
/** @}*/


#if AL_EVENT_ENABLED && DC_SUPPORTED   
#ifdef _OBJD_
/**
 * \brief Object 0x1AFF (TxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1..n
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1AFF[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};

/**
 * \brief Object 0x1AFF (TxPDO) object name
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1AFF[] = "TxPDO-Map Esm variables\000\377";
#endif //#ifdef _OBJD_

/**
 * \brief Object 0x1AFF (TxPDO) object variable
 *
 * SubIndex0 : 1<br>
 * SubIndex01 : 0xA000.1 value of "nEcatStateTrans" (UINT16)
 * SubIndex02 : 0xA000.2 value of "EsmTimeoutCounter" (Int16)
 * SubIndex03 : 0xA000.3 value of "i16WaitForPllRunningTimeout"
 * SubIndex04 : 0xA000.4 value of "i16WaitForPllRunningCnt"
 * SubIndex05 : 0xA000.5 value of "u16LocalErrorCode"
 * SubIndex06 : 0xA000.6 value of "bApplEsmPending"
 * SubIndex07 : 0xA000.7 value of "bEcatWaitForAlControlRes"
 * SubIndex08 : 0xA000.8 value of "bEcatOutputUpdateRunning"
 * SubIndex09 : 0xA000.9 value of "bEcatInputUpdateRunning"
 * SubIndex10 : 0xA000.10 value of "bEcatFirstOutputsReceived"
 * SubIndex11 : 0xA000.11 value of "bLocalErrorFlag"
 * SubIndex12 : 0xA000.12 2Bit Alignment
 
 * SubIndex13 : 0xA000.17 value of "bWdTrigger"
 * SubIndex14 : 0xA000.18 value of "bDcSyncActive"
 * SubIndex15 : 0xA000.19 value of "bEscIntEnabled"
 * SubIndex16 : 0xA000.20 value of "bDcRunning"
 * SubIndex17 : 0xA000.21 value of "bSmSyncSequenceValid"
 * SubIndex18 : 0xA000.22 value of 3Bit Alignment

 * SubIndex21 : 0xA000.33 value of "EcatWdValue" (UINT16)
 * SubIndex22 : 0xA000.34 value of "EcatWdCounter" (UINT16)
 * SubIndex23 : 0xA000.35 value of "Sync0WdCounter" (UINT16)
 * SubIndex24 : 0xA000.36 value of "Sync0WdValue" (UINT16)
 * SubIndex25 : 0xA000.37 value of "Sync1WdCounter" (UINT16)
 * SubIndex26 : 0xA000.38 value of "Sync1WdValue" (UINT16)
 * SubIndex27 : 0xA000.39 value of "LatchInputSync0Value" (UINT16)
 * SubIndex28 : 0x1C32.11 value of "u16SmEventMissedCounter" (UINT16)
 */
PROTO TOBJ1AFF TxPDOMapEsmDiag
#ifdef _TESTAPPL_
 = {28, {0xA0000110/*nEcatStateTrans*/,0xA0000210/*EsmTimeoutCounter*/,0xA0000310/*i16WaitForPllRunningTimeout*/,0xA0000410/*i16WaitForPllRunningCnt*/,
0xA0000510/*u16LocalErrorCode*/,0xA0000601/*bApplEsmPending*/,0xA0000701/*bEcatWaitForAlControlRes*/,0xA0000801/*bEcatOutputUpdateRunning*/,
0xA0000901/*bEcatInputUpdateRunning*/,0xA0000A01/*bEcatFirstOutputsReceived*/,0xA0000B01/*bLocalErrorFlag*/,0xA0000C02/*2Bit Alignment*/,
0xA0001101/*bWdTrigger*/,0xA0001201/*bDcSyncActive*/,0xA0001301/*bEscIntEnabled*/,0xA0001401/*bDcRunning*/,0xA0001601/*bSmSyncSequenceValid*/,0xA0001703/*3Bit Alignment*/,
0xA0002110/*EcatWdValue*/,0xA0002210/*EcatWdCounter*/,0xA0002310/*Sync0WdCounter*/,0xA0002410/*Sync0WdValue*/,0xA0002510/*Sync1WdCounter*/,0xA0002610/*Sync1WdValue*/,0xA0002710/*LatchInputSync0Value*/,
0x1C320B10/*u16SmEventMissedCounter*/}}
#endif
;
/** @}*/
#endif //#if AL_EVENT_ENABLED && DC_SUPPORTED


/**
 * \addtogroup ProcessDataObjects Process data objects
 * 
 * @{
 */
#ifdef _OBJD_
/** \brief 0x6000 (Input counter object) entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x6000 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ | OBJACCESS_TXPDOMAPPING};

/** \brief 0x6000 (Input counter object) object name*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputCounter";
#endif //#ifdef _OBJD_

#endif //#if COE_SUPPORTED

/** \brief 0x6000 (Input counter object) object variable*/
PROTO UINT32 InputCounter
#ifdef _TESTAPPL_
= 0x0;
#endif
;

#if COE_SUPPORTED
#ifdef _OBJD_
/** \brief 0x6020 (Counter Overrun) entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x6020 = {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ | OBJACCESS_TXPDOMAPPING};

/** \brief 0x6020 (Counter Overrun) object name*/
OBJCONST UCHAR OBJMEM aName0x6020[] = "Counter Overrun";
#endif //#ifdef _OBJD_

/** \brief 0x6020 (Counter Overrun) object variable*/
PROTO UINT16 CounterOverrun
#ifdef _TESTAPPL_
= 0x0
#endif
;


#ifdef _OBJD_
/** \brief 0x7010 (Output counter object) entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x7010 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING};

/** \brief 0x7010 (Output counter object) object name*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputCounter";
#endif //#ifdef _OBJD_

#endif //#if COE_SUPPORTED
/** \brief 0x7010 (Output counter object) object variable*/
PROTO UINT32 OutputCounter
#ifdef _TESTAPPL_
= 0x0
#endif
;

#if COE_SUPPORTED
#ifdef _OBJD_
/** \brief 0x7030 (output counter object) entry description*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x7030 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING};

/** \brief 0x7030 (output counter object) object name*/
OBJCONST UCHAR OBJMEM aName0x7030[] = "Output Counter 1";
#endif //#ifdef _OBJD_

/** \brief 0x7030 (output counter object) object variable*/
PROTO UINT32 OutputCounter1
#ifdef _TESTAPPL_
= 0x0
#endif
;
/** @}*/

#ifdef _OBJD_
/** \brief 0x8000 (Stack Configuration) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x8000[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}};

/** \brief 0x8000 (Stack Configuration) object name (the entry names are handled in an extra variable)*/
OBJCONST UCHAR OBJMEM aName0x8000[] = "StackConfiguration\000\377";
#endif //#ifdef _OBJD_

/** \brief 0x8000 (Stack Configuration) object variable*/
PROTO TSLV_CONFIG aSlaveConfig[48]
#ifdef _TESTAPPL_
= {{"ESC_EEPROM_EMULATION",ESC_EEPROM_EMULATION},
    {"EL9800_HW",EL9800_HW},
    {"MCI_HW",MCI_HW},
    {"FC1100_HW",FC1100_HW},
    {"CONTROLLER_16BIT",CONTROLLER_16BIT},
    {"CONTROLLER_32BIT",CONTROLLER_32BIT},
    {"_PIC18",_PIC18},
    {"_PIC24",_PIC24},
    {"ESC_16BIT_ACCESS",ESC_16BIT_ACCESS},
    {"ESC_32BIT_ACCESS",ESC_32BIT_ACCESS},
    {"MBX_16BIT_ACCESS",MBX_16BIT_ACCESS},
    {"BIG_ENDIAN_16BIT",BIG_ENDIAN_16BIT},
    {"BIG_ENDIAN_FORMAT",BIG_ENDIAN_FORMAT},
    {"EXT_DEBUGER_INTERFACE",EXT_DEBUGER_INTERFACE},
    {"UC_SET_ECAT_LED",UC_SET_ECAT_LED},
    {"ESC_SUPPORT_ECAT_LED",ESC_SUPPORT_ECAT_LED},
    {"AL_EVENT_ENABLED",AL_EVENT_ENABLED},
    {"DC_SUPPORTED",DC_SUPPORTED},
    {"ECAT_TIMER_INT",ECAT_TIMER_INT},
    {"INTERRUPTS_SUPPORTED",INTERRUPTS_SUPPORTED},
    {"TEST_APPLICATION",TEST_APPLICATION},
    {"EL9800_APPLICATION",EL9800_APPLICATION},
    {"CiA402_DEVICE",CiA402_DEVICE},
    {"SAMPLE_APPLICATION",SAMPLE_APPLICATION},
    {"SAMPLE_APPLICATION_INTERFACE",SAMPLE_APPLICATION_INTERFACE},
    {"MAILBOX_QUEUE",MAILBOX_QUEUE},
    {"AOE_SUPPORTED",AOE_SUPPORTED},
    {"COE_SUPPORTED",COE_SUPPORTED},
    {"COMPLETE_ACCESS_SUPPORTED",COMPLETE_ACCESS_SUPPORTED},
    {"SEGMENTED_SDO_SUPPORTED",SEGMENTED_SDO_SUPPORTED},
    {"SDO_RES_INTERFACE",SDO_RES_INTERFACE},
    {"BACKUP_PARAMETER_SUPPORTED",BACKUP_PARAMETER_SUPPORTED},
    {"STORE_BACKUP_PARAMETER_IMMEDIATELY",STORE_BACKUP_PARAMETER_IMMEDIATELY},
    {"DIAGNOSIS_SUPPORTED",DIAGNOSIS_SUPPORTED},
    {"EMERGENCY_SUPPORTED",EMERGENCY_SUPPORTED},
    {"VOE_SUPPORTED",VOE_SUPPORTED},
    {"SOE_SUPPORTED",SOE_SUPPORTED},
    {"EOE_SUPPORTED",EOE_SUPPORTED},
    {"STATIC_ETHERNET_BUFFER",STATIC_ETHERNET_BUFFER},
    {"FOE_SUPPORTED",FOE_SUPPORTED},
    {"FOE_SAVE_FILES",FOE_SAVE_FILES},
    {"MAILBOX_SUPPORTED",MAILBOX_SUPPORTED},
    {"BOOTSTRAPMODE_SUPPORTED",BOOTSTRAPMODE_SUPPORTED},
    {"OP_PD_REQUIRED",OP_PD_REQUIRED},
    {"ESC_SM_WD_SUPPORTED",ESC_SM_WD_SUPPORTED},
    {"STATIC_OBJECT_DIC",STATIC_OBJECT_DIC},
    {"ESC_EEPROM_ACCESS_SUPPORT",ESC_EEPROM_ACCESS_SUPPORT},
    {"EXPLICIT_DEVICE_ID",EXPLICIT_DEVICE_ID}}
#endif
;

/** \brief 0x8000 (Stack Configuration) SubIndex0 variable*/
PROTO UINT16 SlaveConfigObjSubindex0
#ifdef _TESTAPPL_
    =(SIZEOF(aSlaveConfig)/SIZEOF(TSLV_CONFIG))
#endif
    ;

#if AL_EVENT_ENABLED && DC_SUPPORTED
#ifdef _OBJD_
/** \brief 0xA000 (ESM variables) entry descriptions*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0xA000[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex001 "nEcatStateTrans" (UINT16)*/
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READ}, /* SubIndex002 "EsmTimeoutCounter" (Int16)*/
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READ}, /* SubIndex003 "i16WaitForPllRunningTimeout"*/
   {DEFTYPE_INTEGER16, 0x10, ACCESS_READ}, /* SubIndex004 "i16WaitForPllRunningCnt"*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex005 "u16LocalErrorCode"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex006 "bApplEsmPending"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex007 "bEcatWaitForAlControlRes"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex008 "bEcatOutputUpdateRunning"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex009 "bEcatInputUpdateRunning"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex010 "bEcatFirstOutputsReceived"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex011 "bLocalErrorFlag"*/
   {DEFTYPE_NULL, 0x02, 0x0000}, /* SubIndex012 2Bit Alignment*/
   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex017 "bWdTrigger"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex018 "bDcSyncActive"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex019 "bEscIntEnabled"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex020 "bDcRunning"*/
   {DEFTYPE_BOOLEAN, 0x01, ACCESS_READ}, /* SubIndex021 "bSmSyncSequenceValid"*/
   {DEFTYPE_NULL, 0x03, 0x0000},   /* SubIndex022 3Bit Alignment*/
   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},   {DEFTYPE_NULL, 0x00, 0x0000},
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex033 "EcatWdValue" (UINT16)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex034 "EcatWdCounter" (UINT16)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex035 "Sync0WdCounter" (UINT16)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex036 "Sync0WdValue" (UINT16)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex037 "Sync1WdCounter" (UINT16)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}, /* SubIndex038 "Sync1WdValue" (UINT16)*/
   {DEFTYPE_UNSIGNED16, 0x10, ACCESS_READ}}; /* SubIndex039 "LatchInputSync0Value" (UINT16)*/

/** \brief 0xA000 (Esm Variables) object and entry names*/
OBJCONST UCHAR OBJMEM aName0xA000[] = "ESM variables\000nEcatStateTrans\000EsmTimeoutCounter\000i16WaitForPllRunningTimeout\000i16WaitForPllRunningCnt\000u16LocalErrorCode\000bApplEsmPending\000bEcatWaitForAlControlRes\
\000bEcatOutputUpdateRunning\000bEcatInputUpdateRunning\000bEcatFirstOutputsReceived\000bLocalErrorFlag\000b\000\000\000\000\000WdTrigger\000bDcSyncActive\000bEscIntEnabled\
\000bDcRunning\000bPllRunning\000Si22 3Bit Alignment\000\000\000\000\000\000\000\000\000\000\000EcatWdValue\000EcatWdCounter\000Sync0WdCounter\000Sync0WdValue\
\000Sync1WdCounter\000Sync1WdValue\000LatchInputSync0Value\000\377";
#endif //#ifdef _OBJD_

/** \brief 0xA000 (ESM variables) SubIndex0 variable (all other entries are handled in extra variables, SDO access will be handle in Read function)*/
PROTO UINT16 A000Subindex0
#ifdef _TESTAPPL_
    =39
#endif
    ;

#endif //#if AL_EVENT_ENABLED && DC_SUPPORTED



/** \brief 0xF010 (Module Profile List) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex0*/
   UINT32   aEntries[3]; /**< \brief Module profile information buffer
                          * 
                          * Bit 0..15: Profile number of the module on position 1<br>
                          * Bit 16..31: Profile specific*/
} OBJ_STRUCT_PACKED_END
TOBJF010;


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
#ifdef _TESTAPPL_
= {0x01,{0x64}}
#endif
;

PROTO UINT8 WriteTestObject( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadTestObject( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 WriteObject0x2FFF( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadObject0x2FFF( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 WriteObject0x3009_0x300A( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadObject0x3009_0x300A( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadObject0x8000( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
#if AL_EVENT_ENABLED && DC_SUPPORTED
PROTO UINT8 ReadObject0xA000( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
#endif  // #if AL_EVENT_ENABLED && DC_SUPPORTED

PROTO UINT8 WriteObject0x3007( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadObject0x3007( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );

#if SDO_RES_INTERFACE
PROTO UINT8 WriteObject0x3006( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadObject0x3006( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 WriteObject0x3002( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
PROTO UINT8 ReadObject0x3002( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
#endif

#if !_PIC24
/*ECATCHANGE_START(V5.11) TEST2*/
PROTO UINT8 ReadObject0x300C( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess );
/*ECATCHANGE_END(V5.11) TEST2*/
#endif

#ifdef _OBJD_
/** \brief Test application object dictionary*/
TOBJECT    OBJMEM ApplicationObjDic[] = {
    /* Enum 0x0800 */
    {NULL,NULL, 0x0800, {DEFTYPE_ENUM, 0x02 | (OBJCODE_REC << 8)}, asEntryDesc0x0800, 0, apEnum0800 },
    /* Enum 0x0801 */
    {NULL,NULL, 0x0801, {DEFTYPE_ENUM, 2 | (OBJCODE_REC << 8)}, asEntryDesc0x0801, 0, apEnum0801 },
    /* Object 0x1601 */
    {NULL,NULL,  0x1601, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1601, aName0x1601, &RxPDOMap, NULL, NULL, 0x0000 },
    /* Object 0x1603 */
    {NULL,NULL,  0x1603, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1603, aName0x1603, &RxPDOMap1, NULL, NULL, 0x0000 },
    /* Object 0x1A00 */
    {NULL,NULL,   0x1A00, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1A00, aName0x1A00, &TxPDOMap, NULL, NULL, 0x0000 },
    /* Object 0x1A02 */
    {NULL,NULL,  0x1A02, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1A02, aName0x1A02, &TxPDOMap1, NULL, NULL, 0x0000 },
#if AL_EVENT_ENABLED && DC_SUPPORTED
    /* Object 0x1AFF */
    {NULL,NULL,  0x1AFF, {DEFTYPE_PDOMAPPING, 28 | (OBJCODE_REC << 8)}, asEntryDesc0x1AFF, aName0x1AFF, &TxPDOMapEsmDiag, NULL, NULL, 0x0000 },
#endif //#if AL_EVENT_ENABLED && DC_SUPPORTED
    /* Object 0x1C12 */
#if MAX_PD_OUTPUT_SIZE > 0
    {NULL,NULL,   0x1C12, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
#else
    {NULL,NULL,   0x1C12, {DEFTYPE_UNSIGNED16, 0 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
#endif
    /* Object 0x1C13 */
#if MAX_PD_INPUT_SIZE > 0
    {NULL,NULL,   0x1C13, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
#else
    {NULL,NULL,   0x1C13, {DEFTYPE_UNSIGNED16, 0 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
#endif
    /* Object 0x2000 */
    {NULL,NULL,  0x2000, {DEFTYPE_RECORD, 4 | (OBJCODE_REC << 8)}, asTestObjEntryDesc, aName0x2000, &sESM0x2000, ReadTestObject, WriteTestObject, 0x0000 },
/*ECATCHANGE_START(V5.11) TEST4*/
    /* Object 0x2010 */
    {NULL,NULL,  0x2010, {DEFTYPE_RECORD, 2 | (OBJCODE_REC << 8)}, asTestObjEntryDesc, aName0x2010, &sMbx0x2010, ReadTestObject, WriteTestObject, 0x0000 },
/*ECATCHANGE_END(V5.11) TEST4*/
    /* Object 0x2020 */
    {NULL,NULL,  0x2020, {DEFTYPE_RECORD, 7 | (OBJCODE_REC << 8)}, asTestObjEntryDesc, aName0x2020, &sCoE0x2020, ReadTestObject, WriteTestObject, 0x0000 },
/* ECATCHANGE_START(V5.11) TEST5*/
    /* Object 0x2030 */
    {NULL,NULL,  0x2030, {DEFTYPE_RECORD, 1 | (OBJCODE_REC << 8)}, asTestObjEntryDesc, aName0x2030, &sFoE0x2030, ReadTestObject, WriteTestObject, 0x0000 },
/* ECATCHANGE_END(V5.11) TEST5*/
    /* Object 0x2FFF */
    {NULL,NULL,  0x2FFF, {DEFTYPE_UNSIGNED32,TEST_CONTROL_BITS | (OBJCODE_ARR << 8)}, asEntryDesc0x2FFF, aName0x2FFF, &sTestControlObj, ReadObject0x2FFF, WriteObject0x2FFF, 0x0000 },
    /* Object 0x3001 */
    {NULL,NULL,  0x3001, {DEFTYPE_REAL32, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x3001, aName0x3001, &r32Obj, NULL, NULL, 0x0000 },
    /* Object 0x3002 */
    {NULL,NULL,  0x3002, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x3002, aName0x3002, &u32Obj, ReadObject0x3002, WriteObject0x3002, 0x0000 },
    /* Object 0x3003 */
    {NULL,NULL,  0x3003, {DEFTYPE_RECORD, 16 | (OBJCODE_REC << 8)}, asEntryDesc0x3003, aName0x3003, &ByteAndLessObject, NULL, NULL, 0x0000 },
    /* Object 0x3004 */
    {NULL,NULL,  0x3004, {DEFTYPE_RECORD, 14 | (OBJCODE_REC << 8)}, asEntryDesc0x3004, aName0x3004, &sObj3004, NULL, NULL, 0x0000 },
    /* Object 0x3005 */
    {NULL,NULL,  0x3005, {DEFTYPE_RECORD, 2 | (OBJCODE_REC << 8)}, asEntryDesc0x3005, aName0x3005, &sObj3005, NULL, NULL, 0x0000 },
    /* Object 0x3006 */
    {NULL,NULL,  0x3006, {DEFTYPE_OCTETSTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x3006, aName0x3006, HugeObj, ReadObject0x3006, WriteObject0x3006, 0x0000 },
    /* Object 0x3007 */
    {NULL,NULL,  0x3007, {DEFTYPE_BIT3, 5 | (OBJCODE_ARR << 8)}, asEntryDesc0x3007, aName0x3007, &sObj3007, ReadObject0x3007, WriteObject0x3007, 0x0000 },
    {NULL,NULL,  0x3008, {DEFTYPE_RECORD, 5 | (OBJCODE_REC << 8)}, asEntryDesc0x3008, aName0x3008, &sObj3008, NULL, NULL, 0x0000 },
    /* Object 0x3009*/
    {NULL,NULL,  0x3009, {DEFTYPE_OCTETSTRING, 254 | (OBJCODE_ARR << 8)}, asEntryDesc0x3009, aName0x3009, &sObj3009, ReadObject0x3009_0x300A, WriteObject0x3009_0x300A, 0x0000 },    
    /* Object 0x300A*/
    {NULL,NULL,  0x300A, {DEFTYPE_OCTETSTRING, 254 | (OBJCODE_ARR/*will be set to REC in SDO Info*/ << 8)}, asEntryDesc0x3009, aName0x300A, &sObj3009, ReadObject0x3009_0x300A, WriteObject0x3009_0x300A, 0x0000 },    
/* ECATCHANGE_START(V5.11) TEST1*/
    /* Object 0x300B*/
    {NULL,NULL,   0x300B, {DEFTYPE_RECORD, 4 | (OBJCODE_REC << OBJFLAGS_OBJCODESHIFT)}, asEntryDesc0x300B, aName0x300B, &sObj300B, NULL, NULL, 0x0000 },
/* ECATCHANGE_END(V5.11) TEST1*/
#if !_PIC24
/*ECATCHANGE_START(V5.11) TEST2*/
    /* Object 0x300C */
    {NULL,NULL,  0x300C, {DEFTYPE_OCTETSTRING, 0 | (OBJCODE_VAR << 8)}, &sEntryDesc0x300C, aName0x300C, NULL, ReadObject0x300C, NULL, 0x0000 },
/*ECATCHANGE_END(V5.11) TEST2*/
#endif //#if !_PIC24
/*ECATCHANGE_START(V5.11) TEST6*/
    /* Object 0x300D */
    {NULL,NULL,  0x300D, {DEFTYPE_RECORD, 4 | (OBJCODE_REC << 8)}, asEntryDesc0x300D, aName0x300D, &sObj300D_S2S, NULL, NULL, 0x0000 },
/*ECATCHANGE_END(V5.11) TEST6*/
    /* Object 0x6000 */
    {NULL,NULL,   0x6000, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x6000, aName0x6000, &InputCounter, NULL, NULL, 0x0000 },
    /* Object 0x6020 */
    {NULL,NULL,   0x6020, {DEFTYPE_UNSIGNED16, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x6020, aName0x6020, &CounterOverrun, NULL, NULL, 0x0000 },
    /* Object 0x7010 */
    {NULL,NULL,   0x7010, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x7010, aName0x7010, &OutputCounter, NULL, NULL, 0x0000 },
    /* Object 0x7030 */
    {NULL,NULL,   0x7030, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x7030, aName0x7030, &OutputCounter1, NULL, NULL, 0x0000 },
    /* Object 0x8000 */
    {NULL,NULL,   0x8000, {DEFTYPE_RECORD, (SIZEOF(aSlaveConfig)/SIZEOF(TSLV_CONFIG)) | (OBJCODE_REC << 8)}, asEntryDesc0x8000, aName0x8000, &SlaveConfigObjSubindex0, ReadObject0x8000, NULL, 0x0000 },
#if AL_EVENT_ENABLED && DC_SUPPORTED
    /* Object 0xA000 */
    {NULL,NULL,   0xA000, {DEFTYPE_RECORD, 39 | (OBJCODE_REC << 8)}, asEntryDesc0xA000, aName0xA000, &A000Subindex0, ReadObject0xA000, NULL, 0x0000 },
#endif
   /* Object 0xF010 */
   {NULL,NULL,   0xF010, {DEFTYPE_UNSIGNED32, 3 | (OBJCODE_ARR << 8)}, asEntryDesc0xF010, aName0xF010, &sModulelist, NULL, NULL, 0x0000 },
    {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}};
#endif    //#ifdef _OBJD_

#endif //#if COE_SUPPORTED

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


PROTO void UpdateActiveTest(void);

#if BACKUP_PARAMETER_SUPPORTED
PROTO void      EE_ResetFlashData(void);
PROTO UINT32    EE_GetChecksum(void);
PROTO UINT8     EE_IsDefaultDataInitialized(void);
PROTO void      EE_StoreDefaultData(void);
PROTO void      EE_LoadDefaultData(void);
PROTO void      EE_ReadWordsFromNonVolatileMemory(UINT16 HUGE *pDest, UINT16 srcOffset, UINT16 n);
PROTO UINT32    EE_WriteWordsToNonVolatileMemory(UINT16 destOffset, UINT16 HUGE * pSrc, UINT16 n);
#endif


/*ECATCHANGE_START(V5.11) TEST9*/
PROTO BOOL bUnlockSdoRequest; /**< \brief If test behaviour 0x2020.7 is enabled the SDO response on object 0x3006.0 is written to the out mailbox if this variable is true)*/
/*ECATCHANGE_END(V5.11) TEST9*/


#undef PROTO
/** @}*/
