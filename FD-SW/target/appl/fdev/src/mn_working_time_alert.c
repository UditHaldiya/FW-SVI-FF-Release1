/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_working_time_alert.c
    \brief "working_time_alert parameters of transduser block" handler

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: mn_working_time_alert.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/

#include <softing_base.h>
#include <ptb.h>
#include "mn_working_time_alert.h"

#define WORING_TIME_ALERT_DISABLE       0u
#define WORING_TIME_ALERT_ENABLE        1u

/* \brief This function will be called regularly to update the parameters of working time alert
    param in:
        p_PTB: pointer to the objects of transducer block
    return:
        void
*/
void UpdateWorkingTimeAlert(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBIF_PTB* p_PTB = p_block_instance->p_block_desc->p_block;

    if(WORING_TIME_ALERT_DISABLE != p_PTB->working_time_alert.enable)
    {
        p_PTB->working_time_alert.total_time = p_PTB->working_times.since_reset;

        if(p_PTB->working_time_alert.total_time >= p_PTB->working_time_alert.alert_point)
        {
            p_PTB->working_time_alert.alert = WORING_TIME_ALERT_ENABLE;
            p_PTB->working_time_alert.historic_alert = WORING_TIME_ALERT_ENABLE;
        }

        T_FBS_WRITE_PARAM_LOCAL write_loc;
        write_loc.rel_idx = REL_IDX_PTB_WORKING_TIME_ALERT;
        /* Store out parameter in non-volatile RAM ---------------------- */
        write_loc.subindex      = 0;
        write_loc.length        = MN_OFFSETOF(_WORKING_TIME_ALERT, enable) + sizeof(p_PTB->working_time_alert.enable);
        write_loc.source        = (USIGN8 *)&p_PTB->working_time_alert;
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
    else
    {
        p_PTB->working_time_alert.total_time = 0;
    }
}
