/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnctllimff.c
    \brief Handling of control limits interface

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

//Glue headers last
#include "mnctllimff.h"


/** \brief A helper to pre-populate control limits from the APP. \n
Also Fills Tx buffer and control limits structure(s) from APP
*/
static fferr_t ctllim_Populate(_POSITION_LIMITS *plim, _FINAL_VALUE_CUTOFF_LO *lo, _FINAL_VALUE_CUTOFF_HI *hi, u8 *from, u8 *to)
{
    //Ask APP what it has
    fferr_t fferr;

    //Populate the send buffer
    Req_ReadSettings_t *subcmd = (void *)from;
    util_PutU8(subcmd->SubCmdNum[0], 166);
    ControlLimitsConf_t *s = (void *)((u8 *)to + MN_OFFSETOF(Rsp_ReadSettings_t, endmark) + 2);

    u8 len = MN_OFFSETOF(Req_ReadSettings_t, endmark) + MN_OFFSETOF(Req_ReadPositionControlLimits_t, endmark);
    fferr = mn_HART_acyc_cmd(170, from, len, to);
    if(fferr == E_OK)
    {
        //Copy response to Tx buffer for write-back (in case we will)
        memcpy(from+MN_OFFSETOF(Req_ReadSettings_t, endmark), s, sizeof(*s));
        //Populate TB types
        if(lo != NULL)
        {
            lo->enable = util_GetU8(s->TightShutoffEnabled[0]);
            lo->cutoff_point_lo = util_GetFloat(s->TightShutOff[0]);
        }
        if(hi != NULL)
        {
            hi->enable = util_GetU8(s->TightCutoffEnabled[0]);
            hi->cutoff_point_hi = util_GetFloat(s->TightCutOff[0]);
        }
        if(plim != NULL)
        {
            plim->component_0 = util_GetU8(s->PositionLimitsProtected[0]);
            plim->component_1 = util_GetU8(s->PositionLimitsEnabled[Xhi]);
            plim->component_2 = util_GetU8(s->PositionLimitsEnabled[Xlow]);
            plim->component_4 = util_GetFloat(s->PositionLimits[Xlow]);
            plim->component_3 = util_GetFloat(s->PositionLimits[Xhi]);
            plim->component_5 = util_GetU8(s->PositionRateLimitsEnabled[Xhi]);
            plim->component_6 = util_GetU8(s->PositionRateLimitsEnabled[Xlow]);
            plim->component_7 = util_GetFloat(s->PositionRateLimits[0]);
        }
    }

    return fferr;
}

fferr_t ffctllim_ReadControlLimits(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    UNUSED_OK(p_read);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    return ctllim_Populate(&p_PTB->position_limits, &p_PTB->final_value_cutoff_lo, &p_PTB->final_value_cutoff_hi, from, to);
} //lint !e818 const-ness may be confusing to reader since the pointed-to object is modified indirectly


/** \brief Writes Cut-off hi configuration
NOTE: apprently, p_write->source is filled in correct order but without alignment padding
*/
fferr_t ffctllim_WriteCutoffHi
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    const T_FBIF_WRITE_DATA *p_write,
    void *from,
    void *to
)
{
    T_FBS_WRITE_PARAM_LOCAL  p_write_loc;
    u8                       len;
    u8                       flag = FALSE;
    fferr_t fferr = ctllim_Populate(NULL, NULL, NULL, from, to);

    if(fferr == E_OK)
    {
        Req_WritePositionControlLimits_t *s1 =
            (void *)((u8 *)from + MN_OFFSETOF(Req_ReadSettings_t, endmark));
        ControlLimitsConf_t *s = (void *)s1->ControlLimitsConf[0];
        switch(p_write->subindex)
        {
            case 0: //everything)
            {
                //if the tightcutoff is enabled, clear the position_limits.Enable_hi
                if (*p_write->source != FALSE)
                {
                    util_PutU8(s->PositionLimitsEnabled[Xhi], FALSE);
                    flag = TRUE;
                }
                util_PutU8(s->TightCutoffEnabled[0], *p_write->source);
                util_PutFloat(s->TightCutOff[0], mn_pull_float(p_write->source+1));
                break;
            }
            case 1: //enable/disable
            {
                //if the tightcutoff is enabled, clear the position limits.Enable_hi
                if (*p_write->source != FALSE)
                {
                    util_PutU8(s->PositionLimitsEnabled[Xhi], FALSE);
                    flag = TRUE;
                }
                util_PutU8(s->TightCutoffEnabled[0], *p_write->source);
                break;
            }
            case 2: //threshold
                util_PutFloat(s->TightCutOff[0], *(float32 *)(void *)p_write->source);
                break;
            default:
                fferr = E_FB_PARA_CHECK;
                break;
        }
    }
    if(fferr == E_OK)
    {
        len = MN_OFFSETOF(Req_WriteSettings_t, endmark)
            + MN_OFFSETOF(Req_WritePositionControlLimits_t, endmark);
        fferr = mn_HART_acyc_cmd(171, from, len, to);
    }
    if(fferr == E_OK && flag != FALSE)
    {

        // Store out parameter position_limits in non-volatile RAM ------------
        p_write_loc.rel_idx       = REL_IDX_PTB_POSITION_LIMITS;
        p_write_loc.subindex      = 2;
        p_write_loc.length        = sizeof(u8);
        p_write_loc.source        = FALSE;
        if (appl_check_hm_state() == HM_RUN )
        {
            p_write_loc.startup_sync = FALSE;
        } else
        {
            p_write_loc.startup_sync = TRUE;
        }

        fferr = fbs_write_param_loc (p_block_instance, &p_write_loc);
    }

    return fferr;
}

/** \brief Writes Cut-off lo configuration
NOTE: apprently, p_write->source is filled in correct order but without alignment padding
*/
fferr_t ffctllim_WriteCutoffLo
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    const T_FBIF_WRITE_DATA *p_write,
    void *from,
    void *to
)
{
    T_FBS_WRITE_PARAM_LOCAL  p_write_loc;
    u8                       len;
    u8                       flag = FALSE;
    fferr_t fferr = ctllim_Populate(NULL, NULL, NULL, from, to);

    if(fferr == E_OK)
    {
        Req_WritePositionControlLimits_t *s1 = (void *)((u8 *)from
                       + MN_OFFSETOF(Req_ReadSettings_t, endmark));
        ControlLimitsConf_t *s = (void *)s1->ControlLimitsConf[0];
        switch(p_write->subindex)
        {
            case 0: //everything)
            {
                //if the tightshutoff is enabled, clear the position limits.Enable_lo
                if (*p_write->source != FALSE)
                {
                    util_PutU8(s->PositionLimitsEnabled[Xlow], FALSE);
                    flag = TRUE;
                }
                util_PutU8(s->TightShutoffEnabled[0], *p_write->source);
                util_PutFloat(s->TightShutOff[0], mn_pull_float(p_write->source+1));
                break;
            }
            case 1: //enable/disable
            {
                //if the tightshoutoff is enabled, clear the position limits.Enable_hi
                if (*p_write->source != FALSE)
                {
                    util_PutU8(s->PositionLimitsEnabled[Xlow], FALSE);
                    flag = TRUE;
                }
                util_PutU8(s->TightShutoffEnabled[0], *p_write->source);
                break;
            }
            case 2: //threshold
                util_PutFloat(s->TightShutOff[0], *(float32 *)(void *)p_write->source);
                break;
            default:
                fferr = E_FB_PARA_CHECK;
                break;
        }
    }
    if(fferr == E_OK)
    {
        len = MN_OFFSETOF(Req_WriteSettings_t, endmark)
            + MN_OFFSETOF(Req_WritePositionControlLimits_t, endmark);
        fferr = mn_HART_acyc_cmd(171, from, len, to);
    }
    if(fferr == E_OK && flag != FALSE)
    {
        // Store out parameter Position_limits in non-volatile RAM ------------
        p_write_loc.rel_idx       = REL_IDX_PTB_POSITION_LIMITS;
        p_write_loc.subindex      = 3;
        p_write_loc.length        = sizeof(u8);
        p_write_loc.source        = FALSE;
        if (appl_check_hm_state() == HM_RUN )
        {
            p_write_loc.startup_sync = FALSE;
        } else
        {
            p_write_loc.startup_sync = TRUE;
        }

        fferr = fbs_write_param_loc (p_block_instance, &p_write_loc);
    }
    return fferr;
}

/** \brief Writes The Rest of position limits configuration
NOTE: apprently, p_write->source is filled in correct order but without alignment padding
*/
fferr_t ffctllim_WriteControlLimits
(
    T_FBIF_BLOCK_INSTANCE *p_block_instance,
    const T_FBIF_WRITE_DATA *p_write,
    void *from,
    void *to
)
{
    T_FBS_WRITE_PARAM_LOCAL  p_write_loc;
    u8                       len;
    u8                       flaghi = FALSE;
    u8                       flaglow = FALSE;
    fferr_t fferr = ctllim_Populate(NULL, NULL, NULL, from, to);

    if(fferr == E_OK)
    {
        Req_WritePositionControlLimits_t *s1 = (void *)((u8 *)from + MN_OFFSETOF(Req_ReadSettings_t, endmark));
        ControlLimitsConf_t *s = (void *)s1->ControlLimitsConf[0];
        switch(p_write->subindex)
        {
            case 0: //everything)
            {
                //if the position limits.Enable_hi is enabled, clear the tightcutoff
                if (p_write->source[1] != FALSE)
                {
                    util_PutU8(s->TightCutoffEnabled[0], FALSE);
                    flaghi = TRUE;
                }
                //if the position limits.Enable_low is enabled, clear the tightshutoff
                if (p_write->source[2] != FALSE)
                {
                    util_PutU8(s->TightShutoffEnabled[0], FALSE);
                    flaglow = TRUE;
                }
                util_PutU8(s->PositionLimitsProtected[0], p_write->source[0]);
                util_PutU8(s->PositionLimitsEnabled[Xhi], p_write->source[1]);
                util_PutU8(s->PositionLimitsEnabled[Xlow], p_write->source[2]);
                util_PutFloat(s->PositionLimits[Xhi], mn_pull_float(&p_write->source[3]));
                util_PutFloat(s->PositionLimits[Xlow], mn_pull_float(&p_write->source[7]));
                util_PutU8(s->PositionRateLimitsEnabled[Xhi], p_write->source[11]);
                util_PutU8(s->PositionRateLimitsEnabled[Xlow], p_write->source[12]);
                util_PutFloat(s->PositionRateLimits[0], mn_pull_float(&p_write->source[13]));
                break;
            }
            case 1: //protect/unprotect
            {
                util_PutU8(s->PositionLimitsProtected[0], p_write->source[0]);
                break;
            }
            case 2: //high lim enabled/disabled
            {
                if (p_write->source[0] != FALSE)
                {
                    util_PutU8(s->TightCutoffEnabled[0], FALSE);
                    flaghi = TRUE;
                }
                util_PutU8(s->PositionLimitsEnabled[Xhi], p_write->source[0]);
                break;
            }
            case 3: //low lim enabled/disabled
            {
                if (p_write->source[0] != FALSE)
                {
                    util_PutU8(s->TightShutoffEnabled[0], FALSE);
                    flaglow = TRUE;
                }
                util_PutU8(s->PositionLimitsEnabled[Xlow], p_write->source[0]);
                break;
            }
            case 4: //high lim
                util_PutFloat(s->PositionLimits[Xhi], mn_pull_float(&p_write->source[0]));
                break;
            case 5: //low lim
                util_PutFloat(s->PositionLimits[Xlow], mn_pull_float(&p_write->source[0]));
                break;
            case 6: //low rate lim enabled/disabled
                util_PutU8(s->PositionRateLimitsEnabled[Xhi], p_write->source[0]);
                break;
            case 7: //high rate lim enabled/disabled
                util_PutU8(s->PositionRateLimitsEnabled[Xlow], p_write->source[0]);
                break;
            case 8: //rate lim
                util_PutFloat(s->PositionRateLimits[0], mn_pull_float(&p_write->source[0]));
                break;
            default:
                fferr = E_FB_PARA_CHECK;
                break;
        }
    }
    if(fferr == E_OK)
    {
        len = MN_OFFSETOF(Req_WriteSettings_t, endmark) + MN_OFFSETOF(Req_WritePositionControlLimits_t, endmark);
        fferr = mn_HART_acyc_cmd(171, from, len, to);
    }
    if (fferr == E_OK)
    {
        //if the final_value_cutoff_hi is changed, write it to local.
        if (flaghi != FALSE)
        {
            // Store out parameter final_value_cutoff_hi in non-volatile RAM --
            p_write_loc.rel_idx       = REL_IDX_PTB_FINAL_VALUE_CUTOFF_HI;
            p_write_loc.subindex      = 1;
            p_write_loc.length        = sizeof(u8);
            p_write_loc.source        = FALSE;
            if (appl_check_hm_state() == HM_RUN )
            {
                p_write_loc.startup_sync = FALSE;
            } else
            {
                p_write_loc.startup_sync = TRUE;
            }
            fferr = fbs_write_param_loc (p_block_instance, &p_write_loc);
        }

        //if the final_value_cutoff_lo is changed, write it to local.
        if (flaglow != FALSE)
        {
            // Store out parameter final_value_cutoff_lo in non-volatile RAM --
            p_write_loc.rel_idx       = REL_IDX_PTB_FINAL_VALUE_CUTOFF_LO;
            p_write_loc.subindex      = 1;
            p_write_loc.length        = sizeof(u8);
            p_write_loc.source        = FALSE;
            if (appl_check_hm_state() == HM_RUN )
            {
                p_write_loc.startup_sync = FALSE;
            } else
            {
                p_write_loc.startup_sync = TRUE;
            }
            fferr = fbs_write_param_loc (p_block_instance, &p_write_loc);
        }

    }

    return fferr;
}


/* This line marks the end of the source */
