/**
\addtogroup AoE ADS over EtherCAT
@{
*/

/**
\file aoeappl.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the AoE mailbox interface.

\version 5.11

<br>Changes to version V5.01:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
<br>Changes to version V4.40:<br>
V5.01 MBX1: Allocate always complete 16Bit memory areas<br>
<br>Changes to version V4.10:<br>
V4.40 COE1: Prototype of function OBJ_GetObjectList() changed, (abortCode added)<br>
<br>Changes to version V4.08:<br>
V4.10 AOEAPPL: fix some issues with the CoE Objects access over AoE
*/


/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#if AOE_SUPPORTED

#include "ecatslv.h"

#if COE_SUPPORTED
#include "objdef.h"
#endif

#define    _AOEAPPL_    1
#include "aoeappl.h"
#undef      _AOEAPPL_
/* ECATCHANGE_START(V5.11) ECAT10*/
/*remove definition of _AOEAPPL_ (#ifdef is used in aoeappl.h)*/
/* ECATCHANGE_END(V5.11) ECAT10*/

/*---------------------------------------------------------------------------------------
------
------    internal Types and Defines
------
---------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
------
------    local variables
------
---------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
------
------    local functions
------
---------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
------
------    functions
------
---------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pCmd      Pointer to the AMS commands

 \brief    This function is called when a AMS indication shall be routed
*////////////////////////////////////////////////////////////////////////////////////////

UINT16    AOEAPPL_AmsInd(AmsCmd MBXMEM *pCmd)
{
    UINT16 result = 0;

    if ( pCmd->head.cmdId == ROUTERCMD_FRAGMENT )
    {
        /* fragmented AMS command */
        pCmd = AOE_FragmentedCmdInd(pCmd);
        if (pCmd == NULL)
        {
            return ERR_NOERROR;
        }
    }

    if (AOE_AmsNetIdsEqual(&pCmd->head.target.netId, &sRouterNetId))
    {
        /* Router is addressed, check if the port is valid */
        UINT16 port = SWAPWORD(pCmd->head.target.port);
        UINT16 cmdId = SWAPWORD(pCmd->head.cmdId);

        /* only port 65535 (for the device itself) is valid in this example */
        if ( port == 65535 )
        {
            /* we only support read and write service in this example */
            if ( cmdId == ADSSRVID_READ || cmdId == ADSSRVID_WRITE )
            {
                TAdsReadReq MBXMEM * pReq = (TAdsReadReq MBXMEM *) pCmd;
#if COE_SUPPORTED
                TAdsReadRes MBXMEM * pRes = (TAdsReadRes MBXMEM *) pCmd;
#endif
                UINT32 dataSize = SWAPDWORD(pCmd->head.cbData);
                UINT32 indexGroup = SWAPDWORD(pReq->indexGroup);
#if COE_SUPPORTED
                UINT32 indexOffset = SWAPDWORD(pReq->indexOffset);
#endif
                UINT32 dataLen = 0;

                /* we have to check the size */
                if ( ( cmdId == ADSSRVID_READ && dataSize == (SIZEOF(TAdsReadReq) - SIZEOF(AmsHead)) )
                    ||( cmdId == ADSSRVID_WRITE && dataSize >= (SIZEOF(TAdsWriteReq) - SIZEOF(AmsHead)) )
                    )
                {
                    /*read requested dateSize*/
                    dataSize = pReq->cbLength;
                    /* the index group 0xF302 is defined for CoE services,
                        that is the only index group we are supporting in this example */
                    switch ( indexGroup )
                    {
#if COE_SUPPORTED
                    case ADSIGRP_CANOPEN_SDO: /* SDO-Request */
                        /* index offset, Bit 16-31: index
                            index offset, Bit 8: complete access
                            index offset, Bit 0-7: subindex */
                        if ( (indexOffset & 0x0000FE00) == 0 )
                        {
                            UINT16 index = (UINT16) (indexOffset >> 16);
                            UINT8 subindex = (UINT8) (indexOffset & 0xFF);
                            UINT8 bCompleteAccess = (UINT8) ((indexOffset >> 8) & 0x01);
                            OBJCONST TOBJECT OBJMEM * pObjEntry = OBJ_GetObjectHandle( index );

                            if ( pObjEntry )
                            {
                                dataLen = OBJ_GetObjectLength( index, subindex, pObjEntry, bCompleteAccess );
                                if ( dataLen > 0 )
                                {
                                    if ( dataSize >= dataLen )
                                    {
                                        if ( cmdId == ADSSRVID_READ )
                                        {
                                            /* get buffer for response */
                                            pRes = (TAdsReadRes *) ALLOCADSMEM(ROUNDUPBYTE2WORD(SIZEOF(TAdsReadRes) + dataLen));
                                            if ( pRes != NULL )
                                            {
                                                /* copy AMS header */
                                                HMEMCPY(pRes, pReq, SIZEOF(AmsHead));
                                                result = OBJ_Read( index, subindex, dataLen, pObjEntry, (UINT16 MBXMEM *) &pRes[1], bCompleteAccess );
                                                if ( result == 0 )
                                                {
                                                    // send AoE response
                                                    pRes->cbLength = dataLen;
                                                    pRes->result = 0;
                                                    AOE_AmsRes((AmsCmd MBXMEM *) pRes, result,(UINT16) dataLen+SIZEOF_TAdsReadRes);
                                                    return result;
                                                }
                                                else
                                                    result = ADSERR_DEVICE_ERROR;
                                            }
                                            else
                                                result = ADSERR_DEVICE_NOMEMORY;
                                        }
                                        else
                                        {
                                            /* request buffer is big enough for response */
                                            result = OBJ_Write( index, subindex, dataLen, pObjEntry, (UINT16 MBXMEM *) &pReq[1], bCompleteAccess );
                                            if ( result == 0 )
                                            {
                                                // send AoE response
                                                pRes->cbLength = dataLen;
                                                pRes->result = 0;
                                                AOE_AmsRes((AmsCmd MBXMEM *) pRes, result,(UINT16) dataLen+SIZEOF_TAdsWriteRes);
                                                return result;
                                            }
                                            else
                                                result = ADSERR_DEVICE_ERROR;
                                        }
                                    }
                                    else
                                        result = ADSERR_DEVICE_INVALIDSIZE;
                                }
                                else
                                    result = ADSERR_DEVICE_NOTFOUND;
                            }
                            else
                                result = ADSERR_DEVICE_NOTFOUND;
                        }
                        else
                            /* index offset is  not valid */
                            result = ADSERR_DEVICE_INVALIDOFFSET;
                        break;
                    case ADSIGRP_CANOPEN_SDO_INFO_LIST:
                        if ( indexOffset == 0 )
                        {
                            /* List 0 (length of lists 1-5) */
                            if ( dataSize >= (INFO_LIST_TYPE_MAX<<1) )
                            {
                                /* get buffer for response */
                                pRes = (TAdsReadRes MBXMEM *) ALLOCADSMEM( SIZEOF(TAdsReadRes) + (INFO_LIST_TYPE_MAX<<1)+SIZEOF_SDOINFOLISTHEAD );
                                /* ListType in Response */
                                ((UINT16 MBXMEM *) &pRes[1])[0] = 0;

                                if ( pRes != NULL )
                                {
                                    UINT8 i;
                                    /* copy AMS header */
                                    HMEMCPY(pRes, pReq, SIZEOF(AmsHead));
                                    for (i = 0; i < INFO_LIST_TYPE_MAX; i++)
                                    {
                                        UINT16 n = OBJ_GetNoOfObjects(i);

                                        /* copy the number of objects of the list type in the SDO Information response */
                                        ((UINT16 MBXMEM *) &pRes[1])[i+1] = SWAPWORD(n);
                                    }

                                    // send AoE response
                                    dataLen = (INFO_LIST_TYPE_MAX<<1) + SIZEOF_SDOINFOLISTHEAD;
                                    pRes->cbLength = dataLen;
                                    pRes->result = 0;
                                    AOE_AmsRes((AmsCmd MBXMEM *) pRes, result,(UINT16) dataLen+SIZEOF_TAdsReadRes);
                                    return result;
                                }
                                else
                                    result = ADSERR_DEVICE_NOMEMORY;
                            }
                            else
                                result = ADSERR_DEVICE_INVALIDSIZE;
                        }
                        else if ( (indexOffset >> 16) <= INFO_LIST_TYPE_MAX )
                        {
                            UINT32 n = OBJ_GetNoOfObjects((UINT8) ((indexOffset >> 16) - 1));

                            if ( dataSize >= (n<<1) )
                            {
                                /* get buffer for response */
                                pRes = (TAdsReadRes MBXMEM *) ALLOCADSMEM(SIZEOF(TAdsReadRes) + (n<<1) + SIZEOF(UINT16) + SIZEOF_SDOINFOLISTHEAD);
                                if ( pRes != NULL )
                                {
                                    TSDOINFOLIST MBXMEM *pListRes = (TSDOINFOLIST MBXMEM *) &pRes[1];
                                    UINT16 MBXMEM *pList = (UINT16 MBXMEM *) &pListRes[1];
                                    // start with the first possible index
                                    UINT16 index = 0x1000;

                                    /* copy AMS header */
                                    HMEMCPY(pRes, pReq, SIZEOF(AmsHead));
                                    pListRes->ListType = (indexOffset >> 16);
                                    OBJ_GetObjectList(pListRes->ListType-1, &index, (UINT16) (n<<1), pList,NULL);
                                    // send AoE response
                                    dataLen = (n<<1) + SIZEOF_SDOINFOLISTHEAD;
                                    pRes->cbLength = dataLen;
                                    pRes->result = 0;
                                    AOE_AmsRes((AmsCmd MBXMEM *) pRes, result,(UINT16) dataLen+SIZEOF_TAdsReadRes);
                                    return result;
                                }
                                else
                                    result = ADSERR_DEVICE_NOMEMORY;
                            }
                            else
                                result = ADSERR_DEVICE_INVALIDSIZE;
                        }
                        else
                            /* index offset is  not valid */
                            result = ADSERR_DEVICE_INVALIDOFFSET;
                        break;
                    case ADSIGRP_CANOPEN_SDO_INFO_OBJ:
                        if ( (indexOffset & 0xFFFF0000L) != 0 )
                        {
                            UINT16 index = (UINT16) (indexOffset >> 16);
                            OBJCONST TOBJECT OBJMEM * pObjEntry = OBJ_GetObjectHandle( index );

                            if ( pObjEntry )
                            {
                                dataLen = OBJ_GetDesc(index, 0, pObjEntry, NULL) + SDO_INFO_OBJ_DESC_RES_SIZE;
                                if ( dataSize >= SDO_INFO_OBJ_DESC_RES_SIZE )
                                {
                                    if ( dataSize < dataLen )
                                        /* buffer too small for description */
                                        dataLen = SDO_INFO_OBJ_DESC_RES_SIZE;
                                    /* get buffer for response */
                                    pRes = (TAdsReadRes *) ALLOCADSMEM(SIZEOF(TAdsReadRes) + dataLen);
                                    if ( pRes != NULL )
                                    {
                                        TSDOINFOOBJ MBXMEM *pObj = (TSDOINFOOBJ MBXMEM *) &pRes[1];

                                        /* copy AMS header */
                                        HMEMCPY(pRes, pReq, SIZEOF(AmsHead));

                                        pObj->Index = SWAPWORD(index);
#if BIG_ENDIAN_FORMAT
                                        {
                                        OBJCONST TSDOINFOOBJDESC OBJMEM *pObjDesc = OBJ_GetObjDesc(pObjEntry);
                                        pObj->Res.DataType = SWAPWORD(pObjDesc->DataType);
                                        pObj->Res.ObjFlags = SWAPWORD(pObjDesc->ObjFlags);
                                        }
#else
                                        OBJTOMBXMEMCPY(&pObj->Res, OBJ_GetObjDesc(pObjEntry), SDO_INFO_OBJ_DESC_SIZE);
#endif
                                        if ( dataLen > SDO_INFO_OBJ_DESC_RES_SIZE )
                                        {
                                            OBJ_GetDesc(index, 0, pObjEntry, ((UINT16 MBXMEM *) &(&pObj->Res)[1]));
                                        }

                                        // send AoE response
                                        pRes->cbLength = dataLen;
                                        pRes->result = 0;
                                        AOE_AmsRes((AmsCmd MBXMEM *) pRes, result,(UINT16) dataLen+SIZEOF_TAdsReadRes);
                                        return result;
                                    }
                                    else
                                        result = ADSERR_DEVICE_NOMEMORY;
                                }
                                else
                                    result = ADSERR_DEVICE_INVALIDSIZE;
                            }
                            else
                                result = ADSERR_DEVICE_NOTFOUND;
                        }
                        else
                            /* index offset is  not valid */
                            result = ADSERR_DEVICE_INVALIDOFFSET;
                        break;
                    case ADSIGRP_CANOPEN_SDO_INFO_ENTRY:
                        /*    Index (Request) Index Offset, Bit 16-31
                            Subindex (Request) Index Offset, Bit 0-7
                            Value info (Request) Index Offset, Bit 8-15 */
                        if ( (indexOffset & 0xFFFF00FFL) != 0 )
                        {
                            UINT16 index   = (UINT16) (indexOffset >> 16);
                            UINT8 subindex = (UINT8) (indexOffset & 0xFF);
                            OBJCONST TOBJECT OBJMEM * pObjEntry = OBJ_GetObjectHandle( index );

                            if ( pObjEntry )
                            {
                                /* get the maximum subindex */
                                UINT8 maxSubindex = (OBJ_GetObjDesc(pObjEntry)->ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;

                                if ( subindex <= maxSubindex )
                                {
                                    dataLen = OBJ_GetDesc(index, subindex, pObjEntry, NULL) + SIZEOF(TSDOINFOENTRY);
                                    if ( dataSize >= SIZEOF(TSDOINFOENTRY) )
                                    {
                                        if ( dataSize < dataLen )
                                            /* buffer too small for description */
                                            dataLen = SIZEOF(TSDOINFOENTRY);
                                        /* get buffer for response */
                                        pRes = (TAdsReadRes *) ALLOCADSMEM(SIZEOF(TAdsReadRes) + dataLen);
                                        if ( pRes != NULL )
                                        {
                                            TSDOINFOENTRY MBXMEM *pEntry = (TSDOINFOENTRY MBXMEM *) &pRes[1];

                                            /* copy AMS header */
                                            HMEMCPY(pRes, pReq, SIZEOF(AmsHead));
                                            pEntry->Index = SWAPWORD(index);
                                            ((UINT16 MBXMEM *) &pRes[1])[1] = (UINT16) indexOffset;
#if BIG_ENDIAN_FORMAT
                                            {
                                            OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntryDesc = OBJ_GetEntryDesc(pObjEntry, subindex);
                                            pEntry->Res.DataType = SWAPWORD(pEntryDesc->DataType);
                                            pEntry->Res.BitLength = SWAPWORD(pEntryDesc->BitLength);
                                            pEntry->Res.ObjAccess = SWAPWORD(pEntryDesc->ObjAccess);
                                            }
#else
                                            OBJTOMBXMEMCPY(&pEntry->Res, OBJ_GetEntryDesc(pObjEntry, subindex), SIZEOF(TSDOINFOENTRYDESC));
#endif
                                            /* the transmission of the value info is not supported yet of the sample code */
                                            pEntry->Info &= ~ENTRY_MASK_VALUEINFO;

                                            if ( dataLen > SDO_INFO_OBJ_DESC_RES_SIZE )
                                            {
                                                OBJ_GetDesc(index, subindex, pObjEntry, ((UINT16 MBXMEM *) &(&pEntry->Res)[1]));
                                            }

                                            // send AoE response
                                            pRes->cbLength = dataLen;
                                            pRes->result = 0;
                                            AOE_AmsRes((AmsCmd MBXMEM *) pRes, result,(UINT16) dataLen+SIZEOF_TAdsReadRes);
                                            return result;
                                        }
                                        else
                                            result = ADSERR_DEVICE_NOMEMORY;
                                    }
                                    else
                                        result = ADSERR_DEVICE_INVALIDSIZE;
                                }
                                else
                                    result = ADSERR_DEVICE_NOTFOUND;
                            }
                            else
                                result = ADSERR_DEVICE_NOTFOUND;
                        }
                        else
                            /* index offset is  not valid */
                            result = ADSERR_DEVICE_INVALIDOFFSET;
                        break;
#endif //#if COE_SUPPORTED
                    default:
                        /* index group is not valid */
                        result = ADSERR_DEVICE_INVALIDGRP;
                        break;
                    }
                }
                else
                    /* cbData is not valid */
                    result = ADSERR_DEVICE_INVALIDSIZE;

                // AoE response with error
                if ( cmdId == ADSSRVID_READ )
                    dataLen = SIZEOF_TAdsReadRes;
                else
                    dataLen = SIZEOF_TAdsWriteRes;

                ((TAdsReadRes MBXMEM *)pCmd)->cbLength = 0;
                ((TAdsReadRes MBXMEM *)pCmd)->result = result;
                AOE_AmsRes(pCmd, 0,(UINT16) dataLen);
                return result;
            }
            else
                /* command is not valid */
                result = ADSERR_DEVICE_SRVNOTSUPP;
        }
        else
        {
            /* port not valid */
            result = ERR_TARGETPORTNOTFOUND;
        }

    }
    else
        /* Net-ID is not valid */
        result = ERR_TARGETMACHINENOTFOUND;

    AOE_AmsRes(pCmd, result, 0);

    return result;
}

#endif /* AOE_SUPPORTED */

/** @}*/
