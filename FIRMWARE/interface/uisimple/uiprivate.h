/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uiprivate.h
    \brief Implementation of MNCB LCD UI private definitions and headers.

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple/uiprivate.h $
    $Date: 1/05/12 3:00p $
    $Revision: 11 $
    $Author: Arkkhasin $
*/

/*
    $History: uiprivate.h $
 * 
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:00p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8577 - header refactoring
 *
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 1/14/11    Time: 3:08p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5407 - removed the unused field from ths structure. Shall use
 * #define fault code ID.
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 12/15/10   Time: 4:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 5059 - Added Current Button interface.
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 12/09/10   Time: 3:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4811 - corrected file header; removed unused states; added proto
 * for the Reset UI State Machine function.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 12/06/10   Time: 1:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4897 - corrected the code to separate the KBD diagnostic into a
 * separate data structure, eliminate mutliple decision making points,
 * steramline T/O check.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 11/12/10   Time: 2:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4652 - corrected bug and eliminated unused structure field.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 10/22/10   Time: 1:09p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Fixed header. Added protection wrapper.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 10/20/10   Time: 10:49a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4384 - Added protos - extracted jumper code from uimain.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/14/10   Time: 5:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4283 - fix for UI in Low Power mode.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 11:15a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Header file containing LUI private definitons.
 *
*/

#ifndef UIPRIVATE_H_
#define UIPRIVATE_H_

#include "uisimple.h"

//AK: moved from uipublic.h
/** REQ 31-002; REQ 31-003 */
// LOGical  Buttons
typedef enum
{
    BUTTON_LOG_STAR,
    BUTTON_LOG_PLUS,
    BUTTON_LOG_MINUS,
    BUTTON_LOG_SP,
    BUTTON_LOG_SM,
    BUTTON_LOG_PM,

    MAX_LOGICAL_BUTTONS,

    BUTTON_LOG_NONE,
    BUTTON_LOG_CANCEL,
    BUTTON_LOG_UNSUPPORTED

} LogicalButtons_t;

//-------------------------------------------------------------------
// Map function to specific button -- for app level use

#define UI_BUTTON_OPENSTOPADJUST    (BUTTON_LOG_MINUS)
#define UI_BUTTON_FINDSTOPS         (BUTTON_LOG_STAR)
#define UI_BUTTON_AUTOTUNE          (BUTTON_LOG_PLUS)

//--------------------------------------------------------
// LUI Exposed Interface

extern LogicalButtons_t     UI_Button(void);
extern void                 UI_EngageUI(void);
extern void                 UI_DisengageUI(void);
extern bool_t               UI_IsUIEngaged(void);

//--------------------------------------------------------
//AK: end move from uipublic.h

//------------------------------------------------------------------
// Simple UI Private Types and Definitions.

/** REQ 31-002 Button description */

typedef struct Simple_UI_Buttons_t
{
    uiinput_t           ButtonPattern;
    u8                  NumberOfButtons;
    bool_t              Supported;
} Simple_UI_Buttons_t;

// There are major paths through the state machine:
// None - Buttons     - Start     - Running - CancelStart - Cancel - None
// None - Buttons     - TestStart - Test    - None
// None - LockedStart - Locked    - None
//
// In each path could be shortcuts in the event of Cancellation - to CancelStart - Cancel - None
// In the Buttons and Start states could be a short-cut to None.

typedef enum
{
    UI_PROCESS_NONE,            // Functional path
    UI_PROCESS_BUTTONS,
    UI_PROCESS_START,
    UI_PROCESS_RUNNING,
    UI_PROCESS_CANCEL_START,
    UI_PROCESS_CANCEL,
    UI_PROCESS_ERROR,

    UI_PROCESS_TEST_START,      // Separate path - test
    UI_PROCESS_TEST

} Simple_UI_ProcessStates_t;

typedef enum
{
    UI_FUNCTION_FINDSTOPS,
    UI_FUNCTION_OPEN,
    UI_FUNCTION_AUTOTUNE,

    UI_FUNCTION_NONE,           // Void function (LAST ONE!)

    UI_MAX_ACTIVE_FUNCTIONS     // Max func #

} Simple_UI_Function_t;

/** \brief LCX UI variable - controls the UI state machine
*/
typedef struct Simple_UI_Control_t
{
    Simple_UI_ProcessStates_t   Process_State;
    Simple_UI_Function_t        UI_Function;
    LogicalButtons_t            PrevButton;
    LogicalButtons_t            CurrentButton;          // Current button - for the LUI Consumers.

    bool_t                      RequestToSetIdle;
    bool_t                      RequestToHoldIdle;

    bool_t                      UIEngaged;              // UI Engaged or not
    bool_t                      UIGlobalStatus;         // LUI Global status: enabled
    bool_t                      PBWithinWindow;         // Indicator that PB is down within time window

    tick_t                      TimeStart;              // T/O for supported PBs
    tick_t                      TimeAtLeast;            // MIN time
    tick_t                      TimeNoLonger;           // MAX time

    LED_DisplayState_t          LEDState;               // LUI LED state

    u16                         CheckWord;              // Checksum
} Simple_UI_Control_t;

/** \brief LCX UI variable - controls the UI KBD Faults state machine
*/
typedef struct Simple_UI_KBD_Diag_t
{
    bool_t              SomethingPressed;               // Something pressed flag
    tick_t              TimeStart;                      // Latched time
    tick_t              MaxTime;                        //
    u16                 CheckWord;                      // Checksum
} Simple_UI_KBD_Diag_t;

/** \brief LCX UI variable - controls the UI KBD Faults state machine
*/
typedef struct Simple_UI_Jumpers_t
{
    u8                      JumperSettings;
    u8                      NewJumperSettings;
    bool_t                  ChangeDetected;
    tick_t                  TimeStart;
    tick_t                  ElapsedTime;
    u16                     CheckWord;
} Simple_UI_Jumpers_t;

//------------------------------------------------------------------
// LUI Private functions.

ErrorCode_t LUI_SetNormalDeviceMode(void);
bool_t      Is_UI_StateMachineIdle(void);
void        Set_UI_StateMachineIdle(void);
void        Reset_UI_StateMachineIdle(void);
void        Let_UI_StateMachineGo(void);

// LED section
void        UI_SetPatternByIndex(LED_DisplayState_t LEDState);

// Jumpers section
void        Simple_UI_JumperHandler(void);
void        UI_TestMainDataStructure(void);
void        UI_Jumpers_Init(void);

#endif      // UIPRIVATE_H_ End

/* This line marks the end of the source */
//------------------------------------------------------------------
