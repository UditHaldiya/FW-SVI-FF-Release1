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

    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/mn_instrum/instrum_hallsen/mn_instrum.h $
    $Date: 4/08/11 4:04p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: mn_instrum.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/08/11    Time: 4:04p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/mn_instrum/instrum_hallsen
 * TFS:6036
*/
#ifndef MN_INSTRUM_H_
#define MN_INSTRUM_H_

#define INSTRUM_AFTER_SEMAPHORE(taskid) /*nothing*/
#define INSTRUM_BEFORE_SEMAPHORE(taskid) /*nothing*/

extern void instrum_HallSensorLog(u8_least channel, u16 value);
#define INSTRUM_AD_SAMPLING(channel, sample) instrum_HallSensorLog(channel, sample)

#endif //MN_INSTRUM_H_
/* This line marks the end of the source */
