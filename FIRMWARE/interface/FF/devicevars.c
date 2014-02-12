/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file devicevars.c
    \brief "ipc variables" handler (borrowed from the trunk's nvram.c)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/FF/devicevars.c $
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
#include "ff_devicevars.h"
#include "param.h"
#include "mncbdefs_ffap.h"
#include "nvram.h"
// -------------------------------- "ipc variables" management section ------------------------

static IPC_FFDeviceParams_t IPC_FFDeviceParams =
{
    {FF_DEVICE_TAG_DEF},
    {FF_DEVICE_ID_DEF},
    0,
    CRC_SEED, //make valid checksum by C init
};

// get the pointer of device variables
IPC_FFDeviceParams_t* GetDeviceVar(void)
{
     return &IPC_FFDeviceParams;
}

static FFData_t m_FFData;
static const FFData_t ffdata_default =
{
    .SimulationActive = false,
    .CheckWord = CRC_SEED,
};

/* Set values of device variables
IPC_FFDeviceParams_t* SetDeviceVar(const IPC_FFDeviceParams_t* p)
{
    Struct_Copy(IPC_FFDeviceParams_t, &IPC_FFDeviceParams, p);
    return &IPC_FFDeviceParams;
}*/

ErrorCode_t  IPC_WriteDeviceTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pDeviceTagSegment = IPC_FFDeviceParams.dev_tag;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_DEVICE_TAG_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pDeviceTagSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_DEVICE_TAG_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pDeviceTagSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pDeviceTagSegment, FF_DEVICE_TAG_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_DEVICE_TAG_WR_MAX_BLOCK_NUM, pData);
         }

         STRUCT_CLOSE(IPC_FFDeviceParams_t, &IPC_FFDeviceParams);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}

ErrorCode_t  IPC_WriteDeviceID(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              DataBlockNumber = pIPC_WritePtrs->IPC_DataBlockNum;
    u8*             pDeviceIDSegment = IPC_FFDeviceParams.dev_id;
    const u8*       pData = (const u8*)pIPC_WritePtrs->pIPC_VarBuffer;

    if (DataBlockNumber > FF_DEVICE_ID_WR_MAX_BLOCK_NUM)
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_BLOCKNUM));
    }
    else
    {
         pDeviceIDSegment += DataBlockNumber * IPC_WRITE_ARRAY_DATASIZE;
         if (DataBlockNumber < FF_DEVICE_ID_WR_MAX_BLOCK_NUM)
         {
            util_PutU8Array(pDeviceIDSegment, IPC_WRITE_ARRAY_DATASIZE, pData);
         }
         else
         {
            // block number = 2, write 10 characters
            util_PutU8Array(pDeviceIDSegment, FF_DEVICE_ID_LEN - IPC_WRITE_ARRAY_DATASIZE * FF_DEVICE_ID_WR_MAX_BLOCK_NUM, pData);
         }

         STRUCT_CLOSE(IPC_FFDeviceParams_t, &IPC_FFDeviceParams);
         util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    }

    UNUSED_OK(VarID);

    return retval;
}

ErrorCode_t  IPC_WriteDeviceAddress(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    u8              varStatus = util_GetU8(pIPC_WritePtrs->pVarStatus);
    if (IsQuality_ACCEPT(varStatus))
    {
        IPC_FFDeviceParams.dev_addr = (u8)util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
        STRUCT_CLOSE(IPC_FFDeviceParams_t, &IPC_FFDeviceParams);
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer));
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

ErrorCode_t  IPC_ReadAppFwVer(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
/*
    typedef union
    {
        u32 FullVersion;
        struct
        {
            u8 fwtype;
            u8 txrev;
            u8 swrev;
            u8 hwrev;
        } SubVersion;
    } app_version_t;

    app_version_t version;

    version.SubVersion.hwrev = APP_HW_REV;
    version.SubVersion.swrev = APP_SW_REV;
    version.SubVersion.txrev = APP_TXSEPC_REV;
    version.SubVersion.fwtype = APP_FW_TYPE;
*/

    u32 FullVersion = ((u8)APP_FW_TYPE) | (APP_TXSEPC_REV << 8) | (APP_SW_REV << 16) | (APP_HW_REV << 24);

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, FullVersion);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);

    UNUSED_OK(VarID);
    return ERR_OK;
}

/** \brief Gets FF data supplied in INTERNAL representation
*/
const void *TypeUnsafe_ff_GetLocalFFData(void *dst)
{
    return STRUCT_TESTGET(&m_FFData, dst);
}

/** \brief Sets FF data supplied in INTERNAL representation
*/
ErrorCode_t TypeUnsafe_ff_RepairSetFFData(const void *src)
{
    const FFData_t *FFData = src;

    if(FFData == NULL)
    {
        FFData = &ffdata_default;
    }

    FFData_t ffdata = *FFData;
    Struct_Copy(FFData_t, &m_FFData, &ffdata);
    return ram2nvramAtomic(NVRAMID_FFData); //always able to repair
}

/** \brief IPC interface for FF variable Simulation
*/
ErrorCode_t  IPC_ReadLuiSimulation(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    FFData_t    ffdata;
    u32         sim;

    (void)ff_GetLocalFFData(&ffdata);
    sim = (u32)(ffdata.SimulationActive ? SIMULATION_ACTIVE : SIMULATION_DEACTIVE);
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, sim);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);

    UNUSED_OK(VarID);
    return ERR_OK;
}

/* This line marks the end of the source */

