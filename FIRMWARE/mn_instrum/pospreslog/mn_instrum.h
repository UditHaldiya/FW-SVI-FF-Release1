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

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/pospreslog/mn_instrum.h $
    $Date: 4/26/11 11:59a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: mn_instrum.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/26/11    Time: 11:59a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/pospreslog
 * TFS:6154
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

extern void instrum_PosPresLog(void);
#define CTLTASK_END_HOOK() instrum_PosPresLog()

#endif //MN_INSTRUM_H_
/* This line marks the end of the source */
