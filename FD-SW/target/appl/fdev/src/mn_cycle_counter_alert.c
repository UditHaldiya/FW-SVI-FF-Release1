/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_cycle_counter_alert.c
    \brief "mn_cycle_counter_alert parameters of transduser block" handler

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: mn_cycle_counter_alert.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/

#include <softing_base.h>
#include <ptb.h>
#include "numutils.h"
#include "mn_cycle_counter_alert.h"

#define MAX_CYCLE_DEADBAND                    (10.0f)
#define MIN_CYCLE_DEADBAND                    (0.05f)

/* \brief synchronize the value of deadband in CYCLE_COUNTER_ALERT_A and CYCLE_COUNTER_ALERT_B
    param in:
        p_PTB: pointer to the objects of transducer block
        p_write: pointer to write structure
    return:
        FF error
*/
fferr_t SyncCycleDeadBand(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;
    float32 deadband = 0.0f;
    float32 tempcomp2;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if(3 == p_write->subindex || 0 == p_write->subindex)
    {
        switch (p_write->subindex)
        {
            case 3:
            {
                deadband = *((float32*)(void*)p_write->source);
                break;
            }
            case 0:
            {
                _CYCLE_COUNTER_ALERT* pAlert = (void*)p_write->source;
                deadband = pAlert->deadband_ab;
                switch (p_write->rel_idx)
                {
                case REL_IDX_PTB_CYCLE_COUNTER_A_ALERT:
                    tempcomp2 = ABS(pAlert->deadband_ab - p_PTB->cycle_counter_a_alert.deadband_ab);
                    //if only ALERT is changed, manully oprate the ST_REV.
                    if ((pAlert->alert_point == p_PTB->cycle_counter_a_alert.alert_point)\
                        && (tempcomp2 < FLOAT_POSITIVE_SMALL_DIFF)\
                        && (pAlert->historic_alert == p_PTB->cycle_counter_a_alert.historic_alert)\
                        && (pAlert->enable == p_PTB->cycle_counter_a_alert.enable)\
                        && ((bool_t)pAlert->alert != (bool_t)p_PTB->cycle_counter_a_alert.alert))
                    {
                        //mannully decrese the ST_REV number.
                        IncrementStaticRevision(p_block_instance, -2);
                    }
                    break;
                case REL_IDX_PTB_CYCLE_COUNTER_B_ALERT:
                    tempcomp2 = ABS(pAlert->deadband_ab - p_PTB->cycle_counter_b_alert.deadband_ab);
                    //if only ALERT is changed, manully oprate the ST_REV.
                    if ((pAlert->alert_point == p_PTB->cycle_counter_b_alert.alert_point)\
                        && (tempcomp2 < FLOAT_POSITIVE_SMALL_DIFF)\
                        && (pAlert->historic_alert == p_PTB->cycle_counter_b_alert.historic_alert)\
                        && (pAlert->enable == p_PTB->cycle_counter_b_alert.enable)\
                        && ((bool_t)pAlert->alert != (bool_t)p_PTB->cycle_counter_b_alert.alert))
                    {
                        //mannully decrese the ST_REV number.
                        IncrementStaticRevision(p_block_instance, -2);
                    }
                    break;
                default:
                    break;
                }

                break;
            }
            default:
            {
            }
        }

        if(deadband > MAX_CYCLE_DEADBAND || deadband < MIN_CYCLE_DEADBAND)
        {
            fferr = E_FB_PARA_CHECK;
        }
        else
        {
            T_FBS_WRITE_PARAM_LOCAL write_loc;

            write_loc.subindex      = 3;
            write_loc.length        = sizeof(float32);
            write_loc.source        = (void*)&deadband;
            if (appl_check_hm_state() == HM_RUN )
            {
              write_loc.startup_sync = FALSE;
            }
            else
            {
              write_loc.startup_sync = TRUE;
            }

            if(REL_IDX_PTB_CYCLE_COUNTER_A_ALERT == p_write->rel_idx)
            {
                write_loc.rel_idx = REL_IDX_PTB_CYCLE_COUNTER_B_ALERT;

                return fbs_write_param_loc (p_block_instance, &write_loc);
            }
            else
            {
                write_loc.rel_idx = REL_IDX_PTB_CYCLE_COUNTER_A_ALERT;

                return fbs_write_param_loc (p_block_instance, &write_loc);
            }
        }
    }
    if(4 == p_write->subindex)
    {
        switch (p_write->rel_idx)
        {
        case REL_IDX_PTB_CYCLE_COUNTER_A_ALERT:
            if(p_write->source[0] != p_PTB->cycle_counter_a_alert.alert)
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        case REL_IDX_PTB_CYCLE_COUNTER_B_ALERT:
            if(p_write->source[0] != p_PTB->cycle_counter_b_alert.alert)
            {
                //mannully decrese the ST_REV number.
                IncrementStaticRevision(p_block_instance, -2);
            }
            break;
        default:
            break;
        }
    }
    return fferr;
}


