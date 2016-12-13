/*-----------------------------------------------------------------------------------------
------	
------	ecat_def.h
------  SSC version : 5.11
-----------------------------------------------------------------------------------------*/

#ifndef _ECATDEF_H_
#define _ECATDEF_H_

/*-----------------------------------------------------------------------------------------
------	
------	Includes
------ 
-----------------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 

/*-----------------------------------------------------------------------------------------
------	
------	Slave Sample Code Configuration Defines
------	
-----------------------------------------------------------------------------------------*/

#define USE_SPI

/** 
EL9800_HW: Shall be set if the Slave code is executed on the PIC mounted on the EL9800 EtherCAT Evaluation Board.<br>
(if the MCI interface provided by EL9800 board should be used MCI_HW shall be set and this define shall be reset).<br>
This settings should also be enabled if the ESC is connected via a serial interface and no specific hardware access files are avilable yet.<br>
NOTE: The PDI type needs also to be configured in the "ESC_CONFIG_DATA". */
#ifndef EL9800_HW
#define EL9800_HW                                 1
#endif

/** 
MCI_HW: Shall be set if the MCI of the ESC is connected.<br>
This settings should also be enabled if the ESC is connected via a parallel interface and no specific hardware access files are avilable yet.<br>
NOTE: The PDI type needs also to be configured in the "ESC_CONFIG_DATA". */
#ifndef MCI_HW
#define MCI_HW                                    0
#endif

/** 
FC1100_HW: Shall be set if the EtherCAT slave controller is located on an FC1100 PCI card.<br>
NOTE: The PDI type needs also to be configured in the "ESC_CONFIG_DATA". */
#ifndef FC1100_HW
#define FC1100_HW                                 0
#endif

/** 
TIESC_HW: Temporary define to support TI ICE/IDK development board (Shall be set if the Slave code shall executed on an AM335X ICE/IDK Evaluation Board<br>
from Texas Instruments) */
#ifndef TIESC_HW
#define TIESC_HW                                  0
#endif

/** 
CONTROLLER_16BIT: Shall be set if the host controller is a 16Bit architecture */
#ifndef CONTROLLER_16BIT
#define CONTROLLER_16BIT                          0
#endif

/** 
CONTROLLER_32BIT: Shall be set if the host controller is a 32Bit architecture */
#ifndef CONTROLLER_32BIT
#define CONTROLLER_32BIT                          1
#endif

/** 
_PIC18: Microchip PIC18F452 Specific Code <br>
This processor is mounted on the Beckhoff Slave Evaluation Board (Hardware version up to EL9800_2). */
#ifndef _PIC18
#define _PIC18                                    0
#endif

/** 
_PIC24: Microchip PIC24HJ128GP306 Specific Code <br>
This processor is mounted on the Beckhoff Slave Evaluation Board (Hardware version up to EL9800_4A). */
#ifndef _PIC24
#define _PIC24                                    1
#endif

/** 
ESC_16BIT_ACCESS: If the microcontroller only supports 16Bit access to the ESC. */
#ifndef ESC_16BIT_ACCESS
#define ESC_16BIT_ACCESS                          1
#endif

/** 
ESC_32BIT_ACCESS: If the microcontroller only supports 32Bit access to the ESC. */
#ifndef ESC_32BIT_ACCESS
#define ESC_32BIT_ACCESS                          0
#endif

/** 
MBX_16BIT_ACCESS: If the microcontroller only supports 16Bit access to local mailbox memory(access to ESC DPRAM is controlled by "ESC_16BIT_ACCESS"). If reset 8Bit access is used. */
#ifndef MBX_16BIT_ACCESS
#define MBX_16BIT_ACCESS                          1
#endif

/** 
BIG_ENDIAN_16BIT: If the microcontroller always make 16 bit access to external memory, operates in BigEndian format<br>
and the switching of the high and low byte is done in hardware. */
#ifndef BIG_ENDIAN_16BIT
#define BIG_ENDIAN_16BIT                          0
#endif

/** 
BIG_ENDIAN_FORMAT: If the microcontroller works with BigEndian format, then this switch shall be set. In that case all WORD-<br>
and DWORD-accesses will make a BYTE- or WORD-swapping, the macros SWAPWORD and SWAPDWORD in ecatslv.h might be adapted. <br>
If this switch is set, then BIG_ENDIAN_16BIT shall be reset. */
#ifndef BIG_ENDIAN_FORMAT
#define BIG_ENDIAN_FORMAT                         0
#endif

/** 
EXT_DEBUGER_INTERFACE: If this switch is set, the external debugger interface on the EL9800_4A (_PIC24) will be activated.<br>
This define will be ignored if _PIC24 is not set. */
#ifndef EXT_DEBUGER_INTERFACE
#define EXT_DEBUGER_INTERFACE                     0
#endif

/** 
UC_SET_ECAT_LED: If set the EtherCAT Run and Error LEDs are set by the uController. If set ESC_SUPPORT_ECAT_LED shall be reset. */
#ifndef UC_SET_ECAT_LED
#define UC_SET_ECAT_LED                           1
#endif

/** 
ESC_SUPPORT_ECAT_LED: This switch can be enabled if the connected ESC support Error and Run LED indication. See the ESC datasheet if the LED indication is supported. If set UC_SET_ECAT_LED shall be reset. */
#ifndef ESC_SUPPORT_ECAT_LED
#define ESC_SUPPORT_ECAT_LED                      0
#endif

/** 
ESC_EEPROM_EMULATION: If this switch is set EEPROM emulation is supported. Not all ESC types support EEPROM emulation. See ESC datasheet for more information. */
#ifndef ESC_EEPROM_EMULATION
#define ESC_EEPROM_EMULATION                      0
#endif

/** 
ESC_EEPROM_SIZE: Specify the EEPROM size in Bytes of the connected EEPROM or the emulated EEPROM. */
#ifndef ESC_EEPROM_SIZE
#define ESC_EEPROM_SIZE                           0x800
#endif

/** 
EEPROM_READ_SIZE: Only required if EEPROM emulation is active. This value defines the number of bytes which will be read per opertion. */
#ifndef EEPROM_READ_SIZE
#define EEPROM_READ_SIZE                          0x8
#endif

/** 
EEPROM_WRITE_SIZE: Only required if EEPROM emulation is active. This value defines the number of bytes which will be written per opertion. */
#ifndef EEPROM_WRITE_SIZE
#define EEPROM_WRITE_SIZE                         0x2
#endif

/** 
AL_EVENT_ENABLED: If an interrupt routine shall be called when one of the Events in the AL Event Register (0x220) changes, <br>
this switch has to be defined to 1 (synchronous modes are supported). <br>
If the AL Event register shall only be polled, this switch has to be defined to 0 (only free run mode is supported). */
#ifndef AL_EVENT_ENABLED
#define AL_EVENT_ENABLED                          1
#endif

/** 
DC_SUPPORTED: If distributed clocks should be supported by the slave, then this switch shall be set.<br>
If this switch is set, then also AL_EVENT_ENABLED shall be set.<br>
NOTE: The DC support needs also be set in the "ESC_CONFIG_DATA" settings. */
#ifndef DC_SUPPORTED
#define DC_SUPPORTED                              1
#endif

/** 
ECAT_TIMER_INT: If this switch is set, then the watchdog time for the EtherCAT watchdog will be checked in a timer interrupt routine. */
#ifndef ECAT_TIMER_INT
#define ECAT_TIMER_INT                            0
#endif

/** 
INTERRUPTS_SUPPORTED: If this switch is set the slave stack provides interrupt handling.<br>
NOTE: value will be evaluated automatically!! */
#ifndef INTERRUPTS_SUPPORTED
#if (DC_SUPPORTED == 1) || (AL_EVENT_ENABLED == 1) || (ECAT_TIMER_INT == 1)
#define INTERRUPTS_SUPPORTED                      1
#else
#define INTERRUPTS_SUPPORTED                      0
#endif
#endif

/** 
TEST_APPLICATION: NOTE: THIS SETTING SHALL NOT BE USED TO CREATE A USER SPECIFIC APPLICATION!<br>
Select this setting to test the slave stack or a master implementation. For further information about this application see the SSC Application Node. */
#ifndef TEST_APPLICATION
#define TEST_APPLICATION                          0
#endif

/** 
EL9800_APPLICATION: Should be set if the Slave Sample Code runs on an EL9800_x Evaluation Board. */
#ifndef EL9800_APPLICATION
#define EL9800_APPLICATION                        1
#endif

/** 
CiA402_DEVICE: If this switch is set, then the sample implementation of CiA402 device profile will be activated. */
#ifndef CiA402_DEVICE
#define CiA402_DEVICE                             0
#endif

/** 
SAMPLE_APPLICATION: Select this define if the hardware independent sample application shall be activated. */
#ifndef SAMPLE_APPLICATION
#define SAMPLE_APPLICATION                        0
#endif

/** 
SAMPLE_APPLICATION_INTERFACE: Select this define to activate the Sample Application Interface. This provides an simple interface to create a static library and used in an external application.<br>
NOTE: The file "EtherCATSampleLibrary.h" includes the library interface (maybe need to be adapted).  */
#ifndef SAMPLE_APPLICATION_INTERFACE
#define SAMPLE_APPLICATION_INTERFACE              0
#endif

/** 
USE_DEFAULT_MAIN: Set to 1 if the main function of a default application shall be used.<br>
Otherwise the Init functions and the mainloop handler shall be called for a user specific function (see ET9300 Application Note for further details www.beckhoff.com/english.asp?download/ethercat_development_products.htm?id=71003127100387). */
#ifndef USE_DEFAULT_MAIN
#define USE_DEFAULT_MAIN                          0 /* See this for an example main implementation*/
#endif

/** 
MAILBOX_QUEUE: If this switch is set, the mailbox services will be stored in a queue.<br>
With this switch reset only one mailbox service can be processed in parallel. */
#ifndef MAILBOX_QUEUE
#define MAILBOX_QUEUE                             1
#endif

/** 
AOE_SUPPORTED: If the AoE services are supported, then this switch shall be set. */
#ifndef AOE_SUPPORTED
#define AOE_SUPPORTED                             0
#endif

/** 
COE_SUPPORTED: If the CoE services are supported, then his switch shall be set. */
#ifndef COE_SUPPORTED
#define COE_SUPPORTED                             1
#endif

/** 
COMPLETE_ACCESS_SUPPORTED: If the complete SDO access (accessing all entries of an object with one SDO service, then this<br>
switch shall be set. Furthermore,COE_SUPPORTED shall be set. */
#ifndef COMPLETE_ACCESS_SUPPORTED
#define COMPLETE_ACCESS_SUPPORTED                 1
#endif

/** 
SEGMENTED_SDO_SUPPORTED: If the segmented SDO services should be supported, then this switch shall be set.<br>
Furthermore, COE_SUPPORTED shall be set. */
#ifndef SEGMENTED_SDO_SUPPORTED
#define SEGMENTED_SDO_SUPPORTED                   1
#endif

/** 
SDO_RES_INTERFACE: If a SDO response cannot be generated immediately (e.g. when access over<br>
a serial interface is needed), this switch should be set. In that case ABORTIDX_WORKING shall be<br>
returned from OBJ_Read or OBJ_Write and the response shall be sent by calling SDOS_SdoRes, when<br>
the response is available. */
#ifndef SDO_RES_INTERFACE
#define SDO_RES_INTERFACE                         1
#endif

/** 
USE_SINGLE_PDO_MAPPING_ENTRY_DESCR: If this setting is set a PDO mapping entry description just need to be defined for the first entry. For all furher entries the same description is used. */
#ifndef USE_SINGLE_PDO_MAPPING_ENTRY_DESCR
#define USE_SINGLE_PDO_MAPPING_ENTRY_DESCR        0
#endif

/** 
BACKUP_PARAMETER_SUPPORTED: If this switch is set, then the functions in the application example to load and<br>
store backup parameter will be compiled. Furthermore, COE_SUPPORTED shall be set. */
#ifndef BACKUP_PARAMETER_SUPPORTED
#define BACKUP_PARAMETER_SUPPORTED                0
#endif

/** 
STORE_BACKUP_PARAMETER_IMMEDIATELY: Objet values will be stored when they are written.This switch is only evaluated if "BACKUP_PARAMETER_SUPPORTED" is set. */
#ifndef STORE_BACKUP_PARAMETER_IMMEDIATELY
#define STORE_BACKUP_PARAMETER_IMMEDIATELY        0
#endif

/** 
DIAGNOSIS_SUPPORTED: If this define is set the slave stack supports diagnosis messages (Object 0x10F3). <br>
To support diagnosis messages COE_SUPPORTED shall be enabled and the platform shall support dynamic memory allocation.<br>
NOTE: this feature is implemented according to ETG.1020 */
#ifndef DIAGNOSIS_SUPPORTED
#define DIAGNOSIS_SUPPORTED                       0
#endif

/** 
EMERGENCY_SUPPORTED: If this define is set the slave stack supports emergency messages. COE_SUPPORTED or SOE_SUPPORTED shall be enabled */
#ifndef EMERGENCY_SUPPORTED
#define EMERGENCY_SUPPORTED                       0
#endif

/** 
VOE_SUPPORTED: If the VoE services should be supported, then this switch shall be set. This means only the calling of the <br>
VoE functions in mailbox.c are implemented, but the VoE service functions have to be added. Furthermore, the example code cannot be linked<br>
correctly, because these functions are missing. */
#ifndef VOE_SUPPORTED
#define VOE_SUPPORTED                             0
#endif

/** 
SOE_SUPPORTED: If the SoE services should be supported, then this switch shall be set. This means only the calling of the <br>
SoE functions in mailbox.c are implemented, but the SoE service functions have to be added. Furthermore, the example code cannot be linked<br>
correctly, because these functions are missing. */
#ifndef SOE_SUPPORTED
#define SOE_SUPPORTED                             0
#endif

/** 
EOE_SUPPORTED: If the EoE services should be supported, then this switch shall be set. */
#ifndef EOE_SUPPORTED
#define EOE_SUPPORTED                             0
#endif

/** 
STATIC_ETHERNET_BUFFER: If this switch is set a static buffer is used to store ethernet frames, otherwise the buffer is allocated on demand */
#ifndef STATIC_ETHERNET_BUFFER
#define STATIC_ETHERNET_BUFFER                    0
#endif

/** 
FOE_SUPPORTED: If the FoE services should be supported, then this switch shall be set.  */
#ifndef FOE_SUPPORTED
#define FOE_SUPPORTED                             0
#endif

/** 
FOE_SAVE_FILES: If set incoming Files are stored in "aFileData" max file size is set by MAX_FILE_SIZE. */
#ifndef FOE_SAVE_FILES
#define FOE_SAVE_FILES                            0
#endif

/** 
MAILBOX_SUPPORTED: This switch is set automatically if at least one mailbox protocol is enabled. */
#ifndef MAILBOX_SUPPORTED
#if !COE_SUPPORTED && !AOE_SUPPORTED && !EOE_SUPPORTED && !FOE_SUPPORTED && !SOE_SUPPORTED && !VOE_SUPPORTED
#define MAILBOX_SUPPORTED                         0
#else
#define MAILBOX_SUPPORTED                         1
#endif
#endif

/** 
BOOTSTRAPMODE_SUPPORTED: If the firmware update over FoE services should be supported, then this switch shall be set.<br>
If this switch is set, then also "FOE_SUPPORTED" shall be set.  */
#ifndef BOOTSTRAPMODE_SUPPORTED
#define BOOTSTRAPMODE_SUPPORTED                   0
#endif

/** 
OP_PD_REQUIRED: If this switch is reset the state transition SAFEOP_2_OP will also successful if no process data was received. The watchdog will only be active when first process data was received (bEcatFirstOutputsReceived) */
#ifndef OP_PD_REQUIRED
#define OP_PD_REQUIRED                            1
#endif

/** 
PREOPTIMEOUT: Specify timeout value for the state transition from Init to PreOP/Boot.(ESI Value : "PreopTimeout").<br>
NOTE: Within the stack this value - 50ms will be used to react before the master run into the timeout. */
#ifndef PREOPTIMEOUT
#define PREOPTIMEOUT                              0x7D0
#endif

/** 
SAFEOP2OPTIMEOUT: Specifiy the timeout from SafeOP to OP. (ESI Value : "SafeopOpTimeout")<br>
NOTE: Within the stack this value - 50ms will be used to react before the master run into the timeout. */
#ifndef SAFEOP2OPTIMEOUT
#define SAFEOP2OPTIMEOUT                          0x2328
#endif

/** 
EXPLICIT_DEVICE_ID: If this switch is set Explicit device ID requests are handled. For further information about Explicit Device ID see ETG.1020 specification: www.ethercat.org/MemberArea/download_protocolenhancements.asp */
#ifndef EXPLICIT_DEVICE_ID
#define EXPLICIT_DEVICE_ID                        0
#endif

/** 
ESC_SM_WD_SUPPORTED: This switch should be set if the SyncManger watchdog provided by the ESC should be used. If reset the process data watchdog is triggered by a local timer */
#ifndef ESC_SM_WD_SUPPORTED
#define ESC_SM_WD_SUPPORTED                       1
#endif

/** 
STATIC_OBJECT_DIC: If this switch is set, the object dictionary is "build" static (by default only PIC18 objects are added static) */
#ifndef STATIC_OBJECT_DIC
#define STATIC_OBJECT_DIC                         0
#endif

/** 
ESC_EEPROM_ACCESS_SUPPORT: If this switch is set the slave stack provides functions to access the EEPROM. */
#ifndef ESC_EEPROM_ACCESS_SUPPORT
#define ESC_EEPROM_ACCESS_SUPPORT                 0
#endif


/*-----------------------------------------------------------------------------------------
------	
------	Compiler Defines
------	
-----------------------------------------------------------------------------------------*/

/** 
FALSE: Will be used for variables from type BOOL */
#ifndef FALSE
#define FALSE                                     0
#endif

/** 
TRUE: Will be used for variables from type BOOL  */
#ifndef TRUE
#define TRUE                                      1
#endif

/** 
BOOL: Should be adapted to the boolean type of the microcontroller */
#ifndef BOOL
#define BOOL                                      unsigned char
#endif

/** 
UINT8: Should be adapted to the unsigned8 type of the microcontroller  */
#ifndef UINT8
#define UINT8                                     unsigned char
#endif

/** 
UINT16: Should be adapted to the unsigned16 type of the microcontroller  */
#ifndef UINT16
#define UINT16                                    unsigned short
#endif

/** 
UINT32: Should be adapted to the unsigned32 type of the microcontroller  */
#ifndef UINT32
#define UINT32                                    unsigned long
#endif

/** 
USHORT: Should be adapted to the unsigned16 type of the microcontroller */
#ifndef USHORT
#define USHORT                                    unsigned short
#endif

/** 
INT8: Should be adapted to the integer8 type of the microcontroller */
#ifndef INT8
#define INT8                                      char
#endif

/** 
INT16: Should be adapted to the integer16 type of the microcontroller  */
#ifndef INT16
#define INT16                                     short
#endif

/** 
INT32: Should be adapted to the integer32 type of the microcontroller */
#ifndef INT32
#define INT32                                     long
#endif

/** 
CHAR: Should be adapted to the character type of the microcontroller */
#ifndef CHAR
#define CHAR                                      char
#endif

/** 
UCHAR: Should be adapted to the unsigned character type of the microcontroller */
#ifndef UCHAR
#define UCHAR                                     unsigned char
#endif

/** 
SIZEOF(x): Used to calculate the size in Bytes */
#ifndef SIZEOF
#define SIZEOF(x)                                 sizeof(x)
#endif

/** 
HUGE: Should be adapted to the huge type of the microcontroller, if the microcontroller<br>
does not support a huge type, HUGE shall be defined to nothing */
#ifndef HUGE
#define HUGE 
#endif

/** 
HMEMSET: Should be defined to the memset function for huge memory, if the microcontroller<br>
does not support a huge type, HMEMSET shall be defined to a 'normal' memset function */
#ifndef HMEMSET
#define HMEMSET                                   memset
#endif

/** 
HMEMCPY: Should be defined to the memcpy function for huge memory, if the microcontroller<br>
does not support a huge type, HMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef HMEMCPY
#define HMEMCPY                                   memcpy
#endif

/** 
HMEMCMP: Should be defined to the memcmp function for huge memory, if the microcontroller<br>
does not support a huge type, HMEMCMP shall be defined to a 'normal' memcmp function */
#ifndef HMEMCMP
#define HMEMCMP                                   memcmp
#endif

/** 
ESCMEM: Should be defined to select the memory type of the ESC memory (e.g. near, far or huge), if the microcontroller<br>
does not support different memory types, ESCMEM shall be defined to nothing */
#ifndef ESCMEM
#define ESCMEM 
#endif

/** 
ESCMEMCPY: Should be defined to the memcpy function for ESCMEM memory, if the microcontroller<br>
does not support different memory types, ESCMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef ESCMEMCPY
#define ESCMEMCPY                                 memcpy
#endif

/** 
ESCMEMSET: Should be defined to the memset function for ESCMEM memory, if the microcontroller<br>
does not support different memory types, ESCMEMSET shall be defined to a 'normal' memset function */
#ifndef ESCMEMSET
#define ESCMEMSET                                 memset
#endif

/** 
ESCMBXMEMCPY: Should be defined to the memcpy function for copying ESCMEM memory to or from MBXMEM memory, if the microcontroller<br>
does not support different memory types, ESCMBXMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef ESCMBXMEMCPY
#define ESCMBXMEMCPY                              memcpy
#endif

/** 
MBXMEM: Should be defined to select the memory type of the memory used for mailbox communication (e.g. near, far or huge), <br>
if the microcontroller does not support different memory types, MBXMEM shall be defined to nothing */
#ifndef MBXMEM
#define MBXMEM
#endif

/** 
MBXMEMCPY: Should be defined to the memcpy function for MBXMEM memory, if the microcontroller<br>
does not support different memory types, MBXMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef MBXMEMCPY
#define MBXMEMCPY                                 memcpy
#endif

/** 
MBXMEMCMP: Should be defined to the memcmp function for MBXMEM memory, if the microcontroller<br>
does not support different memory types, MBXMEMCMP shall be defined to a 'normal' memcmp function */
#ifndef MBXMEMCMP
#define MBXMEMCMP                                 memcmp
#endif

/** 
MBXMEMSET: Should be defined to the memcpy function for MBXMEM memory, if the microcontroller<br>
does not support different memory types, MBXMEMSET shall be defined to a 'normal' memset function */
#ifndef MBXMEMSET
#define MBXMEMSET                                 memset
#endif

/** 
MBXSTRLEN: Should be defined to the strlen function for MBXMEM memory, if the microcontroller<br>
does not support different memory types, MBXSTRLEN shall be defined to a 'normal' strlen function */
#ifndef MBXSTRLEN
#define MBXSTRLEN                                 strlen
#endif

/** 
MBXSTRCPY: Should be defined to the strcpy function for MBXMEM memory, if the microcontroller<br>
does not support different memory types, MBXSTRCPY shall be defined to a 'normal' strcpy function */
#ifndef MBXSTRCPY
#define MBXSTRCPY                                 memcpy
#endif

/** 
OBJCONST: Should be used to define the object dictionary in ROM (f.e. define OBJCONST const) or<br>
in RAM (e.g. define OBJCONST) */
#ifndef OBJCONST
#define OBJCONST                                  const
#endif

/** 
VARCONST: Should be used to define the constant variables in ROM (f.e. define VARCONST const) or<br>
in RAM (e.g. define VARCONST) */
#ifndef VARCONST
#define VARCONST 
#endif

/** 
VARVOLATILE: Should be used to prevent dummy variables to be deleted due to compiler optimization.  */
#ifndef VARVOLATILE
#define VARVOLATILE                               volatile
#endif

/** 
OBJMEM: Should be defined to select the memory type of the memory used for the object dictionary (e.g. near, far or huge), <br>
if the microcontroller does not support different memory types, OBJMEM shall be defined to nothing */
#ifndef OBJMEM
#define OBJMEM 
#endif

/** 
OBJTOMBXMEMCPY: Should be defined to the memcpy function for copying OBJMEM memory to MBXMEM memory, if the microcontroller<br>
does not support different memory types, OBJTOMBXMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef OBJTOMBXMEMCPY
#define OBJTOMBXMEMCPY                            memcpy
#endif

/** 
OBJTOMBXSTRCPY: Should be defined to the strcpy function for copying OBJMEM memory to MBXMEM memory, if the microcontroller<br>
does not support different memory types, OBJTOMBXSTRCPY shall be defined to a 'normal' memcpy function */
#ifndef OBJTOMBXSTRCPY
#define OBJTOMBXSTRCPY                            memcpy
#endif

/** 
OBJMEMCPY: Should be defined to the memcpy function for OBJMEM memory, if the microcontroller<br>
does not support different memory types, OBJMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef OBJMEMCPY
#define OBJMEMCPY                                 memcpy
#endif

/** 
OBJSTRLEN: Should be defined to the strlen function for OBJMEM memory, if the microcontroller<br>
does not support different memory types, OBJSTRLEN shall be defined to a 'normal' strlen function */
#ifndef OBJSTRLEN
#define OBJSTRLEN                                 strlen
#endif

/** 
OBJSTRCPY: Should be defined to the strcpy function for OBJMEM memory, if the microcontroller<br>
does not support different memory types, OBJSTRCPY shall be defined to a 'normal' strcpy function */
#ifndef OBJSTRCPY
#define OBJSTRCPY                                 memcpy
#endif

/** 
MAKE_HUGE_PTR: Should be defined to the initialize a pointer variable with an absolute address */
#ifndef MAKE_HUGE_PTR
#if !_PIC24
#define MAKE_HUGE_PTR                             _mkhp
#else
#define MAKE_HUGE_PTR 
#endif
#endif

/** 
MAKE_PTR_TO_ESC: Should be defined to the initialize the pointer to the ESC */
#ifndef MAKE_PTR_TO_ESC
#if MCI_HW && !FC1100_HW
#define MAKE_PTR_TO_ESC                           &sEsc
#else
#define MAKE_PTR_TO_ESC 
#endif
#endif

/** 
EMCYMEMCPY: Should be defined to the memcpy function for EMCYMEM memory, if the microcontroller<br>
does not support different memory types, EMCYMEMCPY shall be defined to a 'normal' memcpy function */
#ifndef EMCYMEMCPY
#define EMCYMEMCPY                                memcpy
#endif

/** 
EMCYMEMSET: Should be defined to the memset function for EMCYMEM memory, if the microcontroller<br>
does not support different memory types, EMCYMEMSET shall be defined to a 'normal' memcset function */
#ifndef EMCYMEMSET
#define EMCYMEMSET                                memset
#endif

/** 
EMCYMEM: Should be defined to select the memory type of the memory used for the emergencies (e.g. near, far or huge), <br>
if the microcontroller does not support different memory types, EMCYMEM shall be defined to nothing */
#ifndef EMCYMEM
#define EMCYMEM 
#endif

/** 
MEMCPY: Should be defined to copy data within local memory. */
#ifndef MEMCPY
#define MEMCPY                                    memcpy
#endif

/** 
ALLOCMEM(size): Should be defined to the alloc function to get dynamic memory */
#ifndef ALLOCMEM
#define ALLOCMEM(size)                            malloc((size))
#endif

/** 
FREEMEM(pointer): Should be defined to the free function to put back dynamic memory */
#ifndef FREEMEM
#define FREEMEM(pointer)                          free((pointer))
#endif

/** 
VARMEMSET: Should be defined to the memset function for VARMEM memory, if the microcontroller<br>
does not support different memory types, EMCYMEMSET shall be defined to a 'normal' memcset function */
#ifndef VARMEMSET
#define VARMEMSET                                 memset
#endif

/** 
VARMEM: Should be defined to select the memory type of the memory used for dynamic memory (e.g. near, far or huge), <br>
if the microcontroller does not support different memory types, VARMEM shall be defined to nothing */
#ifndef VARMEM
#define VARMEM 
#endif

/** 
MEM_ADDR: Type to access local memory addresses */
#ifndef MEM_ADDR
#if CONTROLLER_32BIT
#define MEM_ADDR                                  UINT32
#elif CONTROLLER_16BIT
#define MEM_ADDR                                  UINT16
#else
#define MEM_ADDR                                  UINT8
#endif
#endif

/** 
GET_MEM_SIZE(ByteSize): Round up the byte size to next matching memory boundary depending on "MEM_ADDR" */
#ifndef GET_MEM_SIZE
#if CONTROLLER_32BIT
#define GET_MEM_SIZE(ByteSize)                    (((ByteSize)+3) >> 2)
#elif CONTROLLER_16BIT
#define GET_MEM_SIZE(ByteSize)                    (((ByteSize)+1) >> 1)
#else
#define GET_MEM_SIZE(ByteSize)                    (ByteSize)
#endif
#endif

/** 
APPL_AllocMailboxBuffer(size): Should be defined to a function to get a buffer for a mailbox service,<br>
this is only used if the switch MAILBOX_QUEUE is set */
#ifndef APPL_AllocMailboxBuffer
#if MAILBOX_QUEUE
#define APPL_AllocMailboxBuffer(size)             malloc((size))
#else
#define APPL_AllocMailboxBuffer(size) 
#endif
#endif

/** 
APPL_FreeMailboxBuffer(pointer): Should be defined to a function to put back a buffer for a mailbox service,<br>
this is only used if the switch MAILBOX_QUEUE is set */
#ifndef APPL_FreeMailboxBuffer
#if MAILBOX_QUEUE
#define APPL_FreeMailboxBuffer(pointer)           free((pointer))
#else
#define APPL_FreeMailboxBuffer(pointer) 
#endif
#endif

/** 
STRUCT_PACKED_START: Is defined before the typedef struct construct to pack the generic structures if necessary */
#ifndef STRUCT_PACKED_START
#define STRUCT_PACKED_START 
#endif

/** 
STRUCT_PACKED_END: Is defined after the typedef struct {} construct to pack the generic structures if necessary */
#ifndef STRUCT_PACKED_END
#define STRUCT_PACKED_END 
#endif

/** 
MBX_STRUCT_PACKED_START: Is defined before the typedef struct construct to pack the MAILBOX structures if necessary */
#ifndef MBX_STRUCT_PACKED_START
#define MBX_STRUCT_PACKED_START 
#endif

/** 
MBX_STRUCT_PACKED_END: Is defined after the typedef struct {} construct to pack the MAILBOX structures if necessary */
#ifndef MBX_STRUCT_PACKED_END
#define MBX_STRUCT_PACKED_END 
#endif

/** 
OBJ_STRUCT_PACKED_START: Is defined before the typedef struct construct to pack the OBJECT structures if necessary */
#ifndef OBJ_STRUCT_PACKED_START
#define OBJ_STRUCT_PACKED_START 
#endif

/** 
OBJ_STRUCT_PACKED_END: Is defined after the typedef struct {} construct to pack the OBJECT structures if necessary */
#ifndef OBJ_STRUCT_PACKED_END
#define OBJ_STRUCT_PACKED_END 
#endif

/** 
OBJ_DWORD_ALIGN: Shall be set if the object structures are not Byte aligned and the Code is executed on an 32bit platform */
#ifndef OBJ_DWORD_ALIGN
#if CONTROLLER_32BIT
#define OBJ_DWORD_ALIGN                           1
#else
#define OBJ_DWORD_ALIGN                           0
#endif
#endif

/** 
OBJ_WORD_ALIGN: Shall be set if the object structures are not Byte aligned and the Code is executed on an 16bit platform */
#ifndef OBJ_WORD_ALIGN
#if CONTROLLER_16BIT
#define OBJ_WORD_ALIGN                            1
#else
#define OBJ_WORD_ALIGN                            0
#endif
#endif


/*-----------------------------------------------------------------------------------------
------	
------	Application Specific Defines
------	
-----------------------------------------------------------------------------------------*/

/** 
MAX_DIAG_MSG: Number of diagnosis message ringbuffer */
#ifndef MAX_DIAG_MSG
#define MAX_DIAG_MSG                              0x14
#endif

/** 
MAX_EMERGENCIES: Number of emergencies supported in parallel */
#ifndef MAX_EMERGENCIES
#define MAX_EMERGENCIES                           0x1
#endif

/** 
MAX_FILE_SIZE: Maximum file size */
#ifndef MAX_FILE_SIZE
#define MAX_FILE_SIZE                             0x180
#endif

/** 
VENDOR_ID: Object 0x1018 SI1 (Vendor ID)	 <br>
An unique EtherCAT Vendor ID is required. Please find all valid Vendor IDs listed at www.ethercat.org/en/vendor_id_list.html.<br>
If your company is not listed, please assign an ID for free at www.ethercat.org/memberarea/vendor_id.asp */
#ifndef VENDOR_ID
#define VENDOR_ID                                 0xE0000002
#endif

/** 
PRODUCT_CODE: Object 0x1018 SI2 (EtherCAT product code) */
#ifndef PRODUCT_CODE
#if FC1100_HW
#define PRODUCT_CODE                              0x044C0C62
#elif EL9800_HW
#define PRODUCT_CODE                              0x26483052
#else
#define PRODUCT_CODE                              0x00000000
#endif
#endif

/** 
REVISION_NUMBER: Object 0x1018 SI3 (EtherCAT product revision number) */
#ifndef REVISION_NUMBER
#if CiA402_DEVICE
#define REVISION_NUMBER                           0x00020211
#elif EL9800_APPLICATION && _PIC18
#define REVISION_NUMBER                           0x00010101
#elif EL9800_APPLICATION && _PIC24
#define REVISION_NUMBER                           0x00020111
#elif FC1100_HW
#define REVISION_NUMBER                           0x00020300
#elif MCI_HW
#define REVISION_NUMBER                           0x00010082
#elif TEST_APPLICATION  && EL9800_HW
#define REVISION_NUMBER                           0x00010411
#else
#define REVISION_NUMBER                           0x00000000
#endif
#endif

/** 
SERIAL_NUMBER: Object 0x1018 SI4 (EtherCAT product serial number) */
#ifndef SERIAL_NUMBER
#define SERIAL_NUMBER                             0x00000000
#endif

/** 
DEVICE_PROFILE_TYPE: Slave device type (Object 0x1000)  */
#ifndef DEVICE_PROFILE_TYPE
#if CiA402_DEVICE
#define DEVICE_PROFILE_TYPE                       0x00020192
#else
#define DEVICE_PROFILE_TYPE                       0x00001389
#endif
#endif

/** 
DEVICE_NAME: Name of the slave device (Object 0x1008) */
#ifndef DEVICE_NAME
#define DEVICE_NAME                               "SSC-Device"
#endif

/** 
DEVICE_NAME_LEN: Length of 'DEVICE_NAME' without '\0' */
#ifndef DEVICE_NAME_LEN
#define DEVICE_NAME_LEN                           0xA
#endif

/** 
DEVICE_HW_VERSION: Hardware version of the slave device (Object 0x1009) */
#ifndef DEVICE_HW_VERSION
#define DEVICE_HW_VERSION                         "n.a."
#endif

/** 
DEVICE_HW_VERSION_LEN: Length of 'DEVICE_HW_VERSION' without '\0' */
#ifndef DEVICE_HW_VERSION_LEN
#define DEVICE_HW_VERSION_LEN                     0x4
#endif

/** 
DEVICE_SW_VERSION: Software version of the slave device (Object 0x100A) */
#ifndef DEVICE_SW_VERSION
#define DEVICE_SW_VERSION                         "5.11"
#endif

/** 
DEVICE_SW_VERSION_LEN: Length of 'DEVICE_SW_VERSION' without '\0' */
#ifndef DEVICE_SW_VERSION_LEN
#define DEVICE_SW_VERSION_LEN                     0x4
#endif

/** 
MIN_PD_WRITE_ADDRESS: Minimum address for the process output data (Sync Manager 2)<br>
inside the application memory of the EtherCAT Slave Controller which could be set by the master. The setting have to be within the ranges of the user memory of the ESC (this is not checked by the tool). */
#ifndef MIN_PD_WRITE_ADDRESS
#define MIN_PD_WRITE_ADDRESS                      0x1000
#endif

/** 
MAX_PD_WRITE_ADDRESS: Maximum address for the process output data (Sync Manager 2)<br>
inside the application memory of the EtherCAT Slave Controller which could be set by the master. The setting have to be within the ranges of the user memory of the ESC (this is not checked by the tool). */
#ifndef MAX_PD_WRITE_ADDRESS
#define MAX_PD_WRITE_ADDRESS                      0x2FFF
#endif

/** 
MIN_PD_READ_ADDRESS: Minimum address for the process input data (Sync Manager 3)<br>
inside the application memory of the EtherCAT Slave Controller which could be set by the master. The setting have to be within the ranges of the user memory of the ESC (this is not checked by the tool). */
#ifndef MIN_PD_READ_ADDRESS
#define MIN_PD_READ_ADDRESS                       0x1000
#endif

/** 
MAX_PD_READ_ADDRESS: Maximum address for the process input data (Sync Manager 3)<br>
inside the application memory of the EtherCAT Slave Controller which could be set by the master. The setting have to be within the ranges of the user memory of the ESC (this is not checked by the tool). */
#ifndef MAX_PD_READ_ADDRESS
#define MAX_PD_READ_ADDRESS                       0x2FFF
#endif

/** 
MIN_MBX_SIZE: Minimum mailbox size (Sync Manager 0 and 1) which could be set by the master. The SSC don't support fragmented SDO info object/entry service => at least entry info including 12byte name shall fit in the mailbox buffer */
#ifndef MIN_MBX_SIZE
#define MIN_MBX_SIZE                              0x0022
#endif

/** 
MAX_MBX_SIZE: Maximum mailbox size (Sync Manager 0 and 1) which could be set by the master. */
#ifndef MAX_MBX_SIZE
#define MAX_MBX_SIZE                              0x0100
#endif

/** 
MIN_MBX_WRITE_ADDRESS: Minimum address for the write (receive) mailbox (Sync Manager 0). The setting have to be within the ranges of the user memory of the ESC (this is not checked by the tool). */
#ifndef MIN_MBX_WRITE_ADDRESS
#define MIN_MBX_WRITE_ADDRESS                     0x1000
#endif

/** 
MAX_MBX_WRITE_ADDRESS: Maximum address for the write (receive) mailbox (Sync Manager 0). The setting have to be within the ranges of the user memory of the ESC (this is not checked by the tool). */
#ifndef MAX_MBX_WRITE_ADDRESS
#define MAX_MBX_WRITE_ADDRESS                     0x2FFF
#endif

/** 
MAX_PD_INPUT_SIZE: Maximum size of the process input data (Sync Manager 3) for cyclic exchange. */
#ifndef MAX_PD_INPUT_SIZE
#define MAX_PD_INPUT_SIZE                         0x0044
#endif

/** 
MIN_MBX_READ_ADDRESS: Minimum address for the read (send) mailbox (Sync Manager 1). */
#ifndef MIN_MBX_READ_ADDRESS
#define MIN_MBX_READ_ADDRESS                      0x1000
#endif

/** 
MAX_MBX_READ_ADDRESS: Maximum address for the read (send) mailbox (Sync Manager 1). */
#ifndef MAX_MBX_READ_ADDRESS
#define MAX_MBX_READ_ADDRESS                      0x2FFF
#endif

/** 
MAX_PD_OUTPUT_SIZE: Maximum size of the process output data (Sync Manager 2) for cyclic exchange. */
#ifndef MAX_PD_OUTPUT_SIZE
#define MAX_PD_OUTPUT_SIZE                        0x044
#endif

/** 
MIN_PD_CYCLE_TIME: Minimum cycle time in ns the slave is supporting <br>
(entry 0x1C32:05 or entry 0x1C33:05) */
#ifndef MIN_PD_CYCLE_TIME
#define MIN_PD_CYCLE_TIME                         0x7A120
#endif

/** 
MAX_PD_CYCLE_TIME: Maximum cycle time in ns the slave is supporting */
#ifndef MAX_PD_CYCLE_TIME
#define MAX_PD_CYCLE_TIME                         0xC3500000
#endif

/** 
PD_OUTPUT_DELAY_TIME: Minimum output delay time in ns the slave is supporting (entry 0x1C32:09) */
#ifndef PD_OUTPUT_DELAY_TIME
#define PD_OUTPUT_DELAY_TIME                      0x0
#endif

/** 
PD_OUTPUT_CALC_AND_COPY_TIME: Output calc+copy time in ns the slave is supporting (entry 0x1C32:06) */
#ifndef PD_OUTPUT_CALC_AND_COPY_TIME
#define PD_OUTPUT_CALC_AND_COPY_TIME              0x0
#endif

/** 
PD_INPUT_CALC_AND_COPY_TIME: Input calc+copy time in ns the slave is supporting (entry 0x1C33:06) */
#ifndef PD_INPUT_CALC_AND_COPY_TIME
#define PD_INPUT_CALC_AND_COPY_TIME               0x0
#endif

/** 
PD_INPUT_DELAY_TIME: Input delay time in ns the slave is supporting (entry 0x1C33:09) */
#ifndef PD_INPUT_DELAY_TIME
#define PD_INPUT_DELAY_TIME                       0x0
#endif



/*-----------------------------------------------------------------------------------------
------	
------	Test Configuration Defines
------	
-----------------------------------------------------------------------------------------*/

#if EL9800_HW && !_PIC24 && !_PIC18
#warning EL9800_HW is enabled and no PIC is selected => the SPI connector TP204/J204 shall be used to connect to host controller. If the onboard MCI connector (TP208/J208) is required enable MCI_HW
#endif

#if FC1100_HW && AL_EVENT_ENABLED
#error The FC1100_HW configuration does not support PDI interrupt by default
#endif

#if _PIC18 && _PIC24
#error Select only one PIC
#endif

#if _PIC18 && !STATIC_OBJECT_DIC
#error The PIC18 configuration requires a static object dictionary
#endif

#if _PIC18 && !EL9800_HW
#error EL9800_HW shall be enabled if PIC18 is set (confiuration for EL9800_2)
#endif

#if _PIC24 && !EL9800_HW
#error EL9800_HW shall be enabled if PIC24 is set (confiuration for EL9800_4)
#endif

#if EXT_DEBUGER_INTERFACE && !_PIC24
#warning The define "EXT_DEBUGER_INTERFACE" is only required for EL9800 PIC24 platforms
#endif

#if ECAT_TIMER_INT && _PIC18
#error The PIC18 configuration does not support timer interrupts by default
#endif

#if DC_SUPPORTED && (_PIC18 || FC1100_HW)
#error The configuration for "FC1100_HW" and "_PIC18" does not support DC synchronisation by default
#endif

#if UC_SET_ECAT_LED && _PIC18
#error The PIC18 platform (EL9800_2) does not support status LEDs
#endif

#if SEGMENTED_SDO_SUPPORTED && _PIC18
#error The PIC18 configuration does not support segmented SDO
#endif

#if MAILBOX_QUEUE && _PIC18
#error The PIC18 configuration does not support mailbox queue
#endif

#if EL9800_APPLICATION && !EL9800_HW
#error EL9800_HW shall be enabled if EL9800_Application is set
#endif

#if (EL9800_APPLICATION + CiA402_DEVICE  + TEST_APPLICATION + SAMPLE_APPLICATION + SAMPLE_APPLICATION_INTERFACE)  > 1
#error Select only one application.
#endif

#if TEST_APPLICATION
#warning Use this application ONLY FOR TESTING PURPOSE.
#endif

#if VOE_SUPPORTED
#warning VoE mailbox functions and structs need to be implemented and defined
#endif

#if BOOTSTRAPMODE_SUPPORTED && !FOE_SUPPORTED
#error FoE shall be enabled, if Bootstrap state is supported 
#endif

#if AOE_SUPPORTED && !MAILBOX_QUEUE
#error AoE requires mailbox queue support
#endif

#if DIAGNOSIS_SUPPORTED && _PIC18
#error PIC18 configuration does not support diagnosis messages
#endif

#if DIAGNOSIS_SUPPORTED &&  !COE_SUPPORTED
#error Diagnosis history requires CoE support
#endif

#if EMERGENCY_SUPPORTED && !COE_SUPPORTED && !SOE_SUPPORTED
#error CoE or SoE shall be enabled if Emergency support is set
#endif

#if FOE_SAVE_FILES && !FOE_SUPPORTED
#error FoE shall be enabled if FoE save files is set
#endif

#if CiA402_DEVICE && !COE_SUPPORTED
#error CoE shall be enabled if CiA402Device set
#endif

#if COMPLETE_ACCESS_SUPPORTED && !COE_SUPPORTED
#error CoE shall be enabled if complete access is set
#endif

#if SEGMENTED_SDO_SUPPORTED && !COE_SUPPORTED
#error CoE shall be enabled if segmented SDO access is set
#endif

#if SDO_RES_INTERFACE && !COE_SUPPORTED
#error CoE shall be enabled if SDO response interface is set
#endif

#if BACKUP_PARAMETER_SUPPORTED && !COE_SUPPORTED
#error CoE shall be enabled if Backup Parameter is set
#endif

#if DIAGNOSIS_SUPPORTED && !COE_SUPPORTED
#error CoE shall be enabled if diagnosis history (0x10F3) is set
#endif

#if MAILBOX_QUEUE && !MAILBOX_SUPPORTED
#error Disable mailbox queue if if no mailbox protocol is supported.
#endif

#if (EL9800_HW && (MCI_HW || FC1100_HW)) || (MCI_HW && (EL9800_HW || FC1100_HW)) || (FC1100_HW && (EL9800_HW || MCI_HW))
#error Select only one PDI / platform. See setting description for detailed information.
#endif

#if DIAGNOSIS_SUPPORTED && STATIC_OBJECT_DIC
#error If Diagnosis History is enabled the object dictionary shall handled dynamically.
#endif

#if CiA402_DEVICE && SOE_SUPPORTED
#error Select only one drive profile.
#endif

#if CONTROLLER_16BIT && CONTROLLER_32BIT
#error Select only one controller architecture.
#endif

#if ESC_16BIT_ACCESS && ESC_32BIT_ACCESS
#error Select only one ESC access width.
#endif

#if UC_SET_ECAT_LED && ESC_SUPPORT_ECAT_LED
#error Either the uController OR the ESC shall set the EtherCAT LEDs. Disable "UC_SET_ECAT_LED" or "ESC_SUPPORT_ECAT_LED"
#endif

#if STATIC_OBJECT_DIC && !EL9800_APPLICATION
#error The static object dictionary is only available for the EL9800_APPLICATION. 
#endif

#if ESC_EEPROM_ACCESS_SUPPORT && ESC_EEPROM_EMULATION
#error Either EEPROM emulation or ESC EEPROM access shall be defined. In case that EEPROM emulation is enabled the stack does not need to acess the EEPROM via the ESC.
#endif


#endif // _ECATDEF_H_

