/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file app_mode.c
    \brief functions about parameter APP_MODE

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"

//Glue headers last
#include "mnhart2ff.h"
#include "app_mode.h"

/* \brief read APP mode parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ReadAPPMode(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 249
    fferr_t fferr;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    Rsp_ReadDeviceMode_t* rsp = (void*)((u8*)rcv_buf + 2);

    fferr = mn_HART_acyc_cmd(249, snd_buf, HC249_REQ_LENGTH, rcv_buf);

    if(fferr == E_OK)
    {
        p_PTB->app_mode = util_GetU8(rsp->FullDeviceMode[0]);
    }

    return fferr;
}

/* \brief write APP mode parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t WriteAPPMode(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 135
    fferr_t fferr;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    Req_ChangeDeviceMode_t* req = snd_buf;

    util_PutU8(req->DeviceMode[0], p_PTB->app_mode);

    fferr = mn_HART_acyc_cmd(135, snd_buf, HC135_REQ_LENGTH, rcv_buf);

    return fferr;
}

