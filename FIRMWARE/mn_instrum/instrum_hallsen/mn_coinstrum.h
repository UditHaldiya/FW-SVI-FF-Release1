/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_coinstrum.h
    \brief MN code instrumentation. Critical section profilng version

    CPU: Any (no instrumentation)

    OWNER: AK

    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/mn_instrum/instrum_hallsen/mn_coinstrum.h $
    $Date: 4/08/11 4:04p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: mn_coinstrum.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/08/11    Time: 4:04p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/mn_instrum/instrum_hallsen
 * TFS:6036
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/01/09    Time: 11:29a
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/noinstrum
 * Default instrumentation (none)
*/
#ifndef MN_COINSTRUM_H_
#define MN_COINSTRUM_H_

#define MN_ENTER_CRITICAL_HOOK(psw) /*nothing*/
#define MN_EXIT_CRITICAL_HOOK(psw) /*nothing*/

#endif //MN_COINSTRUM_H_
/* This line marks the end of the source */
