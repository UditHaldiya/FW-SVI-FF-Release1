/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup Timer Timer tick handler (5 millisecond)
\brief Device driver for timer tick interrupts (5 millisecond)
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Timer_Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_BIOS.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file timer.c
    \brief Device driver for timer tick interrupts (5 millisecond)

    CPU: Any (NO hardware dependencies in this file)

    OWNER: Ernie Price

    \ingroup Timer
*/

#include "mnwrap.h"             //for intrinsic functions used by the OS port
#include "timebase.h"
#include "oswrap.h"
#include "timer.h"
#include "hart.h"

static  u32 ktr5ms, ktrProc;
static u64 tickCounter  = (u64)0;      // main tick counter
//                              0123456701234567
#define TICK_TEST_BASE ((u64)(0x5555555555555555ULL)) //!< test value offset (so that stuck RAM may be caught)
static u64 tickCounter2 = TICK_TEST_BASE;      // copy for integrity

#define CTL_START  1u
#define PRC_START  4u

/** \brief Check the tick timer integrity
*/
void timer_IntegrityCheck(void)
{
    MN_ENTER_CRITICAL();
        MN_RT_ASSERT((tickCounter+TICK_TEST_BASE) == tickCounter2);
    MN_EXIT_CRITICAL();
}

/** \brief Suspend the Process task for the specified number of ticks. The
    task will be suspended pending its sempahore.  The semaphore will be
    posted after the specified numer of ticks.

  \param[in] ticks - the wait time in 5 ms ticks
*/
void bios_WaitTicks(u32 ticks)
{
    MN_DBG_ASSERT(oswrap_IsContext(TASKID_PROCESS));        // make sure it is the Process task calling
    ktrProc = ticks;
    mn_waitforsem(TASKID_PROCESS);
}

/** \brief Process a 5 millisecond interrupt event

    This function is entered every 5 milliseconds from the timer tick.  The
    Control and Process task timers are monitored here.
*/
//extern u8 inwd;
void bios_ProcessTick (void)
{
    u32 flag;

    // Let HART do its thing
    maintain_timers();

    // increment the main counter
    tickCounter++;

    // increment the copy
    tickCounter2++;

    flag = 0;

    // if Process task sleeping ...
    if (ktrProc != 0u)
    {
        // count it down and see if it is ready to awaken
        if (--ktrProc == 0u)
        {
            flag |= PRC_START;
        }
    }

    // count the Control task divider
    ktr5ms++;
    if (ktr5ms >= CTRL_TASK_DIVIDER)
    {
        ktr5ms = 0u;
        flag  |= CTL_START;
    }

    // if any task needs to run, tell the OS
    if (flag != 0u)
    {
        oswrap_IntrEnter();

        // if ready, wake the Process task
        if ((flag & PRC_START) != 0u)
        {
            oswrap_PostTaskSemaphore(TASKID_PROCESS);
        }

        // if ready, wake the Control task
        if ((flag & CTL_START) != 0u)
        {
            oswrap_PostTaskSemaphore(TASKID_CONTROL);
        }
        oswrap_IntrExit();
    }
}

/** \brief Return the current value of the 5 ms tick counter

  \return u32 - the current tick counter LSW.
*/
u32 bios_GetTimer0Ticker(void)
{
   // truncate to 32 bits for this return
    return (u32)tickCounter;
}

/** \brief Return the current 64 bit value of the 5 ms tick counter

  \return u64 - the current tick counter.
*/
MN_DECLARE_API_FUNC(bios_GetTimer0TickerLong)
u64 bios_GetTimer0TickerLong(void)
{
    return tickCounter;
}

/* This line marks the end of the source */
