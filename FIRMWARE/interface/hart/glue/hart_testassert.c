/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_testassert.c
    \brief HART interface for testing assert functionality

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_testassert.c $
    $Date: 2/17/11 3:02p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_testassert.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/17/11    Time: 3:02p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5657 - added assert testability on system level
*/
#include "mnwrap.h"
#include "mnassert.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"

#define ASSERT_TEST_MEANING_OF_LIFE 42

/** \brief passes through a trap or an assert, depending on the param in the
receive buffer (negative=trap, positive is an assert unless == 42.
*/
s8_least hartcmd_AssertEngineTest(const u8 *src, u8 *dst)
{
    const Req_AssertEngineTest_t *s = (const void *)src;
    UNUSED_OK(dst);
    s16 testval = util_GetS16(s->TestTrapID[0]);
    if(testval<0)
    {
        mn_assert(NULL, testval);
    }
    else
    {
        MN_RT_ASSERT(testval==ASSERT_TEST_MEANING_OF_LIFE);
    }

    return HART_NO_COMMAND_SPECIFIC_ERRORS; //response is copied automatically
}

/* This line marks the end of the source */
