/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_activate.c
    \brief HART glue layer for control (position setpoint) limits

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/mn_instrum/profiler/hart_profiler.c $
    $Date: 2/18/11 4:16p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_profiler.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/18/11    Time: 4:16p
 * Updated in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * Added capability to add extra time load to a task
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/27/11    Time: 10:15p
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * HART support of profiler instrumentation FBO TFS:5505 Step 3
*/
#include "mnwrap.h"
#include "oswrap.h"
#include "mn_instrum.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"

/**
\brief Read activation data
*/
s8_least hartcmd_ReadProfilingData(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadProfilingData_t *d = (void *)dst;

    u32 profile[(size_t)TASKID_PROCESS+1U];
    profile_ReadAndClear(profile);
    util_PutU32(d->Longestcontroltask[0], profile[TASKID_CONTROL]);
    util_PutU32(d->Longestcycletask[0], profile[TASKID_CYCLE]);
    util_PutU32(d->LongestHARTtask[0], profile[TASKID_HART]);
    util_PutU32(d->Longestprocesssegment[0], profile[TASKID_PROCESS]);

    MN_ENTER_CRITICAL();
        util_PutU32(d->Addressofcritsect[0], CritSectStat.addr);
        util_PutU32(d->Longestcritsect[0], CritSectStat.maxlen);
        CritSectStat.maxlen = 0U;
    MN_EXIT_CRITICAL();

    util_PutU8(d->FreeCPUtimeH[0], selftest_GetStat()->percentCpuFree);

    ProfilerConfiguration_t *d1 = (void *)d->ProfilerConfiguration[0];
    util_PutU8(d1->ProfilerTaskID[0], profiler_task_id);
    util_PutU32(d1->ProfilerExtratimeload[0], profiler_extra_task_load);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteProfilerConfiguration(const u8 *src, u8 *dst)
{
    const Rsp_WriteProfilerConfiguration_t *s1 = (const void *)src;
    const ProfilerConfiguration_t *s = (const void *)s1->ProfilerConfiguration[0];
    MN_ENTER_CRITICAL();
        profiler_task_id = util_GetU8(s->ProfilerTaskID[0]);
        profiler_extra_task_load = util_GetU32(s->ProfilerExtratimeload[0]);
    MN_EXIT_CRITICAL();
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS; //response is copied automatically
}

/* This line marks the end of the source */
