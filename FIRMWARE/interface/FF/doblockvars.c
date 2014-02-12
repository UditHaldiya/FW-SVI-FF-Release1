/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file DOblockvars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/DOblockvars.c $
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
#include "ff_doblockvars.h"
#include "param.h"
#include "numutils.h"
// -------------------------------- "ipc variables" management section ------------------------

/** Default data */
//static const u8 def_DOBlockTag[FF_DO_TAG_MAX_LEN] = {FF_DO_TAG_DEF};


// The initial values should be obtained from FFP every time when APP firmware starts
// This is just a temp solution
static IPC_FFDOParams_t IPC_FFDOParams =
{
    {FF_DO_TAG_DEF},
    0,
    0,
    {0, 0},
    CRC_SEED, //make valid checksum by C init
};

// get the pointer of DO block variables
IPC_FFDOParams_t* GetDoBlockVar(void)
{
    return &IPC_FFDOParams;
}

ErrorCode_t  IPC_WriteDOTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pDOTagSegment = IPC_FFDOParams.tag;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_DO_TAG_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pDOTagSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_DO_TAG_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pDOTagSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pDOTagSegment, FF_DO_TAG_MAX_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_DO_TAG_WR_MAX_BLOCK_NUM, pData);
         }

         STRUCT_CLOSE(IPC_FFDOParams_t, &IPC_FFDOParams);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}

ErrorCode_t  IPC_WriteDOMode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFDOParams.ModeActual = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFDOParams_t, &IPC_FFDOParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFDOParams.ModeActual);
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

ErrorCode_t  IPC_WriteDOError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFDOParams.BlockErr = (u16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFDOParams_t, &IPC_FFDOParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFDOParams.BlockErr);
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

ErrorCode_t  IPC_WriteDOSPD(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        util_PutU8Array(pIPC_ReadPtrs->pIPC_VarBuffer, sizeof(float32), pIPC_WritePtrs->pIPC_VarBuffer);

        /* The actual limits happen to be 0 to 65535*/
        IPC_FFDOParams.sp_d.value = (s16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        IPC_FFDOParams.sp_d.status = util_GetU8(pIPC_WritePtrs->pVarStatus);
        STRUCT_CLOSE(IPC_FFDOParams_t, &IPC_FFDOParams);
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
