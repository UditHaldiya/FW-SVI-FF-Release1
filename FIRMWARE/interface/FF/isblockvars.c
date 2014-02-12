/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file isblockvars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/isblockvars.c $
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
#include "ff_isblockvars.h"
#include "param.h"
// -------------------------------- "ipc variables" management section ------------------------


// The initial values should be obtained from FFP every time when APP firmware starts
// This is just a temp solution
static IPC_FFISParams_t IPC_FFISParams =
{
    {{FLOAT_STRING_INIT}, 0},
    {{FLOAT_STRING_INIT}, 0},
    {{FLOAT_STRING_INIT}, 0},
    {{FLOAT_STRING_INIT}, 0},
    CRC_SEED, //make valid checksum by C init
};

// get the pointer of IS block variables
IPC_FFISParams_t* GetIsBlockVar(void)
{
     return &IPC_FFISParams;
}

ErrorCode_t  IPC_WriteISIN1(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
	ErrorCode_t     retval = ERR_OK;
	
	util_PutU8Array(IPC_FFISParams.in1.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
	IPC_FFISParams.in1.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
	
    STRUCT_CLOSE(IPC_FFISParams_t, &IPC_FFISParams);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteISIN2(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
	ErrorCode_t     retval = ERR_OK;
    
	util_PutU8Array(IPC_FFISParams.in2.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
	IPC_FFISParams.in2.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
	
    STRUCT_CLOSE(IPC_FFISParams_t, &IPC_FFISParams);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteISIN3(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
	ErrorCode_t     retval = ERR_OK;

	util_PutU8Array(IPC_FFISParams.in3.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
	IPC_FFISParams.in3.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
	
    STRUCT_CLOSE(IPC_FFISParams_t, &IPC_FFISParams);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteISIN4(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
	ErrorCode_t     retval = ERR_OK;
	
	util_PutU8Array(IPC_FFISParams.in4.value, FLOAT_STRING_LEN - 1, pIPC_WritePtrs->pIPC_VarBuffer);
	IPC_FFISParams.in4.status = util_GetU8(((const u8*)(pIPC_WritePtrs->pIPC_VarBuffer)) + FLOAT_STRING_LEN - 1);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
	
    STRUCT_CLOSE(IPC_FFISParams_t, &IPC_FFISParams);
    UNUSED_OK(VarID);
    return retval;
}
