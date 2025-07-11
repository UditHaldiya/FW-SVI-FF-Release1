/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file AI2blockvars.c
    \brief "ipc variables" handler

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/positionConfVars.c $
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
#include "ff_positionConfVars.h"
#include "param.h"
#include "nvram.h"
// -------------------------------- "ipc variables" management section ------------------------

/* \brief this function will be called when IPC_STOP_HI_POS is written
    param in:
        VarID: variable ID
        pIPC_WritePtrs: write buffer
        pIPC_ReadPtrs: read buffer
    return:
        error code
*/
ErrorCode_t  IPC_WriteStopHiPos(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    PositionConf_t  pc;

    (void)pos_GetPositionConf(&pc);
    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        pc.rangeval[Xhi] = util_GetS32(pIPC_WritePtrs->pIPC_VarBuffer);
        ErrorCode_t err = pos_SetPositionConf(&pc);

        if(err != ERR_OK)
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_OUT_OF_RANGE;
            retval = ERR_INVALID_PARAMETER;
            util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, 0);
        }
        else
        {
            util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, pc.rangeval[Xhi]);
        }
    }
    else
    {   // Not accepteable quality - return the BAD status
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, 0);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return retval;
}

/* \brief this function will be called when IPC_STOP_LO_POS is written
    param in:
        VarID: variable ID
        pIPC_WritePtrs: write buffer
        pIPC_ReadPtrs: read buffer
    return:
        error code
*/
ErrorCode_t  IPC_WriteStopLoPos(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    PositionConf_t pc;

    (void)pos_GetPositionConf(&pc);
    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        pc.rangeval[Xlow] = util_GetS32(pIPC_WritePtrs->pIPC_VarBuffer);
        ErrorCode_t err = pos_SetPositionConf(&pc);

        if(err != ERR_OK)
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_OUT_OF_RANGE;
            util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, 0);
            retval = ERR_INVALID_PARAMETER;
        }
        else
        {
            util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, pc.rangeval[Xlow]);
        }
    }
    else
    {   // Not accepteable quality - return the BAD status
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, 0);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return retval;
}
