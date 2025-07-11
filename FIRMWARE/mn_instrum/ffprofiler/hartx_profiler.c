/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hartx_profiler.c
    \brief HART glue layer for profiling instrumentation

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/mn_instrum/profiler/hartx_profiler.c $
    $Date: 7/01/11 5:16p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hartx_profiler.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/01/11    Time: 5:16p
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * Hart 6 flavor of hart_profiler.c (based on its ver. 2)
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
#include "hartdefx.h"
#include "bufutils.h"

s32 main_GetCpuFree(void); //not used in this codebase yet

/**
\brief Read activation data
*/
s8_least HartCmd_ReadProfilingData(const void *src, void *dst, HartCmdParams_t *params)
{
    UNUSED_OK(src);
    UNUSED_OK(params);
    Rsp_ReadProfilingData_t *d = dst;

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

    util_PutU8(d->FreeCPUtimeH[0], (u8)main_GetCpuFree());

    ProfilerConfiguration_t *d1 = (void *)d->ProfilerConfiguration[0];
    util_PutU8(d1->ProfilerTaskID[0], profiler_task_id);
    util_PutU32(d1->ProfilerExtratimeload[0], profiler_extra_task_load);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least HartCmd_WriteProfilerConfiguration(const void *src, void *dst, HartCmdParams_t *params)
{
    const Rsp_WriteProfilerConfiguration_t *s1 = src;
    const ProfilerConfiguration_t *s = (const void *)s1->ProfilerConfiguration[0];
    MN_ENTER_CRITICAL();
        profiler_task_id = util_GetU8(s->ProfilerTaskID[0]);
        profiler_extra_task_load = util_GetU32(s->ProfilerExtratimeload[0]);
    MN_EXIT_CRITICAL();
    UNUSED_OK(dst);
    UNUSED_OK(params);
    return HART_NO_COMMAND_SPECIFIC_ERRORS; //response is copied automatically
}

/* This line marks the end of the source */
