/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uihw_test.h
    \brief

    CPU: Any (with standard C compiler)

    OWNER: AP
    \n RCS: $Id$
    Created: 9/27/04 Author:
*/

/*
    $History: uihw_test.h $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 2:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4946 - eliminated test_TestProcess_SHIM in favor of a correct
 * prototype of test_TestProcess
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 4:44p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * UI Hardware Test module - header file.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 2:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Bug 3278 - previous change was removed due to integration issues.
 * Reincorporated the change.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 5/24/10    Time: 6:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 5/24/10    Time: 6:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Bug 3226 - Toggle mode eliminated.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 5/19/10    Time: 5:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * COrrected the header.
*
*/


#ifndef UI_HW_TEST_H_
#define UI_HW_TEST_H_

#include "led.h" //for types
#include "process.h" //for types

/** \brief LCX Device Test mode type definition
*/

/** REQ 30-008 Test Modes IDs */
typedef enum
{
    TEST_MODE_START,
    TEST_MODE_LED_REMOTE,

    TEST_MODE_MAX

} test_TestMode_t;

procresult_t test_TestProcess(s16 *procdetails);

extern  ErrorCode_t     test_SetTestParameters(test_TestMode_t    TestMode,
                                               u16_least          TestTime,
                                               LEDNumber_t        TestLed);

#endif // UI_HW_TEST_H_

/* end of the source */
