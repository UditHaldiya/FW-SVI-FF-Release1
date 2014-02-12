/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnctllimff.c
    \brief Handling of find stops interface

    CPU: Any

    OWNER: AK
*/


//Softing headers first
#include <softing_base.h>
#include <ptb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "process.h"
#include "hartcmd.h"
#include "faultpublic.h"
#include "ipcdefs.h"
#include "ipcvarids.h"

//Glue headers last
#include "mnfindstopsff.h"

#if 0 //now in fbif_cfg.h
#define FINDSTOPS_START 1
#define FINDSTOPS_CANCEL 2
#define FINDSTOPS_SETCLOSED 3
#define FINDSTOPS_SETOPEN 4
#define FINDSTOPS_ACCEPT 5
#define FINDSTOPS_COMMIT 6
#define FINDSTOPS_RUNNING 11
#define FINDSTOPS_NOTRUNNING 0
#endif

#define CAL_TYPE_CLONED         (5u)
#define CAL_TYPE_READY_CLONE     (43u)

fferr_t ffposcal_ReadFindStops(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    UNUSED_OK(p_read);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    fferr_t fferr = mn_HART_acyc_cmd(251, from, 0, to);
    if(fferr == E_OK)
    {
        Rsp_CheckProcess_t *d = (void *)((u8 *)to + 2);
        p_PTB->find_stops =
            (util_GetU8(d->ProcessId[0]) == PROC_FIND_STOPS)?
            FINDSTOPS_RUNNING:FINDSTOPS_NOTRUNNING;
    }

    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly


/** \brief Writes Find Stops command
NOTE: apprently, p_write->source is filled in correct order but without alignment padding
*/
fferr_t ffposcal_WriteFindStops(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    fferr_t fferr;


    u8 selection = p_write->source[0];
#if 1
    //For Vladimir's request; does nothing for worst-case timing but feels good
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    if((selection != FINDSTOPS_CANCEL) && (p_PTB->mode_blk.actual != MODE_MAN))
    {
        return E_FB_WRONG_MODE;
    }
#else
    UNUSED_OK(p_block_instance);
#endif
    Req_ManualFindStops_t *s = (void *)from;
    u8 len = MN_OFFSETOF(Req_ManualFindStops_t, endmark);
    Req_WriteSetpoint_t *subcmd = (void *)from;
    u8 lensub = MN_OFFSETOF(Req_WriteSetpoint_t, endmark);

    switch(selection)
    {
        case FINDSTOPS_START:
            fferr = mn_HART_acyc_cmd(180, from, 0, to);
            break;
        case FINDSTOPS_CANCEL:
            fferr = mn_HART_acyc_cmd(253, from, len, to);
            break;
        case FINDSTOPS_ACCEPT:
            util_PutU8(s->ManualFindStopsStep[0], HFindStopsAccept);
            fferr = mn_HART_acyc_cmd(179, from, len, to);
            break;
        case FINDSTOPS_COMMIT:
            util_PutU8(s->ManualFindStopsStep[0], HFindStopsCommit);
            fferr = mn_HART_acyc_cmd(179, from, len, to);
            break;
        case FINDSTOPS_SETCLOSED:
            util_PutU8(subcmd->SubCmdNum[0], 2);
            fferr = mn_HART_acyc_cmd(155, from, lensub, to);
            break;
        case FINDSTOPS_SETOPEN:
            util_PutU8(subcmd->SubCmdNum[0], 3);
            fferr = mn_HART_acyc_cmd(155, from, lensub, to);
            break;
        default:
            fferr = E_FB_PARA_CHECK;
            break;
    }

    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly

/** \brief Reads find stops info
*/
fferr_t ffposcal_ReadFindStopsInfo(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    u8 subindex = p_read->subindex;
    fferr_t fferr = E_OK;
    if((subindex==0) || (subindex==4) || (subindex==5))
    {
        //Need to populate stop values
        //A *VERY* temporary solution
        fferr = mn_HART_acyc_cmd(141, from, 0, to);
        if(fferr == E_OK)
        {
            u8 *rsp = to;
            p_PTB->travel_calibration.stop_hi_pos = util_GetS16(rsp+2+HC141_HIGH_POS_STOP);
            p_PTB->travel_calibration.stop_lo_pos = util_GetS16(rsp+2+HC141_LOW_POS_STOP);
        }
    }

    if((subindex==0) || (subindex==7))
    {
        //Last result
        p_PTB->travel_calibration.last_result = (mn_IsAppFault(p_PTB, FAULT_FIND_STOPS_FAILED))?0:1;
    }

    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly

/** \brief Reads open stop adjustment
*/
fferr_t ffposcal_ReadOpenStopAdjustment(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    UNUSED_OK(p_read);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    Req_ReadStopAdjust_t *subcmd = (void *)from;
    u8 len = MN_OFFSETOF(Req_ReadStopAdjust_t, endmark) + MN_OFFSETOF(Req_ReadOpenStopAdjust_t, endmark);
    util_PutU8(subcmd->SubCmdNum[0], 0);
    fferr_t fferr = mn_HART_acyc_cmd(169, from, len, to);
    if(fferr == E_OK)
    {
        Rsp_ReadOpenStopAdjust_t *d = (void *)((u8 *)to + 2 + MN_OFFSETOF(Rsp_ReadStopAdjust_t, endmark));
        float32 f = util_GetFloat(d->adj_OpenStopAdj[0]);
        p_PTB->open_stop_adjustment = f; //directly
    }
    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly

/** \brief Writes open stop adjustment
*/
fferr_t ffposcal_WriteOpenStopAdjustment(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    Req_WriteStopAdjust_t *subcmd = (void *)from;
    Req_SetOpenStopAdjust_t *s = (void *)((u8*)from + MN_OFFSETOF(Req_WriteStopAdjust_t, endmark));
    u8 len = MN_OFFSETOF(Req_WriteStopAdjust_t, endmark) + MN_OFFSETOF(Req_SetOpenStopAdjust_t, endmark);
    util_PutU8(subcmd->SubCmdNum[0], 0);
    float32 f = mn_pull_float(p_write->source);
    util_PutFloat(s->adj_OpenStopAdj[0], f);
    fferr_t fferr = mn_HART_acyc_cmd(167, from, len, to);
    if(fferr == E_OK)
    {
        p_PTB->open_stop_adjustment = f; //directly; in case needed (I doubt)
    }
    return fferr;
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly

static void  SaveCalibrationType(T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    write_loc.rel_idx = REL_IDX_PTB_TRAVEL_CALIBRATION;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = MN_OFFSETOF(_TRAVEL_CALIBRATION, last_result) + sizeof(p_PTB->travel_calibration.last_result);
    write_loc.source        = (USIGN8 *)(&p_PTB->travel_calibration);
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

/* \brief this function will be called when user write stop_hi_pos and stop_low_pos for valve clone function
    param in:
        p_block_instance: pointer to function block
        p_write: write structure
        snd_buf:  send buffer
        rcv_buf:  receive buffer
    return:
        FF error
*/
fferr_t ffposcal_WriteStopPosition(T_FBIF_BLOCK_INSTANCE *p_block_instance, const T_FBIF_WRITE_DATA *p_write, void *snd_buf, void *rcv_buf)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    fferr_t fferr = E_OK;
    Req_IPCWriteShortVariables_t* req_buf = snd_buf;

    switch (p_write->subindex)
    {
    case 0:
        {
            //_TRAVEL_CALIBRATION* p_travel_cal = (_TRAVEL_CALIBRATION*)(void*)p_write->source;
            //alignment issue
            u8* p_cal_type = p_write->source + MN_OFFSETOF(_TRAVEL_CALIBRATION, cal_type)-1; //offset = 79
            s32* p_stop_hi_pos = (s32*)(void*)(p_write->source + MN_OFFSETOF(_TRAVEL_CALIBRATION, stop_hi_pos)-1); //71
            s32* p_stop_lo_pos = (s32*)(void*)(p_write->source + MN_OFFSETOF(_TRAVEL_CALIBRATION, stop_lo_pos)-1); //75
            if(*p_cal_type == CAL_TYPE_READY_CLONE &&
               (*p_stop_hi_pos != p_PTB->travel_calibration.stop_hi_pos ||
                *p_stop_lo_pos != p_PTB->travel_calibration.stop_lo_pos)
               )
            {
                util_PutU16(req_buf->IPCVariableID1[0], (u16)IPC_STOP_HI_POS);
                util_PutU8(req_buf->IPCVar1Status[0], IPC_QUALITY_GOOD);
                util_PutS32(req_buf->IPCVar1Buffer[0], *p_stop_hi_pos);
                util_PutU16(req_buf->IPCVariableID2[0], (u16)IPC_STOP_LO_POS);
                util_PutU8(req_buf->IPCVar2Status[0], IPC_QUALITY_GOOD);
                util_PutS32(req_buf->IPCVar2Buffer[0], *p_stop_lo_pos);
                fferr = mn_HART_acyc_cmd(186, snd_buf, HC186_REQ_LENGTH, rcv_buf);
                if(E_OK == fferr)
                {
                    *p_cal_type = CAL_TYPE_CLONED;
                }
            }
            else if (*p_cal_type != CAL_TYPE_READY_CLONE &&
               (*p_stop_hi_pos != p_PTB->travel_calibration.stop_hi_pos ||
                *p_stop_lo_pos != p_PTB->travel_calibration.stop_lo_pos)
               )
            {
                fferr = E_FB_WRITE_LOCK;
            }
            else{}
            break;
        }
    case 4:
        {
            s32 stop_hi_pos = *(s32*)(void*)p_write->source;
            if(p_PTB->travel_calibration.cal_type == CAL_TYPE_READY_CLONE)
            {
                util_PutU16(req_buf->IPCVariableID1[0], (u16)IPC_STOP_HI_POS);
                util_PutU8(req_buf->IPCVar1Status[0], IPC_QUALITY_GOOD);
                util_PutS32(req_buf->IPCVar1Buffer[0], stop_hi_pos);
                util_PutU16(req_buf->IPCVariableID2[0], (u16)IPC_STOP_LO_POS);
                util_PutU8(req_buf->IPCVar2Status[0], IPC_QUALITY_BAD);
                util_PutS32(req_buf->IPCVar2Buffer[0], 0);
                fferr = mn_HART_acyc_cmd(186, snd_buf, HC186_REQ_LENGTH, rcv_buf);
                if(E_OK == fferr)
                {
                    p_PTB->travel_calibration.cal_type = CAL_TYPE_CLONED;
                    SaveCalibrationType(p_block_instance);
                }
            }
            else
            {
                fferr = E_FB_WRITE_LOCK;
            }
            break;
        }
    case 5:
        {
            s32 stop_lo_pos = *(s32*)(void*)p_write->source;
            if(p_PTB->travel_calibration.cal_type == CAL_TYPE_READY_CLONE)
            {
                util_PutU16(req_buf->IPCVariableID1[0], (u16)IPC_STOP_HI_POS);
                util_PutU8(req_buf->IPCVar1Status[0], IPC_QUALITY_BAD);
                util_PutS32(req_buf->IPCVar1Buffer[0], 0);
                util_PutU16(req_buf->IPCVariableID2[0], (u16)IPC_STOP_LO_POS);
                util_PutU8(req_buf->IPCVar2Status[0], IPC_QUALITY_GOOD);
                util_PutS32(req_buf->IPCVar2Buffer[0], stop_lo_pos);
                fferr = mn_HART_acyc_cmd(186, snd_buf, HC186_REQ_LENGTH, rcv_buf);
                if(E_OK == fferr)
                {
                    p_PTB->travel_calibration.cal_type = CAL_TYPE_CLONED;
                    SaveCalibrationType(p_block_instance);
                }
            }
            else
            {
                fferr = E_FB_WRITE_LOCK;
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return fferr;
}






/* This line marks the end of the source */
