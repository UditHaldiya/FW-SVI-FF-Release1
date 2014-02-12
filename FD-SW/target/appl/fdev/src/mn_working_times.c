/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_working_times.c
    \brief implement WORKING_TIMES parameter in TB block

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: mn_trend_utility.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/

#include <softing_base.h>
#include <ptb.h>
#include "time_defs.h"
#include "mn_working_time_alert.h"
#include "mn_working_times.h"

#define RES_RESET       0xFFFFFFFFu

static void UpdateSineReset(const T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;
    p_PTB->working_times.since_reset++;
}

static void UpdateTotalTime(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;
    p_PTB->working_times.total_time++;
    p_PTB->working_time_alert.total_time++;

    //save working_time.tatal_time
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    write_loc.rel_idx = REL_IDX_PTB_WORKING_TIMES;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 2;
    write_loc.length        = sizeof(p_PTB->working_times.total_time);
    write_loc.source        = (USIGN8 *)&(p_PTB->working_times.total_time);
    if (appl_check_hm_state() == HM_RUN)
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    (void)fbs_write_param_loc(p_block_instance,&write_loc);

    //save working_time_alert.total_time
    write_loc.rel_idx = REL_IDX_PTB_WORKING_TIME_ALERT;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 1;
    write_loc.length        = sizeof(p_PTB->working_time_alert.total_time);
    write_loc.source        = (USIGN8 *)&(p_PTB->working_time_alert.total_time);
    if (appl_check_hm_state() == HM_RUN)
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    (void)fbs_write_param_loc(p_block_instance,&write_loc);

    IncrementStaticRevision(p_block_instance, -2);
}

/* \brief resource block set reset flag
    param in:
            void
*/
void SetResetFlag(void)
{
    T_FBIF_BLOCK_INSTANCE * p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;

    p_PTB->working_times.since_reset = RES_RESET;

    T_FBS_WRITE_PARAM_LOCAL write_loc;
    write_loc.rel_idx = REL_IDX_PTB_WORKING_TIMES;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = sizeof(p_PTB->working_times.since_reset);
    write_loc.source        = (USIGN8 *)&(p_PTB->working_times.since_reset);
    if (appl_check_hm_state() == HM_RUN)
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    (void)fbs_write_param_loc(p_block_instance,&write_loc);
}

/* \brief set initial value for WORKING_TIMES
    param in:
        p_block_instance: pointer to function block
*/
void InitialWorkingTimes(const T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;

    p_PTB->working_times.since_reset = 0;

}

/* \brief update WORKING_TIMES periodically
    param in:
        p_block_instance: pointer to function block
*/
void UpdateWorkingTimes(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    u32         TimeDiff;
    u32         CurrentTime;
    static u32      LastTime = 0;

    CurrentTime = osif_get_time_in_ms();
    TimeDiff = TIME_DIFF(LastTime,CurrentTime);

    if (TimeDiff >= (TIME_INSECOND_ONEHOUR * NUMBER_OF_MS_IN_SECOND))
    {
        LastTime = CurrentTime;
        UpdateSineReset(p_block_instance);
        UpdateTotalTime(p_block_instance);
    }
}

