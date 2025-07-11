/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file do2blockvars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/do2blockvars.c $
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
#include "ff_do2blockvars.h"
#include "param.h"
#include "numutils.h"
// -------------------------------- "ipc variables" management section ------------------------


// The initial values should be obtained from FFP every time when APP firmware starts
// This is just a temp solution
static IPC_FFDO2Params_t IPC_FFDO2Params =
{
    {FF_DO2_TAG_DEF},
    0,
    0,
    {0, 0},
    CRC_SEED, //make valid checksum by C init
};

// get the pointer of DO2 block variables
IPC_FFDO2Params_t* GetDo2BlockVar(void)
{
    return &IPC_FFDO2Params;
}

ErrorCode_t  IPC_WriteDO2Tag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pDOTagSegment = IPC_FFDO2Params.tag;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_DO2_TAG_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pDOTagSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_DO2_TAG_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pDOTagSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pDOTagSegment, FF_DO2_TAG_MAX_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_DO2_TAG_WR_MAX_BLOCK_NUM, pData);
         }

         STRUCT_CLOSE(IPC_FFDO2Params_t, &IPC_FFDO2Params);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}

ErrorCode_t  IPC_WriteDO2Mode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFDO2Params.ModeActual = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFDO2Params_t, &IPC_FFDO2Params);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFDO2Params.ModeActual);
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

ErrorCode_t  IPC_WriteDO2Error(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFDO2Params.BlockErr = (u16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFDO2Params_t, &IPC_FFDO2Params);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFDO2Params.BlockErr);
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

ErrorCode_t  IPC_WriteDO2SPD(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        util_PutU8Array(pIPC_ReadPtrs->pIPC_VarBuffer, sizeof(float32), pIPC_WritePtrs->pIPC_VarBuffer);

        /* The actual limits happen to be 0 to 65535*/
        IPC_FFDO2Params.sp_d.value = (s16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        IPC_FFDO2Params.sp_d.status = util_GetU8(pIPC_WritePtrs->pVarStatus);
        STRUCT_CLOSE(IPC_FFDO2Params_t, &IPC_FFDO2Params);
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
