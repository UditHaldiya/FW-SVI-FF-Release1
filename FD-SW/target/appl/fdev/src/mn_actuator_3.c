/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_actuator_3.c
    \brief functions about parameter ACTUATOR_3

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "crc.h"

//Glue headers last
#include "mnhart2ff.h"
#include "mn_actuator_3.h"

void IncrementStaticRevision(T_FBIF_BLOCK_INSTANCE* p_block_instance, s8 sIncNum)
{
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if (sIncNum > 0)
    {
        p_PTB->st_rev += (u8)sIncNum;
    }
    else
    {
        p_PTB->st_rev -= (u8)(ABS(sIncNum));
    }


    write_loc.rel_idx = REL_IDX_PTB_ST_REV;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = sizeof(p_PTB->st_rev);
    write_loc.source        = (USIGN8 *)(void*)&(p_PTB->st_rev);
    if (appl_check_hm_state() == HM_RUN )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    (void)fbs_write_param_loc (p_block_instance,&write_loc);
}

/* \brief read actuator_3 parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ReadActuator3(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    fferr_t fferr = E_OK;
    //hart cmd 170,210
    fferr_t fferr_action;
    Req_ReadSettings_t* req = snd_buf;
    Rsp_ReadAirAction_t* rsp_210 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    util_PutU8(req->SubCmdNum[0], 210);
    fferr_action = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr_action == E_OK)
    {
        u8 ap_air_action = util_GetU8(rsp_210->AirAction[0]);
        if (MN_AIR_TO_CLOSE == ap_air_action)
        {
            //act fail action vlaue is 2
            p_PTB->actuator_3.act_fail_action = MN_DDVALUE_AIR_TO_CLOSE;
        }
        else if (MN_AIR_TO_OPEN == ap_air_action)
        {
            //act fail action vlaue is 1
            p_PTB->actuator_3.act_fail_action = MN_AIR_TO_OPEN;
        }
        else
        {
            fferr = E_ERROR;
        }
    }
    else
    {
        fferr = fferr_action;
    }

    //hart cmd 170 167
    Rsp_ReadPneumaticParameters_t* rsp_167 = (void*)((u8*)rcv_buf + HC170_LENGTH + 2);
    util_PutU8(req->SubCmdNum[0], 167);
    fferr_action = mn_HART_acyc_cmd(170, snd_buf, HC170_REQ_LENGTH, rcv_buf);

    if(fferr_action == E_OK)
    {
        PneumaticParameters_t* p_pp = (PneumaticParameters_t*)rsp_167->PneumaticParameters[0];
        u8 single_flag = util_GetU8(p_pp->SingleActingFlag[0]);
        if(SINGLE_ACTING == single_flag)
        {
            p_PTB->actuator_3.style = SINGLE_ACTING;
        }
        else
        {
            p_PTB->actuator_3.style = DOUBLE_ACTING;
        }
    }
    else
    {
        fferr = fferr_action;
    }
    return fferr;
}

/* \brief write actuator_3 parameter
    param in:
        p_block_instance: pointer to function block
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t WriteActuator3(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf)
{
    fferr_t fferr = E_OK;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if(4 == p_write->subindex || 0 == p_write->subindex)
    {
        if(MODE_OS != p_PTB->mode_blk.target)
        {
            fferr = E_FB_WRONG_MODE;
            return fferr;
        }
        else
        {
            //hart cmd 171,210
            Req_WriteSettings_t* req = snd_buf;
            Req_WriteAirAction_t* req_210 = (void*)((u8*)snd_buf + HC171_REQ_LENGTH);
            util_PutU8(req->SubCmdNum[0], 210);
            u8 ap_air_action = 0;
            if(4 == p_write->subindex)
            {
                if (MN_DDVALUE_AIR_TO_CLOSE == *((u8*)p_write->source))
                {
                    //act fail action vlaue should be 0
                    ap_air_action = MN_AIR_TO_CLOSE;
                }
                else if (MN_AIR_TO_OPEN == *((u8*)p_write->source))
                {
                    //act fail action vlaue is 1
                    ap_air_action = MN_AIR_TO_OPEN;
                }
                else
                {
                    fferr = E_FB_PARA_LIMIT;
                    return fferr;
                }
            }
            else
            {
                _ACTUATOR_3* p_actuator_3 = (_ACTUATOR_3*)(void*)p_write->source;
                if (MN_DDVALUE_AIR_TO_CLOSE == p_actuator_3->act_fail_action)
                {
                    //act fail action vlaue should be 0
                    ap_air_action = MN_AIR_TO_CLOSE;
                }
                else if (MN_AIR_TO_OPEN == p_actuator_3->act_fail_action)
                {
                    //act fail action vlaue is 1
                    ap_air_action = MN_AIR_TO_OPEN;
                }
                else
                {
                    fferr = E_FB_PARA_LIMIT;
                    return fferr;
                }
            }
            util_PutU8(req_210->AirAction[0], ap_air_action);

            fferr = mn_HART_acyc_cmd(171, snd_buf, HC171_REQ_LENGTH + MN_OFFSETOF(Req_WriteAirAction_t, endmark), rcv_buf);

            if(E_OK == fferr)
            {
                IncrementStaticRevision(p_block_instance, 1);
            }
            return fferr;
        }
    }
    else
    {
        return fferr;
    }
}
