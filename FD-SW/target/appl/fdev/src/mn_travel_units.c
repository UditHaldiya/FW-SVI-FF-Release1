/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_travel_units.c
    \brief functions about parameter TRAVEL_UNITS

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>
//Glue headers last
#include "mnhart2ff.h"
#include "mn_travel_units.h"

#define TRAVEL_RANGE_MIN    (0.1f)
#define TRAVEL_RANGE_MAX    (10000.0f)
#define FF_UNITS_RAD        (1004u)
#define FF_UNITS_DEG        (1005u)
#define FF_UNITS_CM         (1012u)
#define FF_UNITS_MM         (1013u)
#define FF_UNITS_INCH       (1019u)
#define FF_UNITS_PERC       (1342u)

static fferr_t  SaveTravelAccumulationAlertAB(T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    fferr_t ret;

    write_loc.rel_idx = REL_IDX_PTB_TRAVEL_ACCUMULATION_A_ALERT;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = MN_OFFSETOF(_TRAVEL_ALERT, enable) + sizeof(p_PTB->travel_accumulation_a_alert.enable);
    write_loc.source        = (USIGN8 *)(&p_PTB->travel_accumulation_a_alert);
    if (appl_check_hm_state() == HM_RUN )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    ret = fbs_write_param_loc (p_block_instance,&write_loc);
    if(E_OK != ret)
    {
        return E_ERROR;
    }

    write_loc.rel_idx = REL_IDX_PTB_TRAVEL_ACCUMULATION_B_ALERT;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = MN_OFFSETOF(_TRAVEL_ALERT, enable) + sizeof(p_PTB->travel_accumulation_a_alert.enable);
    write_loc.source        = (USIGN8 *)(&p_PTB->travel_accumulation_b_alert);
    if (appl_check_hm_state() == HM_RUN )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    ret = fbs_write_param_loc (p_block_instance,&write_loc);
    if(E_OK != ret)
    {
        return E_ERROR;
    }
    return E_OK;
}

/* \brief write travel parameter
    param in:
        p_block_instance: pointer to function block
    return:
        FF error
*/
fferr_t WriteTravel(T_FBIF_BLOCK_INSTANCE* p_block_instance,  const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;
    float32 rate = 0.0f;
    float32 t_value;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if (p_PTB->mode_blk.target != MODE_OS
        && p_PTB->mode_blk.target != MODE_MAN)
    {
        fferr = E_FB_WRONG_MODE;
        return fferr;
    }
    switch (p_write->subindex)
    {
        case 0:
        {
            _TRAVEL* p_travel = (_TRAVEL*)(void*)p_write->source;
            if((TRAVEL_RANGE_MIN > p_travel->component_0 || TRAVEL_RANGE_MAX < p_travel->component_0)
               || (FF_UNITS_RAD != p_travel->component_1 &&
                   FF_UNITS_DEG != p_travel->component_1 &&
                   FF_UNITS_MM != p_travel->component_1 &&
                   FF_UNITS_CM != p_travel->component_1 &&
                   FF_UNITS_INCH != p_travel->component_1 &&
                   FF_UNITS_PERC != p_travel->component_1
                   )
               )
            {
                fferr = E_FB_PARA_CHECK;
            }
            else
            {
                rate = p_travel->component_0 / p_PTB->travel.component_0;
            }
            break;
        }
        case 1:
        {
            float32* p_range = (float32*)(void*)p_write->source;
            if(TRAVEL_RANGE_MIN > *p_range || TRAVEL_RANGE_MAX < *p_range)
            {
                fferr = E_FB_PARA_CHECK;
            }
            else
            {
                rate = (*p_range) / p_PTB->travel.component_0;
            }
            break;
        }
        case 2:
        {
            u16* p_units_index = (u16*)(void*)p_write->source;
            if(FF_UNITS_RAD != *p_units_index &&
               FF_UNITS_DEG != *p_units_index &&
               FF_UNITS_MM != *p_units_index &&
               FF_UNITS_CM != *p_units_index &&
               FF_UNITS_INCH != *p_units_index &&
               FF_UNITS_PERC != *p_units_index)
            {
                fferr = E_FB_PARA_CHECK;
            }
            break;
        }
        default:
        {
            fferr = E_FB_PARA_CHECK;
            break;
        }
    }
    if(E_OK == fferr && (p_write->subindex == 0 || p_write->subindex == 1))
    {
        t_value = (rate * (float32)p_PTB->travel_accumulation_a_alert.travel_accumulation);
        p_PTB->travel_accumulation_a_alert.travel_accumulation = (u32)t_value;
        t_value = (rate * (float32)p_PTB->travel_accumulation_b_alert.travel_accumulation);
        p_PTB->travel_accumulation_b_alert.travel_accumulation = (u32)t_value;
        t_value = (rate * (float32)p_PTB->travel_accumulation_a_alert.alert_point);
        p_PTB->travel_accumulation_a_alert.alert_point = (u32)t_value;
        t_value = (rate * (float32)p_PTB->travel_accumulation_b_alert.alert_point);
        p_PTB->travel_accumulation_b_alert.alert_point = (u32)t_value;
        p_PTB->travel_accumulation_a_alert.deadband_ab = (rate * (float32)p_PTB->travel_accumulation_a_alert.deadband_ab);
        p_PTB->travel_accumulation_b_alert.deadband_ab = (rate * (float32)p_PTB->travel_accumulation_b_alert.deadband_ab);

        fferr = SaveTravelAccumulationAlertAB(p_block_instance);
    }

    return fferr;
}

