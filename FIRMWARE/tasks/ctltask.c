/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ctltask.c
    \brief Collection of chores to do late system initialization and launch
           the control algorithm.

           The actual control algorithm voodoo is control_Control() in
           control.c

     CPU: Any

    OWNER: DZ EP?
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
//#define NDEBUG
#include "projectdef.h"
#include "mnwrap.h"
#include "oswrap.h"
#include "ad.h"
#include "process.h"
#include "devicemode.h"
#include "control.h"
#include "power.h"
#include "timebase.h"
#include "fullthrottle.h"
#include "bufferhandler.h"
#include "sysiolopwr.h"

//#include "mn_instrum.h"
#ifndef CTLTASK_END_HOOK
#define CTLTASK_END_HOOK() /*nothing*/
#endif //CTLTASK_END_HOOK

static u8_least ktrDisp = 0U; //counter of skips of the cycle task
static void ctltask_TriggerCycleTask(void)
{		
	ktrDisp++;
    if (ktrDisp >= CYCLE_TASK_DIVIDER)
    {
        ktrDisp = 0;
        oswrap_PostTaskSemaphore(TASKID_CYCLE);
    }
}

void control_ControlTask(void *arg)
{
    control_RunOnce();
    // error_SetFault(FAULT_LOW_POWER_APP);
//    bsp_WDSignpost();               // inhouse debugging
    for (;;)
    {
#ifdef OLD_OSPORT
        mn_waitsem(arg);
#else
        UNUSED_OK(arg);
        mn_waitsem(TASKID_CONTROL);                    // wait for timer to post semaphore - every 15 milliseconds
#endif

//      bsp_WDSignpost();               // inhouse debugging
#if GREEN_BOARD_BUTTONS
        BTN_Poll();
#endif
        mode_GuardControlMode(); //to take care of disasters that happened since last checked
        (void)bios_MeasureAd();
        full_FullThrottle(); //TFS:9578 - Take Full Open/Close out of control
        sysio_Handle_LowPowerLimiting(); //FBO interface layer, needs to run no matter what Control is doing
        control_Control();
#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
        //-------------------------------------------------------
        // LCX - LED Updates...
        LED_PeriodicUpdate();
        //-------------------------------------------------------
#endif //FEATURE_LOCAL_UI

        power_CheckClearAssert();

        buffer_SampleAllData();

		ctltask_TriggerCycleTask();

        CTLTASK_END_HOOK();

        process_SetWatchdogFlag(WD_CONTROL_TASK_RUN);
    }
}

/* This line marks the end of the source */
