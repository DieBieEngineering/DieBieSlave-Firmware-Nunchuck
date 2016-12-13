/**
 * \addtogroup AoE ADS over EtherCAT
 * @{
 */

/**
\file	aoeappl.h
\author EthercatSSC@beckhoff.com

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

#ifndef _AOEAPPL_H_

#define _AOEAPPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecataoe.h"


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/


#endif //_AOEAPPL_H_

/* ECATCHANGE_START(V5.11) ECAT10*/
#if defined(_AOEAPPL_) && (_AOEAPPL_ == 1)
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


/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO UINT16    AOEAPPL_AmsInd(AmsCmd MBXMEM *pCmd);

#undef PROTO
/** @}*/
