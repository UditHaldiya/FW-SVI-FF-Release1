/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file AOblockvars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/AOblockvars.c $
    $Date: 08/23/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

    \ingroup FF
*/

#include "mnwrap.h"
#include "errcodes.h"
#include "faultpublic.h"
#include "numutils.h"
#include "bufutils.h"
#include "fpconvert.h"
#include "hart.h"
#include "devicemode.h"
#include "ipcdefs.h"
#include "ipcvarids.h"
#include "ff_aoblockvars.h"
#include "param.h"
// -------------------------------- "ipc variables" management section ------------------------

/** Default data */
//static const u8 def_AOBlockTag[FF_AO_TAG_MAX_LEN] = {FF_AO_TAG_DEF};


// The initial values should be obtained from FFP every time when APP firmware starts
// This is just a temp solution
static IPC_FFAOParams_t IPC_FFAOParams =
{
    {FF_AO_TAG_DEF},
    0,
    0,
    {{FLOAT_STRING_INIT}, 0},
    CRC_SEED, //make valid checksum by C init
};

// get the pointer of AO block variables
IPC_FFAOParams_t* GetAoBlockVar(void)
{
     return &IPC_FFAOParams;
}

ErrorCode_t  IPC_WriteAOTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pAOTagSegment = IPC_FFAOParams.tag;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_AO_TAG_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pAOTagSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_AO_TAG_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pAOTagSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pAOTagSegment, FF_AO_TAG_MAX_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_AO_TAG_WR_MAX_BLOCK_NUM, pData);
         }

         STRUCT_CLOSE(IPC_FFAOParams_t, &IPC_FFAOParams);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}

ErrorCode_t  IPC_WriteAOMode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFAOParams.ModeActual = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFAOParams_t, &IPC_FFAOParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFAOParams.ModeActual);
    }
    else
    {
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteAOError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFAOParams.BlockErr = (u16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFAOParams_t, &IPC_FFAOParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFAOParams.BlockErr);
    }
    else
    {
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);

    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteAOSP(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
	ErrorCode_t     retval = ERR_OK;

	util_PutU8Array(IPC_FFAOParams.sp.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
	IPC_FFAOParams.sp.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));

    STRUCT_CLOSE(IPC_FFAOParams_t, &IPC_FFAOParams);
    UNUSED_OK(VarID);
    return retval;
}
