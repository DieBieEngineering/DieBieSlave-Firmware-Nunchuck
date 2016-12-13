/**
\addtogroup ESM EtherCAT State Machine
@{
*/

/**
\file bootmode.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 4.20

<br>Changes to version - :<br>
V4.20: File created
*/

/*--------------------------------------------------------------------------------------
------
------    Includes
------
--------------------------------------------------------------------------------------*/
#include "ecat_def.h"
#if BOOTSTRAPMODE_SUPPORTED

#define    _BOOTMODE_ 1
#include "bootmode.h"
#undef _BOOTMODE_

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    State        Current state

 \brief Dummy BL_Start function
*////////////////////////////////////////////////////////////////////////////////////////
void BL_Start( UINT8 State)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    password    download password

 \brief Dummy BL_StartDownload function
*////////////////////////////////////////////////////////////////////////////////////////
void BL_StartDownload(UINT32 password)
{
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pData    Data pointer
 \param    Size    Data Length


 \return    FoE error code

 \brief Dummy BL_Data function
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 BL_Data(UINT16 *pData,UINT16 Size)
{
    return 0;
}
#endif //BOOTSTRAPMODE_SUPPORTED
/** @} */
