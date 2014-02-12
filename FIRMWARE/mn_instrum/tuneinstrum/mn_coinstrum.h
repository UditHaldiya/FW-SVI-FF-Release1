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

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum/mn_coinstrum.h $
    $Date: 5/19/11 12:20p $
    $Revision: 6 $
    $Author: Justin Shriver $

    \ingroup OSWrap
*/
/* $History: mn_coinstrum.h $
 * 
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 5/19/11    Time: 12:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:6337
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 5/18/11    Time: 10:18a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:6337
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 5:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * TFS:5384 Instrument Autotune Temporarily
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/13/11    Time: 7:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/tuneinstrum
 * Stole the content of late_defines.h (TFS:5384)
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



#ifdef NDEBUG
#error("Autotune Instrumentation not allowed in release build")
#else
#define OPTIONAL_TUNE_DIAG_FLAG
#endif






#endif //MN_COINSTRUM_H_
/* This line marks the end of the source */
