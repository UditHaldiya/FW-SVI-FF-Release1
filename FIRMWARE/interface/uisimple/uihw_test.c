/**
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uihw_test.c
    \brief Implementation of MNCB LCX Manufacturing test.

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple/uihw_test.c $
    $Date: 1/05/12 1:40p $
    $Revision: 5 $
    $Author: Arkkhasin $
*/

/*
    $History: uihw_test.c $
 * 
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 1:40p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8577 headers refactoring
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 2:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/sysio/ledui
 * TFS:4946 - eliminated test_TestProcess_SHIM in favor of a correct
 * prototype of test_TestProcess
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 4:45p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * UI Hardware Test module - source file.
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 8/18/10    Time: 5:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3924 - replaced type ticks_t with tick_t.
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 11:13a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3627: Replaced FAILSAFE state with separate new DEBUG state for teh
 * RED LED.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 7/29/10    Time: 9:58a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3627: Updated LED patterns and control of the LED display. Updated
 * the test mode code to accomodate new mode of controlling of LED
 * display.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 7/19/10    Time: 11:45a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3550 - Added option to run forever (T/O value = 255).
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 2:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3278 - integrated the changes for the code review, removed unused
 * Test mode, corrected requirement numbers.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 5/19/10    Time: 5:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Added comments reflecting Requirements.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 5/14/10    Time: 2:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * EXcluded debugging code.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 5/14/10    Time: 2:11p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Bug 3156 - File with the test mode code added.

*
*/

#include "config.h"
#include "mnwrap.h" //for MN_NOINIT
#include "mnassert.h"
#include "oswrap.h"
#include "mncbtypes.h" //for button lock business
#include "uipublic.h"
#include "uiprivate.h"
#include "uiinput.h"
#include "errcodes.h"
#include "sysio.h"
#include "bitutils.h"
#include "bios.h"
#include "uihw_test.h"
#include "timebase.h"

//------------------------------------------------------------------
// T/O 0 - not allowed, 1..254 - T/O in seconds, 255 - forever

#define TEST_MODE_FOREVER_TIME          (255)

//------------------------------------------------------------------
// Structure defining the PB to LED mapping
typedef struct PButtonToLED_t
{
    uiinput_t           PB_BitMap;
    LED_DisplayState_t  LED_State;

} PButtonToLED_t;

static const PButtonToLED_t PButtonToLED[] =
{
    {BUTTON_STAR,           LED_DISPLAY_STATE_DEBUG_1},     // * -> GL1
    {BUTTON_MINUS,          LED_DISPLAY_STATE_DEBUG_2},     // - -> GL2
    {BUTTON_PLUS,           LED_DISPLAY_STATE_DEBUG_3},     // + -> GL3
    {BUTTON_CANCEL,         LED_DISPLAY_STATE_DEBUG_4},     // CANCEL -> RL
    {BUTTON_MASK,           LED_DISPLAY_STATE_DARK   }      // No such Button
};

//------------------------------------------------------------------
// Table to translate LED # from the HART command to LED display state
// The code performs the range checking during HART command validation

static const LED_DisplayState_t LED_To_State[NUMBER_OF_LEDS] =
{
    LED_DISPLAY_STATE_DEBUG_1,      // -> GL1
    LED_DISPLAY_STATE_DEBUG_2,      // -> GL2
    LED_DISPLAY_STATE_DEBUG_3,      // -> GL3
    LED_DISPLAY_STATE_DEBUG_4       // -> RL
};

/** \brief LCX Test Mode variable: Test Mode ID
    \brief LCX Test Mode variable: LED number (for the Remote HART LED test)
    \brief LCX Test Mode variable: Handling Time Out
*/

typedef struct TestModeParam_t
{
    test_TestMode_t     TestMode;
    LED_DisplayState_t  TestLedState;
    u16_least           TestTime;
    tick_t              Timeout;
    u16                 CheckWord;

} TestModeParam_t;

static  TestModeParam_t TestModeParam;

/** \brief LCX Test mode - local function
  Maps pushbutton to LED.
  REQ 30-009
  \parameter[in] - uiinput_t c - pushbuttons bit map
  \return LED_DisplayState_t - current LED State or NON_EXISTENT_HARDWARE_LED, if
                        there are no PBs pressed or invalid combination is pressed
*/
static LED_DisplayState_t   Get_LedNumberButton(uiinput_t c)
{
    bool_t      ButtonFound = false;
    u32         index = 0u;

    while ((PButtonToLED[index].LED_State != LED_DISPLAY_STATE_DARK) &&
           (!ButtonFound))
    {
        if (PButtonToLED[index].PB_BitMap == c)
        {
            ButtonFound = true;
        }
        else
        {
            index++;
        }
    }

    return PButtonToLED[index].LED_State;
}

/** \brief LCX Test mode - local function
    Operate LED - get Button Map, lit the selected LED
    The function implements requirement REQ 30-009
    \parameter[in] - uiinput_t c - pushbuttons
*/
static void Test_LedSwitch(uiinput_t c)
{
    LED_DisplayState_t LedState = Get_LedNumberButton(c);
    LED_SetState(LedState);
}

/** \brief LCX Test mode - External API fiuction
  API to HART command - Set specific Test Mode
  The function implements requirements REQ 30-003; REQ 30-004
  \parameter[in] - test_TestMode_t - The Test Mode to set
  \parameter[in] - u16_least       - Test Time Timeout
  \parameter[in] - LEDNumber_t     - LED number to test
  \return ErrorCode_t - Error
*/
ErrorCode_t     test_SetTestParameters(test_TestMode_t    TestMode,
                                       u16_least          TestTime,
                                       LEDNumber_t        TestLed)
{
    ErrorCode_t     retval;

    /** REQ 30-005; REQ 30-006; REQ 30-007 */
    // Validate Test Mode and Test Time and LED number
    if ((TestTime == 0u) ||
        (TestMode >= TEST_MODE_MAX) ||
        (TestLed  >= NUMBER_OF_LEDS))
    {   // Parameters are not OK, send an error
        retval = ERR_INVALID_PARAMETER;
    }
    else
    {   // Parameters are OK - set the up

        TestModeParam.TestMode     = TestMode;
        TestModeParam.TestTime     = TestTime;
        TestModeParam.TestLedState = LED_To_State[TestLed];
        TestModeParam.Timeout      = TIME_VALUE(TestTime * ONE_SECOND);
        STRUCT_CLOSE(TestModeParam_t, &TestModeParam);
        retval = ERR_OK;
    }
    return retval;
}

/** REQ 30-001 */

/** \brief LCX Test mode - External API fiuction
  API to PROCESS - The Device Test Process to run
*/
procresult_t test_TestProcess(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    uiinput_t       c;
    tick_t          StartTime;
    bool_t          ProcessDone = false;

    /** REQ 30-009; REQ 30-010; REQ 30-011 */
    UI_DisengageUI();           // Disconnect LUI
    quad_EnableInt();           // Enable RE

    /** REQ 30-009; REQ 30-010 */
    LED_SetState(LED_DISPLAY_STATE_DARK);

    StartTime = bios_GetTimer0Ticker();

    while (!ProcessDone)
    {   // Test Loop ...
        c  = sysio_GetPushButtons();
        c |= (u8)(c >> BUTTON_WIDTH);
        c &= BUTTON_MASK;

        Struct_Test(TestModeParam_t, &TestModeParam);

        /** REQ 30-004 */
        if (TestModeParam.TestTime != TEST_MODE_FOREVER_TIME)
        {   // If T/O value does not specify Run Forever - handle T/O
            if ((bios_GetTimer0Ticker() - StartTime) > TestModeParam.Timeout)
            {   // Timeout
                ProcessDone = true;
            }
        }
        else
        {   // Refresh the flag
            ProcessDone = false;
        }

        if (!ProcessDone)
        {
            switch (TestModeParam.TestMode)
            {
                /** REQ 30-009 */
                case TEST_MODE_START :
                    Test_LedSwitch(c);
                    break;

                /** REQ 30-008; REQ 30-010 */
                case TEST_MODE_LED_REMOTE :
                    // In this mode ignore the PB! Use test_TestLed
                    LED_SetState(TestModeParam.TestLedState);
                    break;

                case TEST_MODE_MAX :            // MAX value - the same as Unused
                default :
                    MN_DBG_ASSERT(TestModeParam.TestMode >= TEST_MODE_MAX);
                    break;
            }
        } // Proces not done

        /*
            Need to mopup, refresh WD, etc. so we must call process_WaitForTime()
            This may conflict with the above call to UI_Button().  This could be kind of sticky.
            Tha Cancel function should come back via the process_WaitForTime() return.
            This leaves a possible problem with setting the Stop Adjustment
        */
        if (process_WaitForTime(T0_050))
        {
            ProcessDone = true;
        }
    }

    /** REQ 30-012 DIsable QE and Engage LUI */
    quad_DisableInt();          // Disable RE
    UI_EngageUI();              // Connect LUI back
    return PROCRESULT_OK;
}

/* This line marks the end of the source */
//------------------------------------------------------------------
