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

    $Archive: /MNCB/Dev/FIRMWARE/includes/oswrap.h $
    $Date: 3/29/09 12:55a $
    $Revision: 42 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: oswrap.h $
*/
#ifndef MN_INSTRUM_H_
#define MN_INSTRUM_H_
enum taskid_t;
extern void profile_TimeStamp(enum taskid_t taskid);
extern void profile_MeasureTime(enum taskid_t taskid);
#define INSTRUM_AFTER_SEMAPHORE(taskid) profile_TimeStamp(taskid)
#define INSTRUM_BEFORE_SEMAPHORE(taskid) profile_MeasureTime(taskid)
extern void profile_ReadAndClear(u32 results[]);

#endif //MN_INSTRUM_H_
/* This line marks the end of the source */
