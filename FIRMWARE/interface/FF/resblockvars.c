/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file resblockvars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/resblockvars.c $
    $Date: 05/06/12 11:19a $
    $Revision: 1 $
    $Author: ericj $

    \ingroup FF
*/
/* $History: resblockvars.c $
 *
 *
 * *****************  Version 1  *****************
 * User: ericj    Date: 05/06/12    Time: 10:38p
 * Created in $/MNCB/Dev/FIRMWARE/interface/FF
 * ipc vars handdler
*/

#include "mnwrap.h"
#include "errcodes.h"
#include "faultpublic.h"
#include "numutils.h"
#include "bufutils.h"
#include "fpconvert.h"
#include "hart.h"
#include "devicemode.h"
#include "ffdefs.h"
#include "ipcdefs.h"
#include "ipcvarids.h"
#include "ff_resblockvars.h"
// -------------------------------- "ipc variables" management section ------------------------

/** Default data */
static const u8 def_ResBlockTag[FF_RB_TAG_MAX_LEN] = {FF_RB_TAG_DEF};


// The initial values should be obtained from FFP every time when APP firmware starts
// This is just a temp solution
static IPC_FFResourceParams_t IPC_FFResourceParams =
{
    {FF_RB_TAG_DEF},
    0,
    1,
    1,
    0,
    CRC_SEED, //make valid checksum by C init
};

static IPC_FFResourceParams_t * const pIPC_FFResourceParams = &IPC_FFResourceParams;

// get the pointer of resource block variables
const IPC_FFResourceParams_t* GetResBlockVar(void)
{
     return pIPC_FFResourceParams;
}


//lint, remove static in the future if need the interface
static ErrorCode_t SetResBlockTag(const void *src)
{
    const u8* pCChanged = src;
    if(pCChanged == NULL)
    {
        pCChanged = def_ResBlockTag;
    }

    util_PutU8Array((u8*)pIPC_FFResourceParams->tag, FF_RB_TAG_MAX_LEN, pCChanged);
    STRUCT_CLOSE(IPC_FFResourceParams_t, pIPC_FFResourceParams);

    return ERR_OK;
}

/* needn't this interface for the finial release, user can check by LUI
ErrorCode_t IPC_ReadRbTag(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t         retval = ERR_OK;
    u8                  DataBlockNumber = pIPC_ReadPtrs->IPC_DataBlockNum;

    if (DataBlockNumber > FF_RB_TAG_RD_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
        Struct_Test(IPC_FFResourceParams_t, pIPC_FFResourceParams);
        util_PutU8Array(pIPC_ReadPtrs->pIPC_VarBuffer, FF_RB_TAG_MAX_LEN, pIPC_FFResourceParams->tag);
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);
    return retval;
}
*/
ErrorCode_t IPC_WriteRbTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pRBTagSegment = pIPC_FFResourceParams->tag;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_RB_TAG_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pRBTagSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_RB_TAG_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pRBTagSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pRBTagSegment, FF_RB_TAG_MAX_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_RB_TAG_WR_MAX_BLOCK_NUM, pData);
         }

         retval = SetResBlockTag(pIPC_FFResourceParams->tag);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}


ErrorCode_t  IPC_ReadRbRestart(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    //temp, RESTART will be back to 1 after the completion of restart.
    //util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 1.0F);
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 1);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    UNUSED_OK(VarID);
    return retval;
}


ErrorCode_t  IPC_WriteRbRestart(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t retval = ERR_OK;
    u8      ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    u8      restart = 1;

    // Check the Variable quality and write the var ONLY if the quality is accepteable!
    if (IsQuality_ACCEPT(*(pIPC_WritePtrs->pVarStatus)))
    {   // OK, Write the data

        restart = (u8)(u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        if ((restart < IPC_RESTART_VALUE_MIN) || (restart > IPC_RESTART_VALUE_MAX))
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_INCONSISTENT;
        }
        else
        {
            switch(restart)
            {
                case IPC_RESTART_PROCESSOR:
                    hart_ResetDeviceOnSend();
                    break;
                default:
                    break;
            }
            restart = 1;
        }
    }
    else
    {   // Quality is not acceptable. DO not write the data, signal back.
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        restart = 0u;
    }

    //(void)fpconvert_IntToFloatBuffer((s32)restart, UNITSID_DUMMY, pIPC_ReadPtrs->pIPC_VarBuffer);
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, restart);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_ReadFaultState(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
#ifdef OLD_DEVMODE
    const ModeData_t* pMode;

    pMode = mode_GetMode();

    if ((pMode->mode == MODE_OOS) && (pMode->submode == SUBMODE_OOS_FAILSAFE))
#else
    if (mode_GetMode() == MODE_FAILSAFE)
#endif //OLD_DEVMODE
    {
      util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, FF_FAULT_STATE_ACTIVE);
    }
    else
    {
      util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, FF_FAULT_STATE_CLEAR);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    UNUSED_OK(VarID);
    return retval;
}


ErrorCode_t  IPC_ReadSetFaultState(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->SetFstate);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    UNUSED_OK(VarID);
    return retval;
}


ErrorCode_t  IPC_WriteSetFaultState(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    u8              SetFstateTarget = FF_SET_FSTATE_OFF;

    if (IsQuality_ACCEPT(*(pIPC_WritePtrs->pVarStatus)))
    {
        SetFstateTarget = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);

        // The checking may be done in FFP. If so, remove
        if (isSetFstate_DEFINED(SetFstateTarget))
        {
            if (SetFstateTarget == FF_SET_FSTATE_SET)
            {
                error_SetFault(FAULT_NONE); //invent a failsafe fault
                ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR; // default value, can remove
                pIPC_FFResourceParams->SetFstate = SetFstateTarget; // may remove. do write
            }
            else if(SetFstateTarget == FF_SET_FSTATE_OFF)
            {
                ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR; // default value, can remove
                pIPC_FFResourceParams->SetFstate = SetFstateTarget; // may remove. do write
            }
            else
            {
            }
        }
        else
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_OUT_OF_ENUM;
        }
    }
    else
    {
       ReturnStatus = *pIPC_WritePtrs->pVarStatus;
    }

    //pIPC_FFResourceParams->SetFstate = FF_SET_FSTATE_OFF; // may remove. back to OFF

    if (IsQuality_ACCEPT(ReturnStatus))
    {
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->SetFstate);
    }
    else
    {
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_ReadClearFaultState(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->ClrFstate);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    UNUSED_OK(VarID);
    return retval;
}


ErrorCode_t  IPC_WriteClearFaultState(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    u8              ClrFstateTarget = FF_CLR_FSTATE_OFF;

    if (IsQuality_ACCEPT(*(pIPC_WritePtrs->pVarStatus)))
    {
        ClrFstateTarget = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);

        // The checking may be done in FFP. If so, remove
        if (isClrFstate_DEFINED(ClrFstateTarget))
        {
            if (ClrFstateTarget == FF_CLR_FSTATE_CLR)
            {
                error_ClearFaults(false); //TBD
                if(error_IsAnyFaultWithAttributes(FATTRIB_FAILSAFE))
                {
                    ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_STATE_CONFLICT;
                }
                else
                {
                    ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR; // default value, can remove
                    pIPC_FFResourceParams->ClrFstate = ClrFstateTarget; // may remove. do write
                }
            }
            else if(ClrFstateTarget == FF_CLR_FSTATE_OFF)
            {
                    ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR; // default value, can remove
                    pIPC_FFResourceParams->ClrFstate = ClrFstateTarget; // may remove. do write
            }
            else
            {
            }
        }
        else
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_OUT_OF_ENUM;
        }
    }
    else
    {
       ReturnStatus = *pIPC_WritePtrs->pVarStatus;
    }

    //pIPC_FFResourceParams->ClrFstate = FF_CLR_FSTATE_OFF; // may remove. back to OFF

    if (IsQuality_ACCEPT(ReturnStatus))
    {
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->ClrFstate);
    }
    else
    {
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_ReadRbWrongCfg(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    UNUSED_OK(VarID);
    return retval;
}


ErrorCode_t  IPC_ReadRbBlockErrDesc1(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    UNUSED_OK(VarID);
    return retval;
}

/* needn't this interface, the data can checked by LUI
ErrorCode_t  IPC_ReadRbModeActual(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;

    Struct_Test(IPC_FFResourceParams_t, pIPC_FFResourceParams);
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->ModeActual);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);

    UNUSED_OK(VarID);
    return retval;
}
*/
ErrorCode_t  IPC_WriteRbModeActual(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ActualMode;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        ActualMode  = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        storeMemberU8(pIPC_FFResourceParams, ModeActual, ActualMode);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->ModeActual);
        util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    }
    else
    {
        u8  ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
        util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    }

    UNUSED_OK(VarID);
    return retval;
}

ErrorCode_t  IPC_WriteRbError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    u16              BlockErr;

    u8 varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        BlockErr  = (u16)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        storeMemberU16(pIPC_FFResourceParams, BlockErr, BlockErr);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, pIPC_FFResourceParams->BlockErr);
        util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    }
    else
    {
        u8  ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
        util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    }

    UNUSED_OK(VarID);
    return ERR_OK;
}

/* This line marks the end of the source */
