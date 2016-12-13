/**
 * \addtogroup FoE FileTransfer over EtherCAT
 * @{
 */

/**
\file foeappl.h
\author EthercatSSC@beckhoff.com

\version 5.01

<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _FOEAPPL_H_
#define _FOEAPPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

#endif //_FOEAPPL_H_

#ifdef _FOEAPPL_
    #define PROTO
#else
    #define PROTO extern
#endif


/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/
PROTO    UINT16     FOE_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT16 MBXMEM * pData, UINT32 password);
PROTO    UINT16     FOE_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password);
PROTO    UINT16     FOE_Data(UINT16 MBXMEM * pData, UINT16 Size);
PROTO    UINT16     FOE_Ack(UINT32 fileOffset, UINT16 MBXMEM * pData);
PROTO    UINT16     FOE_Busy(UINT16 done, UINT32 fileOffset, UINT16 MBXMEM * pData);
PROTO    void       FOE_Error(UINT32 errorCode);

#undef PROTO
/** @}*/
