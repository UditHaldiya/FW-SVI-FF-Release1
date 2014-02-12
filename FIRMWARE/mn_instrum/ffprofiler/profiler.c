/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file profiler.c
    \brief MN code instrumentation. OS tasks profilng

    CPU: Any (no instrumentation)

    OWNER: AK

    $Archive: /MNCB/Dev/FIRMWARE/mn_instrum/profiler/profiler.c $
    $Date: 2/18/11 4:16p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: profiler.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/18/11    Time: 4:16p
 * Updated in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * Added capability to add extra time load to a task
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/27/11    Time: 10:31p
 * Updated in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * TFS:5505 Step 3 - (Trunk) update profiling code for HART interface and
 * Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/01/09    Time: 11:31a
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * Profiler instrumentation (tasks and critical sections)
*/
#include "mnwrap.h"
#include "oswrap.h"
#include "mn_instrum.h"
#include "mn_coinstrum.h"
//#include "lpc2114io.h"
#include "stmcommon.h"

typedef struct TaskProfile_t
{
    u16 start;
    u16 end;
    u16 max_duration;
} Atomic_ TaskProfile_t;

static TaskProfile_t TaskProfile[(u32)TASKID_ISPCOMM+1];
static bool_t Atomic_ been_there[NELEM(TaskProfile)];

#define bios_GetHighResolutionTimer instrum_GetHighResolutionTimer

static bool_t Atomic_ need_init=true; //lint !e633 spurious warning because of Atomic_
void profile_TimeStamp(taskid_t taskid)
{
#if 1
    if(need_init)
    {
        need_init = false;  //lint !e633 spurious warning because of Atomic_
        PRF_InitTimer();
        //Before timer init, any capture of critical sections is useless
        CritSectStat.maxlen = 0;
    }
#endif
    TaskProfile[taskid].start = bios_GetHighResolutionTimer();
    if((u8)taskid == profiler_task_id)
    {
        u16 diff;
        do
        {
            diff = (bios_GetHighResolutionTimer() - TaskProfile[taskid].start);
        } while(diff < profiler_extra_task_load);
    }
}


u32 profiler_extra_task_load = 0;
u8 profiler_task_id = 0;

void profile_MeasureTime(taskid_t taskid)
{
    u16 stamp = bios_GetHighResolutionTimer();
    if(been_there[taskid])
    {
        u16 dur = stamp - TaskProfile[taskid].start;
        //dur &= 0x00FFFFFFU;
        TaskProfile[taskid].end = stamp;
        if(TaskProfile[taskid].max_duration < dur)
        {
            TaskProfile[taskid].max_duration = dur;
        }
    }
    else
    {
        //Initialize
        TaskProfile[taskid].start = stamp;
        been_there[taskid] = true;  //lint !e633 spurious warning because of Atomic_
    }
}

void profile_ReadAndClear(u32 results[])
{
    CONST_ASSERT((u32)TASKID_IDLE==0U);
    MN_ENTER_CRITICAL();
        for(u8_least i=1; i<NELEM(TaskProfile); i++) //skipping idle task
        {
            results[i] = TaskProfile[i].max_duration;
            TaskProfile[i].max_duration = 0U;
        }
    MN_EXIT_CRITICAL();
}

/* This line marks the end of the source */
