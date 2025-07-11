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

    $Archive: /MNCB/Dev/FIRMWARE/mn_instrum/maestra/mn_coinstrum.h $
    $Date: 9/30/09 7:13p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: mn_coinstrum.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 9/30/09    Time: 7:13p
 * Updated in $/MNCB/Dev/FIRMWARE/mn_instrum/maestra
 * Most instrumentation is now off. They must be enabled in unit test make
 * driver.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/29/09    Time: 11:53p
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/maestra
 * First adaptation of MacroExpressions Maestra unit test environment to
 * MNCB codebase. Supports HTML output. Two libraries (plain text and HTML
 * output) are provided along with EWARM 5.30 projects to build them.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/01/09    Time: 11:29a
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/noinstrum
 * Default instrumentation (none)
*/
#ifndef MN_COINSTRUM_H_
#define MN_COINSTRUM_H_

#include "maestra.h"

#define MN_ENTER_CRITICAL_HOOK(psw) /*nothing*/
#define MN_EXIT_CRITICAL_HOOK(psw) /*nothing*/

#endif //MN_COINSTRUM_H_
/* This line marks the end of the source */
