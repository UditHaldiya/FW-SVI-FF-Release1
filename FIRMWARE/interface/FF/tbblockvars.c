/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file tbblockvars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/tbblockvars.c $
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
#include "ff_tbblockvars.h"
#include "param.h"
// -------------------------------- "ipc variables" management section ------------------------

/** Default data */
//static const u8 def_TBBlockTag[FF_TB_TAG_MAX_LEN] = {FF_TB_TAG_DEF};


// The initial values should be obtained from FFP every time when APP firmware starts
// This is just a temp solution
static IPC_FFTBParams_t IPC_FFTBParams =
{
    {FF_TB_TAG_DEF},
    0,
    0,
    {{FLOAT_STRING_INIT}, 0},
    {{FLOAT_STRING_INIT}, 0},
    0,
    0,
    CRC_SEED, //make valid checksum by C init
};

// get the pointer of TB block variables
IPC_FFTBParams_t* GetTbBlockVar(void)
{
     return &IPC_FFTBParams;
}

ErrorCode_t  IPC_WriteTBTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pTBTagSegment = IPC_FFTBParams.tag;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_TB_TAG_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pTBTagSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_TB_TAG_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pTBTagSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pTBTagSegment, FF_TB_TAG_MAX_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_TB_TAG_WR_MAX_BLOCK_NUM, pData);
         }

         STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}

ErrorCode_t  IPC_WriteTBMode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFTBParams.ModeActual = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFTBParams.ModeActual);
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

ErrorCode_t  IPC_WriteTBError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFTBParams.BlockErr = (u16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFTBParams.BlockErr);
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

ErrorCode_t  IPC_WriteTBFinalSP(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    util_PutU8Array(IPC_FFTBParams.working_sp.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
    IPC_FFTBParams.working_sp.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);

    STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));

    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteTBFinalPOS(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    util_PutU8Array(IPC_FFTBParams.working_pos.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
    IPC_FFTBParams.working_pos.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);

    STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));

    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteTBUICustConfig0(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFTBParams.cust_config_0 = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFTBParams.cust_config_0);
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

ErrorCode_t  IPC_WriteTBUICustConfig1(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFTBParams.cust_config_1 = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFTBParams_t, &IPC_FFTBParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_FFTBParams.cust_config_1);
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


