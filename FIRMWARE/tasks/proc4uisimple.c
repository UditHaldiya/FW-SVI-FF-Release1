/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file proc4uisimple.c
    \brief Necessary wrappers for normal mode processes

    CPU: Any

    OWNER: AK

    \ingroup Process
*/

#include "mnwrap.h"
#include "proc4uisimple.h"
#include "devicemode.h"
#include "procshims.h"
#include "osadjust.h"

/** \brief A wrapper to run a normal mode process by switching to setup and back
\param proc - a process to run
*/
static procresult_t process_WrapNormalModeProcess(s16 *procdetails, procresult_t (*proc)(s16 *procdetails))
{
    procresult_t ret = PROCRESULT_OK;
    MN_ENTER_CRITICAL();
#ifdef OLD_DEVMODE
        ModeData_t mode;
        mode = *mode_GetMode();
        if (mode.submode != SUBMODE_OOS_NORMAL)
#else
        if(mode_GetMode() != MODE_SETUP)
#endif
        {   // Not Normal OOS
            ret = PROCRESULT_CANCELED;
        }
        if (ret == PROCRESULT_OK)
        {
            ErrorCode_t err;
#ifdef OLD_DEVMODE
            err = mode_SetOOSModeEx(SUBMODE_OOS_NORMAL, false);
#else
            err = mode_SetMode(MODE_SETUP);
#endif
            if (err != ERR_OK)
            {
                ret = PROCRESULT_CANCELED;
            }
        }
    MN_EXIT_CRITICAL();

    if (ret == PROCRESULT_OK)
    {
        ret = proc(procdetails);
#ifdef OLD_DEVMODE
        (void)mode_SetOperateMode();
        /* EXPLANATION and an item for code review of devicemode.c:
        mode_SetOperateMode can only fail if a failsafe or a low power
        condition results during the process.
        In these cases, we are OK to stay where we are.
        */
#else
        (void)mode_SetMode(MODE_OPERATE); //no bumpless xfer even if configured
        /* EXPLANATION and an item for code review of devicemode.c:
        mode_SetMode can only fail if a failsafe
        condition results during the process.
        In these cases, we are OK to stay where we are.
        */
#endif
    }
    return ret;
}

//TFS:4288
procresult_t osadj_RunOpenStopAdjustment_SHIM(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    osadj_RunOpenStopAdjustment();  /** TFS:5241 */
    return PROCRESULT_OK;
}

//TFS:4288
procresult_t osadj_RunOpenStopAdjustment_Normal(s16 *procdetails)
{   /** TFS:4293 */
    return process_WrapNormalModeProcess(procdetails, osadj_RunOpenStopAdjustment_SHIM);
}

procresult_t cal_RunFindStops_Normal(s16 *procdetails)
{
    return process_WrapNormalModeProcess(procdetails, cal_RunFindStops_SHIM);
}


procresult_t tune_Run_Selftune_Normal(s16 *procdetails)
{
    return process_WrapNormalModeProcess(procdetails, tune_Run_Selftune_SHIM);
}

/* This line marks the end of the source */
