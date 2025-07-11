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

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/cputraps/mn_instrum.h $
    $Date: 6/10/11 2:26p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: mn_instrum.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/10/11    Time: 2:26p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/cputraps
 * TFS:6584 Instrumentation for CPU traps
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

#ifdef __STDC__
//Stuff for C
#define INSTRUM_AFTER_SEMAPHORE(taskid) /* */
#define INSTRUM_BEFORE_SEMAPHORE(taskid) /* */

extern void profile_ReadAndClear(u32 results[]);

#else
//We assume Assembler until we deploy C++
#define INIT_INSTRUM INIT_INSTRUM_MACRO
#endif //__STDC__

#endif //MN_INSTRUM_H_
/* This line marks the end of the source */
