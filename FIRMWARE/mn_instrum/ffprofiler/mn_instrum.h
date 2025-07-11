/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_instrum.h
    \brief MN code instrumentation. Task profilng version

    CPU: Philips LPC21x4 (ARM7TDMI-S)

    OWNER: AK

    $Archive: /MNCB/Dev/FIRMWARE/mn_instrum/profiler/mn_instrum.h $
    $Date: 1/27/11 10:31p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: mn_instrum.h $
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
#ifndef MN_INSTRUM_H_
#define MN_INSTRUM_H_

#include "taskids.h"

extern void profile_TimeStamp(enum taskid_t taskid);
extern void profile_MeasureTime(enum taskid_t taskid);
#define INSTRUM_AFTER_SEMAPHORE(taskid) profile_TimeStamp(taskid)
#define INSTRUM_BEFORE_SEMAPHORE(taskid) profile_MeasureTime(taskid)

extern void profile_ReadAndClear(u32 results[]);


#endif //MN_INSTRUM_H_
/* This line marks the end of the source */
