/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_language.c
    \brief functions about parameter UI_LANGUAGE

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include "base.h"
//end incantaion

#include "fbap.h"
#include "fbif.h"
#include "fbs_api.h"
#include "ptb.h"

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"

//Glue headers last
#include "mnhart2ff.h"
#include "ui_language.h"


/* \brief read UI language parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ReadUILanguage(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 170,1
    fferr_t fferr;

    Req_ReadSettings_t* req = snd_buf;

    Rsp_ReadUIlanguage_t* rsp_1 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    util_PutU8(req->SubCmdNum[0], 1);

    fferr = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr == E_OK)
    {
        p_PTB->ui_language = util_GetU8(rsp_1->UILanguage[0]);
    }

    return fferr;
}

/* \brief write UI language parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t WriteUILanguage(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 171,1
    fferr_t fferr;

    Req_WriteSettings_t* req = snd_buf;

    Req_WriteUIlanguage_t* req_1 = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    util_PutU8(req->SubCmdNum[0], 1);

    util_PutU8(req_1->UILanguage[0], p_PTB->ui_language);

    fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH + MN_OFFSETOF(Req_WriteUIlanguage_t, endmark), rcv_buf);

    return fferr;
}
