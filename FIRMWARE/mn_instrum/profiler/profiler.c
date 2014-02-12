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

    $Archive: /MNCB/Dev/FIRMWARE/includes/oswrap.h $
    $Date: 3/29/09 12:55a $
    $Revision: 42 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: oswrap.h $
*/
#include "mnwrap.h"
#include "oswrap.h"
#include "mn_instrum.h"
#include "mn_coinstrum.h"
#include "timer.h"
#include "lpc2114io.h"

typedef struct TaskProfile_t
{
    u32 start;
    u32 duration;
    u32 max_duration;
} TaskProfile_t;

u32 profiler_extra_task_load = 0;
u8 profiler_task_id = 0;

static TaskProfile_t TaskProfile[TASKID_PROCESS+1];
static bool_t been_there[NELEM(TaskProfile)];

/** \brief Return the current value of the high resolution timer

  \return u32 - the current value of Timer1 upcounter.
*/
static u32 bios_GetHighResolutionTimer(void)
{
   // truncate to 32 bits for this return
    return T1->TC;
}

/** \brief Return the current value of the high resolution timer

  \return u32 - the current value of Timer1 upcounter.
*/
tick_t instrum_GetHighResolutionTimer(void)
{
    return bios_GetHighResolutionTimer();
}

void profile_TimeStamp(taskid_t taskid)
{
    TaskProfile[taskid].start = bios_GetHighResolutionTimer();
}

void profile_MeasureTime(taskid_t taskid)
{
    u32 stamp = bios_GetHighResolutionTimer();
    if(been_there[taskid])
    {
        u32 dur = stamp - TaskProfile[taskid].start;
        TaskProfile[taskid].duration = dur;
        if(TaskProfile[taskid].max_duration < dur)
        {
            TaskProfile[taskid].max_duration = dur;
        }
    }
    else
    {
        //Initialize
        TaskProfile[taskid].start = stamp;
        been_there[taskid] = true;
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
