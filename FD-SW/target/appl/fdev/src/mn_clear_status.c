/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_clear_status.c
    \brief function about parameter CLEAR_STATUS

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include "hartcmd.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"
#include "mnhart2ff.h"
#include "mn_clear_status.h"

#define CLEAR_STATUS_ON_ACTION      0u
//#define CLEAR_STATUS_CLEAR_CURRENT  1u
//#define CLEAR_STATUS_CLEAR_HISTORY  2u

/* \brief write clear status parameter
    param in:
        p_write: pointer to write block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t WriteClearStatus(const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf)
{
    //hart cmd 138
    fferr_t fferr = E_OK;
    u8 clr_status = util_GetU8(p_write->source);

    if(CLEAR_STATUS_ON_ACTION != clr_status)
    {
        util_PutU8(snd_buf, clr_status - 1);
        fferr = mn_HART_acyc_cmd(138, snd_buf, HC138_REQ_LENGTH, rcv_buf);

        if(E_OK == fferr)
        {
            util_PutU8(p_write->source, CLEAR_STATUS_ON_ACTION);
        }
    }

    return fferr;
}
