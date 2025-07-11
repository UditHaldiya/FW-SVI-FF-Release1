/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_fedlxmit.c
    \brief Glue functions related to position control HART support

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_posctl.c $
    $Date: 1/20/12 10:48a $
    $Revision: 10 $
    $Author: Anatoly Podpaly $

    \ingroup HARTapp
*/

#include "mnwrap.h"
#include "hart.h"
#include "hartdef.h"
#include "bufutils.h"
#include "faultpublic.h"
#include "mn_flash.h"
#include "hartfunc.h"

typedef         u32 (*flashOp)(u32 value);
static const    u8* hdata;
static          u16 bdata[MAX_PROG_DATA / sizeof(u16)];

static u32 flash_pgm(u32 addr);
static u32 flash_setpartn(u32 ptn);

// dispatch table for flash update operations
static const flashOp ops[] =
{
    [SET_PARTN]   = flash_setpartn,
    [OPEN_PARTN]  = flash_OpenPartition,
    [CLOSE_PARTN] = flash_ClosePartition,
    [PROG_BLOCK]  = flash_pgm,
    [PARTN_BASE]  = flash_GetPartitionBaseAddress
};

/** \brief Set the flash partition buit according to the passed value
    and schedule a reset on the way out.

    \param ptn - the partition state to set (Bank0/bank1}
*/
static u32 flash_setpartn(u32 ptn)
{
    hart_ResetDeviceOnSend();
    return flash_SetPartition(ptn);
}

/** \brief fetch the rest of the program-block params and call the flash program routine
    See mn_flash.h for offsets in the hart paylaod  The rest if the command,
    flags and length are stil in the hart buffer, hdata;

    \param addr - the beginning flash address at which to prgraom the payload data
*/
static u32 flash_pgm(u32 addr)
{
    static u32 lastaddr = 0, result;
  
    u8_least len   = *(hdata - 1) - DATA_OFFSET;    // length in bytes from hart buffer
    u8_least flags = *(hdata + FLAG_OFFSET);        // flags (update CRC | NOERASE

    memcpy(bdata, hdata + DATA_OFFSET, len);        // ensure 16-bit alignment

    if (lastaddr != addr)
    {
        lastaddr = addr;
        result   = flash_ProgramBlock(bdata, HARDWARE(u16*, addr), len, flags);
    }
    return result;
}

/** \brief This command IS an IPC Firmware Download support.  It dispatches the commands
    from the FF CPU to update the flash image

    \param src - pointer to the payload of the received hart packet
    \param dst - pointer to the payload of the reply hart packet
    \return - the 32-bit result from the requested operation
*/
s8_least hartcmd_IPCFirmwareDownloadDataTransfer(const u8 *src, u8 *dst)
{
    const Req_IPCFirmwareDownloadDataTransfer_t     *s1 = (const void *)src;
    Rsp_IPCFirmwareDownloadDataTransfer_t           *d1 = (void *)dst;
    const u8 *s2 = s1->FWSendDataBlock[0];
          u8 *d2 = d1->FWReceiveDataBlock[0];

    u8_least cmd = util_GetU8 (src + CMD_OFFSET);
    u32     addr = util_GetU32(src + ADDR_OFFSET);
    u32     rslt = 0u;

    UNUSED_OK(s2);
    UNUSED_OK(d2);

    if (cmd < FMAX_OP)
    {
        if (cmd == VER_INFO)
        {
            return -(s8_least)GetFwInfo(dst);
        }
        hdata = src;                        // save a pointer so flash_pgm() can get it
        rslt = ops[cmd](addr);
    }
    util_PutU32(dst, rslt);
    return -(s8_least)sizeof(rslt);
}

s8_least hartcmd_ReadAPPActiveFlashBank(const u8 *src, u8 *dst)
{
	const Req_ReadAPPActiveFlashBank_t *s = (const void *)src;
	UNUSED_OK(s);
	Rsp_ReadAPPActiveFlashBank_t *d = (void *)dst;
	util_PutU8(d->APPFlashBank[0], flash_GetRunningPartitionID());
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
