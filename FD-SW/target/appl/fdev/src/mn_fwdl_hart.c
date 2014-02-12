/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_fwdl_hart.c
    \brief functions to pass firmware download commands to the APP CPU

    CPU: Any

    OWNER: EP
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Ernir Price $
*/
#include <softing_base.h>
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "mnhart2ff.h"
#include "mn_flash.h"

static u8 send_buffer[60], recv_buffer[60];

//-----------------------------------------------------------------------------------------

/** \brief Send a PROG_BLOCK command to the APP CPU using hart command 245.
    See mn_flash.h for the list of commands. Also see hart_fwdlxmit.c in the APP CPU.

    \param data - pointer to the block of firmware data.
    \param addr - the starting flash address for the data
    \param len - te length in bytes of the data
    \param flags - bitmap of flags indication update of address and
    \return flash command result if successful; otherwise 0xffffffff
*/
/* async hart command 245 to update flash in the APP CPU */
u32 fwdk_WriteAppCPU(const void *data, u32 addr, u8_least len, u8_least flags)
{
    fferr_t fferr;

    util_PutU8 (send_buffer + CMD_OFFSET,  PROG_BLOCK);
    util_PutU8 (send_buffer + FLAG_OFFSET, (u8)flags);
    util_PutU32(send_buffer + ADDR_OFFSET, addr);
    memcpy     (send_buffer + DATA_OFFSET, data, len);

    fferr = mn_HART_acyc_cmd(245, send_buffer, (u8)(len + DATA_OFFSET), recv_buffer);
    if(fferr == E_OK)
    {
        return util_GetU32(recv_buffer + HART_NUM_STATUS_BYTES);
    }
    return ~0;
}

/** \brief Ask the APP CPU about the versions of software in ints flash banks

    \return - pointer to the returned array[2] of FFInfo_t
*/
void *fwdk_GetVerInfo(void)
{
    fferr_t fferr;

    util_PutU8 (send_buffer + CMD_OFFSET, (u8)VER_INFO);
    fferr = mn_HART_acyc_cmd(245, send_buffer, (u8)DATA_OFFSET, recv_buffer);
    if(fferr == E_OK)
    {
        return recv_buffer + HART_NUM_STATUS_BYTES;
    }
    return NULL;
}

/** \brief Send a flash command to the APP CPU usig hart command 245.  See
    mn_flash.h for the list of commands. Also see hart_fwdlxmit.c in the APP CPU.

    \param cmdtype - one of the supported commands. Except PROG_BLOCK which is
                    handled above.
    \param value - a 32-bit command secific paramaeter
    \return the 32-bit result from the flash command execution in the APP CPU;
                0xffffffff if the cammand failed.
*/
u32 fwdk_WriteAppCPU32(u8_least cmdtype, u32 value)
{
    fferr_t fferr;

    util_PutU8 (send_buffer + CMD_OFFSET, (u8)cmdtype);
    util_PutU8 (send_buffer + FLAG_OFFSET, (u8)0);
    util_PutU32(send_buffer + ADDR_OFFSET, value);
    fferr = mn_HART_acyc_cmd(245, send_buffer, (u8)DATA_OFFSET, recv_buffer);
    if(fferr == E_OK)
    {
        return util_GetU32(recv_buffer + HART_NUM_STATUS_BYTES);
    }
    return ~0;
}

// end of source
