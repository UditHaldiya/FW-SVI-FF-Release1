/* Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_instrum.h
    \brief MN code instrumentation. Critical section profilng version

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
#ifndef MN_COINSTRUM_H_
#define MN_COINSTRUM_H_

#define TIMER1_NOT_AVAILABLE //for app code to compile out

#define MN_ENTER_CRITICAL_HOOK(psw) \
    tick_t stamp; \
    stamp = instrum_GetHighResolutionTimer();


#define MN_EXIT_CRITICAL_HOOK(psw) \
    if((psw & CPU_NO_INTERRUPTS) != CPU_NO_INTERRUPTS) \
    { \
        stamp = instrum_GetHighResolutionTimer() - stamp; \
        if(stamp > CritSectStat.maxlen) \
        { \
            CritSectStat.maxlen = stamp; \
            CritSectStat.addr = mn_getPC(); \
        } \
    }

//An assembler routine returning its LR in R0
extern u32 mn_getPC(void);

typedef struct CritSectStat_t
{
    tick_t maxlen; //max length of a critical section so far
    u32 addr; //address of the end of the longest critical section
} CritSectStat_t;

extern CritSectStat_t CritSectStat; //critical sections profiler

extern tick_t instrum_GetHighResolutionTimer(void);

extern u32 profiler_extra_task_load;
extern u8 profiler_task_id;

#endif //MN_COINSTRUM_H_
/* This line marks the end of the source */
