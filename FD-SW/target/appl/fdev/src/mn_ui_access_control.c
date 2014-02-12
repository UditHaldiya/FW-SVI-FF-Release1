/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_ui_access_control.c
    \brief functions about parameter UI_ACCESS_CONTROL

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>
//Glue headers last
#include "bufutils.h"
#include "hartdef.h"
#include "mnhart2ff.h"
#include "mn_ui_access_control.h"

/* \brief read UI access control parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ReadUIAccessControl(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    //hart cmd 170,2
    fferr_t fferr;

    Req_ReadSettings_t* req = snd_buf;

    Rsp_ReadUIAccessControl_t* rsp_2 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    util_PutU8(req->SubCmdNum[0], 2);

    fferr = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr == E_OK)
    {
        UIAccessControl_t* p_access_ctrl = (UIAccessControl_t*)(void*)rsp_2->UIAccessControl[0];
        p_PTB->ui_access_control.component_0 = util_GetU8((p_access_ctrl->UILockLevel[0]));
        p_PTB->ui_access_control.component_1 = util_GetU8((p_access_ctrl->UIPasswordEnabledFlag[0]));
        p_PTB->ui_access_control.component_2 = util_GetS16((p_access_ctrl->UIpassword[0]));
    }

    return fferr;
}

/* \brief write UI access control parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t WriteUIAccessControl(const T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf)
{
    //hart cmd 171,2
    fferr_t fferr = E_OK;

    Req_WriteSettings_t* req = snd_buf;

    Req_WriteUIAccessControl_t* req_2 = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    UIAccessControl_t* p_access_ctrl = (UIAccessControl_t*)(void*)(req_2->UIAccessControl);

    util_PutU8(req->SubCmdNum[0], 2);

    switch (p_write->subindex)
    {
    case 0://all
        {
            UIAccessControl_t* p_asscess_ctrl_src = (UIAccessControl_t*)(void*)p_write->source;
            util_PutU8(p_access_ctrl->UILockLevel[0], *(p_asscess_ctrl_src->UILockLevel[0]));
            util_PutU8(p_access_ctrl->UIPasswordEnabledFlag[0], *(p_asscess_ctrl_src->UIPasswordEnabledFlag[0]));
            util_PutS16(p_access_ctrl->UIpassword[0], *(p_asscess_ctrl_src->UIpassword[0]));
            break;
        }
    case 1://lock_level
        {
            u8 UILockLevel = *(p_write->source);
            util_PutU8(p_access_ctrl->UILockLevel[0], UILockLevel);
            util_PutU8(p_access_ctrl->UIPasswordEnabledFlag[0], p_PTB->ui_access_control.component_1);
            util_PutS16(p_access_ctrl->UIpassword[0], p_PTB->ui_access_control.component_2);
            break;
        }
    case 2://password_enabled
        {
            u8 UIPasswordEnabledFlag = *(p_write->source);
            util_PutU8(p_access_ctrl->UILockLevel[0], p_PTB->ui_access_control.component_0);
            util_PutU8(p_access_ctrl->UIPasswordEnabledFlag[0], UIPasswordEnabledFlag);
            util_PutS16(p_access_ctrl->UIpassword[0], p_PTB->ui_access_control.component_2);
            break;
        }
    case 3://password
        {
            s16 UIpassword = *((s16*)(void*)p_write->source);
            util_PutU8(p_access_ctrl->UILockLevel[0], p_PTB->ui_access_control.component_0);
            util_PutU8(p_access_ctrl->UIPasswordEnabledFlag[0], p_PTB->ui_access_control.component_1);
            util_PutS16(p_access_ctrl->UIpassword[0], UIpassword);
            break;
        }
    default:
        {
            fferr = E_ERROR;
        }
    }

    if(fferr == E_OK)
    {
        fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH + MN_OFFSETOF(Req_WriteUIAccessControl_t, endmark), rcv_buf);
    }

    return fferr;
}

