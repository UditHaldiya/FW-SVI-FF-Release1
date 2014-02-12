/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uimain.c
    \brief Implementation of MNCB LCD UI.

    CPU: Any

    OWNER: AP
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/

/*
    $History: uimain.c $
 * 
 * *****************  Version 108  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 5:43p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * Restore LCX build FBO TFS:8782
 *
 * *****************  Version 107  *****************
 * User: Arkkhasin    Date: 1/10/12    Time: 12:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * (Partially) restored LCX build broken by work on AP build (TFS:8577).
 * No LCX code change except fault table where flash test timeout is still
 * inactive
 *
 * *****************  Version 106  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 1:15p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8577 - common WRITE_NUMBER stub (and specific #define's moved in)
 *
 * *****************  Version 104  *****************
 * User: Anatoly Podpaly Date: 2/15/11    Time: 2:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5622 -- corrected the comments and the code -- reset UI state
 * machine to normal.
 *
 * *****************  Version 103  *****************
 * User: Anatoly Podpaly Date: 2/03/11    Time: 5:31p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5511 -- added edges to PB state analysis.
 * TFS:5464 -- (partial) added check for prohibited mode(s) instead of LOW
 * POWER alone.
 *
 * *****************  Version 102  *****************
 * User: Anatoly Podpaly Date: 1/20/11    Time: 5:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5389 - corrected mode usage.
 *
 * *****************  Version 101  *****************
 * User: Anatoly Podpaly Date: 1/14/11    Time: 3:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5407 - removed the var, use #defined constant instead.
 * TFS:5393 - removed unneccessary critical section.
 * TFS:5391 - removed errouneuous check.
 *
 * *****************  Version 100  *****************
 * User: Anatoly Podpaly Date: 1/04/11    Time: 3:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5071, TFS:5022, TFS:4798, TFS:4803. The CANCEL button will result
 * in teh device trasfer to Normal mode without invoking Bumpless
 * transfer.
 *
 * *****************  Version 99  *****************
 * User: Anatoly Podpaly Date: 12/15/10   Time: 4:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 5059 - Added Current Button interface.
 *
 * *****************  Version 98  *****************
 * User: Anatoly Podpaly Date: 12/13/10   Time: 2:15p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 5021 - eliminated function UI_GetLaunchButton() - will be using
 * hard defines from uipublic.h; also replaced "flexible" button names in
 * functional descriptors with specific ones.
 *
 * *****************  Version 97  *****************
 * User: Anatoly Podpaly Date: 12/09/10   Time: 3:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4958 - removed public UI_IsOKtoRunAutoTune() function, now is used
 * as wrapper for selftune-defined function. Removed unused definition.
 *
 * Bug 4811 - moved UI Lock indication to global status instead of UI
 * state machine; modified the code to use Reset UI State Machine instead
 * of set and hold.
 *
 * *****************  Version 96  *****************
 * User: Anatoly Podpaly Date: 12/06/10   Time: 4:53p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4956 - replaced UI function with new PID slot ID function.
 *
 * *****************  Version 95  *****************
 * User: Anatoly Podpaly Date: 12/06/10   Time: 2:00p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Changed teh default Init value.
 *
 * *****************  Version 94  *****************
 * User: Anatoly Podpaly Date: 12/06/10   Time: 1:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4897
 * - corrected the issue by eliminating multiple points of "decision
 * making";
 * - corrected KBD diagnostics;
 * - streamlined time calculations;
 * - utiluized standard practice of having init structure via const
 * default data;
 *
 * *****************  Version 93  *****************
 * User: Anatoly Podpaly Date: 12/03/10   Time: 2:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4951 - removed obsoleted function void ui_NotifyNonUIProc(u8
 * procId).
 *
 * *****************  Version 92  *****************
 * User: Anatoly Podpaly Date: 11/12/10   Time: 2:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4652 - fixed jumper code, removed unused init function.
 *
 * Bug 4432 - Removed single mode "assembly" point.
 *
 * *****************  Version 91  *****************
 * User: Anatoly Podpaly Date: 11/01/10   Time: 5:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4493 - unused header dimensions.h removed.
 *
 * *****************  Version 90  *****************
 * User: Anatoly Podpaly Date: 10/28/10   Time: 3:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Incicentally overwritten previous chamge - OSA proc ID. Restored the
 * ID.
 *
 * *****************  Version 89  *****************
 * User: Anatoly Podpaly Date: 10/28/10   Time: 3:30p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4432 - fixed the If statement to eliminate the LINT complain about
 * potential side effects, corrected the if statement and "atomized" the
 * Mode info collection.
 *
 * *****************  Version 88  *****************
 * User: Sergey Kruss Date: 10/26/10   Time: 2:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:4288 - OS Adj is called as a "normal" process via proc table.
 *
 * *****************  Version 87  *****************
 * User: Anatoly Podpaly Date: 10/20/10   Time: 10:52a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4384 - extracted jumper code into the separate file.
 * Bug 4253 - Fixed a bug in "Hold Idle" request, fixed cancellation of
 * the Test mode.
 *
 * *****************  Version 86  *****************
 * User: Anatoly Podpaly Date: 10/19/10   Time: 2:38p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4359 - Added support for HART commands:
 * 170,218 - read immediate PID selector
 * 170,219 - read immediate ATO selector
 *
 * *****************  Version 85  *****************
 * User: Anatoly Podpaly Date: 10/15/10   Time: 5:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4075 - updated code to resolve the following requirements:
 * 1. Bumpless transfer shall not be honored when a UI process completes
 * or cancels or fails and returns to normal mode
 * 2. Bumpless transfer shall be honored when UI starts transition to
 * normal mode with no UI process running and with “non-normal”, or
 * “override,” mode indication.
 * 3. Bumpless transfer shall be honored when normal mode is requested by
 * HART.
 * 4. Bumpless transfer shall not be honored when normal mode is restored
 * on power-up or on exit from low power mode (=AP behavior).
 * 5. Low power UI indication shall be “all lights off” and shall override
 * any other competing LED pattern
 * 6. Low power UI indication of Bumpless transfer shall be the same as
 * indication of “non-normal”, or “override,” mode
 *
 * *****************  Version 84  *****************
 * User: Anatoly Podpaly Date: 10/14/10   Time: 5:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4283: fix for UI in Low Power mode.
 *
 * *****************  Version 83  *****************
 * User: Anatoly Podpaly Date: 10/13/10   Time: 11:12a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Created SetOperateMode wrapper: rerlated to the bug #4075.
 *
 * *****************  Version 82  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 1:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4184 - extracted LED blinking patterns into separate file.ALso
 * extracted UI private definitions into a separate .h file.
 * Bug 4188 - implemented UI Lock.
 * Bug 4220 - corrected UI behavior - CANCLE button gets LUI out of Setup
 * mode.
 *
 * *****************  Version 81  *****************
 * User: Sergey Kruss Date: 10/01/10   Time: 10:02a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * <TFS:4224>
 * Reset OS Adj to 100% when air action changes.
 *
 * *****************  Version 80  *****************
 * User: Justin Shriver Date: 9/16/10    Time: 11:44a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:4123 mapping of LEDs
 *
 * *****************  Version 79  *****************
 * User: Anatoly Podpaly Date: 9/09/10    Time: 10:39a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4093: based on reviewer comments: removed unneded casts.
 *
 * *****************  Version 78  *****************
 * User: Anatoly Podpaly Date: 9/08/10    Time: 7:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4093 - Remap Phys PID SLot ID to Logical and clamp it not to exceed
 * teh PID Parameter Array dimensions.
 *
 * *****************  Version 77  *****************
 * User: Anatoly Podpaly Date: 9/03/10    Time: 10:07a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4064: Implemented requirements re: LOW POWER mode.
 * Bug 4006: Addressed as the behavior of the LOW POWER mode is corrected;
 * corrected Test mode behavior in ths scope of the LOW POWER mode.
 *
 * *****************  Version 76  *****************
 * User: Anatoly Podpaly Date: 8/30/10    Time: 6:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3967 - corrected code to eliminate LINT warning.
 *
 * *****************  Version 75  *****************
 * User: Anatoly Podpaly Date: 8/18/10    Time: 5:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3923 - incorporate results of code review:  Process ID should be of
 * type ProcID_t, use PROC_NONE iinstead of 0; Add ALL _OFF LED name
 * (Placeholder; moved definitions of Function - specific LEDs here.
 *
 * *****************  Version 74  *****************
 * User: Anatoly Podpaly Date: 8/16/10    Time: 8:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3733 - corrections for the mode check.
 *
 * *****************  Version 73  *****************
 * User: Anatoly Podpaly Date: 8/16/10    Time: 6:51p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3871 - moved blinking patterns from driver level to ui level.
 * Bug 3733 - shall not switch to Normal if already in OOS.
 *
 * *****************  Version 72  *****************
 * User: Anatoly Podpaly Date: 8/13/10    Time: 11:00a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Correction for the bug 3682: updated the Slot Numbers for ATO and ATC.
 *
 * *****************  Version 71  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 6:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Added req references
 *
 * *****************  Version 70  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 4:43p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3682 - AutoTune shall not run if the incorrect PID slots are
 * selected.
 *
 * *****************  Version 69  *****************
 * User: Anatoly Podpaly Date: 7/29/10    Time: 9:59a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3627: Updated LED patterns and control of the LED display. Updated
 * theLUI code to accomodate new mode of controlling of LED display.
 *
 * Removed the test mode, instead interoduced a new and specific LED
 * state. Removed Beacon LED controls, as the control is done via LED
 * driver.
 *
 * *****************  Version 68  *****************
 * User: Anatoly Podpaly Date: 7/16/10    Time: 6:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3534 - Obsoleted state PRERUN- removed.
 * Bug 3536 - Added FAULT indication on the Red Status LED.
 *
 * *****************  Version 67  *****************
 * User: Anatoly Podpaly Date: 5/28/10    Time: 3:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * COrrected and added Req numbers.
 *
 * *****************  Version 66  *****************
 * User: Anatoly Podpaly Date: 5/27/10    Time: 6:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Corrected processing of multiple button events.
 * CHanged MACROs for more readibility
 *
 * *****************  Version 65  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 5:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * ADded requirements ##
 *
 * *****************  Version 64  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 3:31p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 3279, 3278 - fixed multiple buttons handling, addeed warning for
 * OpenStopAdj.
 *
 * *****************  Version 63  *****************
 * User: Anatoly Podpaly Date: 5/19/10    Time: 3:39p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * 3236: removed redundant request to cancel a process.
 *
 * *****************  Version 62  *****************
 * User: Arkkhasin    Date: 5/14/10    Time: 6:53p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Update for new process API
 *
 * *****************  Version 60  *****************
 * User: Anatoly Podpaly Date: 5/14/10    Time: 4:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Moved LED_Periodicstructure test here from process.c
 *
 * *****************  Version 59  *****************
 * User: Anatoly Podpaly Date: 5/14/10    Time: 2:09p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Bug 3156: Added Test ode - API for the test mode to Engage, Disengage
 * LUI and in inquire the status of engagement.
 *
 * *****************  Version 58  *****************
 * User: Anatoly Podpaly Date: 3/30/10    Time: 3:04p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Update: UI baheviour change: If the device is not in Normal mode, then
 * the CANCEL button resets it to Normal mode.
 * Also : introduced a variable to handle future UI Engage / Disengage
 * modes (for manufacturing tests).
 *
 * *****************  Version 57  *****************
 * User: Anatoly Podpaly Date: 3/26/10    Time: 6:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Added NVRAM storage for the Jumper settings.
 *
 * *****************  Version 56  *****************
 * User: Anatoly Podpaly Date: 3/26/10    Time: 4:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Removed function ui_FollowLock.
 *
 * *****************  Version 55  *****************
 * User: Anatoly Podpaly Date: 3/19/10    Time: 5:53p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Code review results: typo fixed.
 *
 * *****************  Version 54  *****************
 * User: Anatoly Podpaly Date: 3/11/10    Time: 11:09a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * - Corrected FAILSAFE mode detection and processing;
 * - Added code to block LUI in FAILSAFE mode - reruirement;
 * - Added code to correct UI behavior in Setup mode - requirement.
 *
 * *****************  Version 53  *****************
 * User: Anatoly Podpaly Date: 3/05/10    Time: 11:57a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * LINT + added check for the validity of the UI function.
 *
 * *****************  Version 52  *****************
 * User: Anatoly Podpaly Date: 3/05/10    Time: 10:05a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Fixed problem cancellation of the process was indicated as an error,
 * i.e. the process failure. Cancellation by CANCEL button should not be a
 * failure.
 *
 * *****************  Version 51  *****************
 * User: Anatoly Podpaly Date: 3/04/10    Time: 5:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * - Added correct Jumper processing with UI state machine idling.
 * - Added code for error processing : process call backs.
 *
 * *****************  Version 50  *****************
 * User: Anatoly Podpaly Date: 3/01/10    Time: 2:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Removed unused parts of UI
 * Added that detects the change in Jumopper settings and causes the state
 * machine to get idle and hold the idle while the jumper change takes
 * effect.
 *
 * *****************  Version 49  *****************
 * User: Anatoly Podpaly Date: 2/25/10    Time: 5:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Added mod to catch the edges - short button presses.
 *
 * *****************  Version 48  *****************
 * User: Anatoly Podpaly Date: 2/24/10    Time: 3:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * LINT correction.
 *
 * *****************  Version 47  *****************
 * User: Anatoly Podpaly Date: 2/24/10    Time: 3:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Added new feature: button, when pressed for at least ACTIVITY TIME will
 * start blinking as if the process hes been started, if pressed longer
 * than NO LONGER time - it will cancel and go to "beaconong" mode. The
 * actual process will start only on button release.
 *
 * *****************  Version 46  *****************
 * User: Anatoly Podpaly Date: 2/23/10    Time: 3:43p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Increased T/O for "too long" from 4s to 7s. Added calls to clear keypad
 * faults.
 *
 * *****************  Version 45  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:57a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * LCX development - generic PB handler, corrected the CANCEL issue with
 * Beacon. Added few support functions to be used with Jumper handler.
 *
 * *****************  Version 44  *****************
 * User: Anatoly Podpaly Date: 2/05/10    Time: 4:25p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Mods:
 * -- Include blinking mode in the data structure;
 * -- Renamed Manual as interactive;
 * -- Struct_close instead of struct_copy
 * -- naming changes for clarity;
 * -- Generic Button processor added;
 * -- Removed unused code segments;
 * -- Unsupported buttons added.
 *
 * *****************  Version 43  *****************
 * User: Anatoly Podpaly Date: 2/02/10    Time: 5:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Modified to use new process control method.
 *
 * *****************  Version 42  *****************
 * User: Anatoly Podpaly Date: 2/02/10    Time: 3:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * Move to new process control scheme.
 *
 * *****************  Version 41  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 5:08p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/ui
 * LCX implementation: UI state machine.
*/

//--------------------------------------------------------------------
//
// The code in this file contains the implementation of REQ 31-001
//
// The following LUI requirements are implemented in other modules and
// are not referenced in this file
//
// REQ 31-004 - Quadrature Encoder
// REQ 31-011 - LED blinking patterns
// REQ 31-030 - Quadrature Encoder
// REQ 31-031 - Quadrature Encoder
// REQ 31-032 - H/W Drivers
// REQ 31-033 - SSEL Pin (GPIO)
//
// Several requirements are referenced in this file and also in other files.
//--------------------------------------------------------------------

#include "config.h"
#include "mnwrap.h" //for MN_NOINIT
#include "oswrap.h"
#include "mnassert.h"
#include "mncbtypes.h" //for button lock business
#include "uipublic.h"
#include "uiinput.h"
#include "errcodes.h"
#include "configure.h"
#include "uistartnodes.h"
#include "devicemode.h"
#include "sysio.h"
#include "process.h"
#include "calib.h"
#include "control.h"
//#include "bitutils.h"
#include "bios.h"
#include "nvram.h"
#include "uiprivate.h"
#include "uilock.h"

// For Autotune slots
#include "selftune.h"

#include "faultpublic.h"

//-------------------------------------------------------------------
// LCX - specific variables
//Raw input definitions
#define UIINPUT_RAW_NONE ((uiinput_t)0)

#define BUTTON_ACTIVE_MASK  ((uiinput_t)(BUTTON_STAR | BUTTON_PLUS | BUTTON_MINUS))
#define BUTTON_NO_BUTTON    UIINPUT_RAW_NONE

// Pairs...
#define BUTTON_2_SP         ((uiinput_t)(BUTTON_STAR | BUTTON_PLUS))
#define BUTTON_2_SM         ((uiinput_t)(BUTTON_STAR | BUTTON_MINUS))
#define BUTTON_2_PM         ((uiinput_t)(BUTTON_PLUS | BUTTON_MINUS))




/** REQ 31-012; REQ 31-013; REQ 31-014; REQ 31-015; REQ 31-017 */
/** REQ 31-022 */
// Button should be pressed for more than ...
#define TIME_BUTTONPRESSED_ACTIVITY         (T2_000)

/** REQ 31-023 */
// Button should be pressed but less than ...
// Original 4 sec -> 5 sec -> 7 sec
#define TIME_BUTTONPRESSED_NOLONGER         ((T5_000) + (T2_000))

//-------------------------------------------------------------------
/** \brief LCX UI This variable is to support the LUI Jumper settings stored in NVRAM
*/

//-------------------------------------------------------------------
/** \brief LCX UI Button combinations for the button decoder
*/

/** REQ 31-002; REQ 31-035; REQ 31-029 */
// Array of Patterns and corresponding LOG buttons
static const Simple_UI_Buttons_t Logical_UI_Buttons[MAX_LOGICAL_BUTTONS] =
{
    [BUTTON_LOG_STAR]  = {BUTTON_STAR,    1,  true},
    [BUTTON_LOG_PLUS]  = {BUTTON_PLUS,    1,  true},
    [BUTTON_LOG_MINUS] = {BUTTON_MINUS,   1,  true},
    [BUTTON_LOG_SP]    = {BUTTON_2_SP,    2,  false},
    [BUTTON_LOG_SM]    = {BUTTON_2_SM,    2,  false},
    [BUTTON_LOG_PM]    = {BUTTON_2_PM,    2,  false},
};

static const Simple_UI_Control_t   Simple_UI_Control_Default =
{
    .Process_State          = UI_PROCESS_NONE,
    .UI_Function            = UI_FUNCTION_NONE,
    .PrevButton             = BUTTON_LOG_NONE,
    .CurrentButton          = BUTTON_LOG_NONE,

    .RequestToSetIdle       = false,
    .RequestToHoldIdle      = false,
    .UIEngaged              = true,
    .UIGlobalStatus         = true,
    .PBWithinWindow         = false,

    .TimeStart              = 0,
    .TimeAtLeast            = TIME_BUTTONPRESSED_ACTIVITY,
    .TimeNoLonger           = TIME_BUTTONPRESSED_NOLONGER,

    .LEDState               = LED_DISPLAY_STATE_DARK,

    .CheckWord              = 0
};

static Simple_UI_Control_t   Simple_UI_Control;

//-------------------------------------------------------------------
/** \brief LCX UI variable - controls the UI KBD Faults state machine
*/

static const Simple_UI_KBD_Diag_t Simple_UI_KBD_Diag_Default =
{
    .SomethingPressed       = false,                        // !< Something pressed flag
    .TimeStart              = 0,                            // !< Latched time
    .MaxTime                = TIME_BUTTONPRESSED_NOLONGER,  // !< Max time
                                                            //
    .CheckWord              = 0                             // !< Checksum
};

static Simple_UI_KBD_Diag_t Simple_UI_KBD_Diag;

//-------------------------------------------------------------------
/** \brief LCX UI Current Time - this variable updated every LUI cycle
  and is not protected by the checksum
*/

static tick_t               m_LUI_Global_CurrentTime;

//-------------------------------------------------------------------
// Define Processes and results

// Meanings of repsonses. The only meaningful is FAILED,
// the rest of them is considered INGNORE (for now)
typedef enum
{
    CALLBACK_INFO,              // !< This ID means: nothing
    CALLBACK_FAILED,            // !< This ID means: the process failed
    CALLBACK_OK,                // !< This ID means: the process succeeded
    // CALLBACK_INIT,              // !< This is the INIT value Means nothing.
    CALLBACK_WARNING,           // !< This callback means the process issuing a WARNING
    CALLBACK_END                // !< Used for parsing only - indicates the end of list
} CallBackMeaning_t;

typedef struct CallBackResults_t
{
    s16_least               CallBackID;         // !< Node_id: the callback ID
    CallBackMeaning_t       CallBackMeaning;    // !< What it means for UI...
} CallBackResults_t;

typedef struct CallBackValue_t
{
    s16_least               CallBackID;
} CallBackValue_t;

/** REQ 31-034 */
// Redefine for local purposes
#define UI_UNDEFINED        (UINODEID_INITNODE)

/** REQ 31-034 */
// Results for Find Stops
static const CallBackResults_t FindStops_Results[] =
{
    {UINODEID_CALSTOPS,                 CALLBACK_OK},
    {UINODEID_STOPS_RUNNING,            CALLBACK_INFO},
    {UINODEID_STOPS_RUNNING_LOW,        CALLBACK_INFO},
    {UINODEID_STOPS_RUNNING_HIGH,       CALLBACK_INFO},
    {UINODEID_STOPS_RUNNING_WARNING,    CALLBACK_WARNING},
    {UINODEID_RUNSTOPS_FAILED,          CALLBACK_FAILED},
    {UI_UNDEFINED,                      CALLBACK_END}
};

/** REQ 31-034 */
// Results for AutoTune
static const CallBackResults_t AutoTune_Results[] =
{
    {UINODEID_TUNE1,                CALLBACK_INFO},
    {UINODEID_TUNE2,                CALLBACK_INFO},
    {UINODEID_TUNE3,                CALLBACK_INFO},
    {UINODEID_TUNE_OK,              CALLBACK_OK},
    {UINODEID_TUNE_FAILED,          CALLBACK_FAILED},
    {UINODEID_TUNE_PROTECTED,       CALLBACK_FAILED},
    {UINODEID_TUNE_WARNING,         CALLBACK_WARNING},
    {UI_UNDEFINED,                  CALLBACK_END}
};

/** REQ 31-034 */
// Results for Open Stop Adjustment
static const CallBackResults_t StopAdjusts_Results[] =
{
    {UINODEID_OPENSTOPADJ_OK,       CALLBACK_OK},
    {UINODEID_OPENSTOPADJ_FAILED,   CALLBACK_FAILED},
    {UINODEID_OPENSTOPADJ_WARNING,  CALLBACK_WARNING},
    {UINODEID_OPENSTOPADJ_NORMAL,   CALLBACK_INFO},
    {UI_UNDEFINED,                  CALLBACK_END}
};

/** REQ 31-002; REQ 31-006; REQ 31-007; REQ 31-008; REQ 31-009 */
/** REQ 31-019; REQ 31-020 */
/** REQ 31-040 */
// Actual Variables

static bool_t   AlwaysOKToRunProcess(void);             // Forward declaration

/** \brief LCX UI Functional state descriptors
 This is the UI Function descriptor, that combines togerther the function (process ID) and the initiator (button)
 the display (OK< Error and Warning) pre-run checks.
*/
/** REQ 31-040 - the structure contains ptr to function to pre-check the conditions BEFORE running the process */
typedef struct Simple_UI_ActivityDesc_t
{
    LED_DisplayState_t          DisplayFunctionOK;      // !< Display function associated with the UI function
    LED_DisplayState_t          DisplayFunctionWarning; // !< Display function WARNING
    LED_DisplayState_t          DisplayFunctionError;   // !< Display function ERROR
    LogicalButtons_t            LogButton;              // !< Logical Button that launches the function
    bool_t                      (* const ptrOK_To_Run)(void);  // !< OK To launch the process
    s16_least                   PrechekFailedID;        // !< Call Back ID when pre-chek failed
    const CallBackResults_t    *pCallBackResults;       // !< Pointer to the Result Array for Parsing
    ProcId_t                    ProcessID;              // !< Process ID that is associated with the function
} Simple_UI_ActivityDesc_t;

static const Simple_UI_ActivityDesc_t Simple_UI_ActivityDesc[UI_MAX_ACTIVE_FUNCTIONS] =
{
    [UI_FUNCTION_NONE] =        // Empty Function!
    {
        .DisplayFunctionOK      = LED_DISPLAY_STATE_NORMAL,
        .DisplayFunctionWarning = LED_DISPLAY_STATE_NORMAL,
        .DisplayFunctionError   = LED_DISPLAY_STATE_NORMAL,
        .LogButton              = BUTTON_LOG_NONE,
        .ptrOK_To_Run           = AlwaysOKToRunProcess,
        .PrechekFailedID        = UI_UNDEFINED,
        .pCallBackResults       = NULL,
        .ProcessID              = PROC_NONE
    },

    [UI_FUNCTION_FINDSTOPS] =
    {
        .DisplayFunctionOK      = LED_DISPLAY_STATE_SELECTED_1,
        .DisplayFunctionWarning = LED_DISPLAY_STATE_WARNING_1,
        .DisplayFunctionError   = LED_DISPLAY_STATE_ERROR_1,
        .LogButton              = UI_BUTTON_FINDSTOPS,
        .ptrOK_To_Run           = AlwaysOKToRunProcess,
        .PrechekFailedID        = UINODEID_RUNSTOPS_FAILED,
        .pCallBackResults       = FindStops_Results,
        .ProcessID              = PROC_FINDSTOPS_NORMAL
    },

    [UI_FUNCTION_OPEN] =
    {
        .DisplayFunctionOK      = LED_DISPLAY_STATE_SELECTED_2,
        .DisplayFunctionWarning = LED_DISPLAY_STATE_WARNING_2,
        .DisplayFunctionError   = LED_DISPLAY_STATE_ERROR_2,
        .LogButton              = UI_BUTTON_OPENSTOPADJUST,
        .ptrOK_To_Run           = AlwaysOKToRunProcess,
        .PrechekFailedID        = UINODEID_OPENSTOPADJ_FAILED,
        .pCallBackResults       = StopAdjusts_Results,
        .ProcessID              = PROC_OPENSTOPADJ_NORMAL   //TFS:4288
    },

    [UI_FUNCTION_AUTOTUNE] =
    {
        .DisplayFunctionOK      = LED_DISPLAY_STATE_SELECTED_3,
        .DisplayFunctionWarning = LED_DISPLAY_STATE_WARNING_3,
        .DisplayFunctionError   = LED_DISPLAY_STATE_ERROR_3,
        .LogButton              = UI_BUTTON_AUTOTUNE,
        .ptrOK_To_Run           = posctl_IsTunableCtlset,
        .PrechekFailedID        = UINODEID_TUNE_FAILED,
        .pCallBackResults       = AutoTune_Results,
        .ProcessID              = PROC_AUTOTUNE_NORMAL
    }
};

//-------------------------------------------------------------------
// Variables and types for Callback

static CallBackValue_t  CallBackValue =
{
    .CallBackID             = UI_UNDEFINED,
};

//-------------------------------------------------------------------
// LCX UI Supporting code

//-------------------------------------------------------------------
// Few MACROs for readability...

#define     ALL_BUTTONS_RELEASED(ButState)              (ButState == BUTTON_LOG_NONE)
#define     ANY_ACTIVE_BUTTON_PRESSED(ButState)         ((ButState != BUTTON_LOG_NONE) && (ButState != BUTTON_LOG_CANCEL))
#define     ANY_BUTTON_PRESSED(ButState)                (ButState != BUTTON_LOG_NONE)
#define     BUTTON_CANCEL_PRESSED(ButState)             (ButState == BUTTON_LOG_CANCEL)
#define     SUPPORTED_BUTTON_PRESSED(ButState)          (ButState != BUTTON_LOG_UNSUPPORTED)

//-------------------------------------------------------------------
// LCX UI State Machine handling functions

/** \brief LED_GetFunctionState
 *  \return LED_DisplayFunctionState_t - the current LED Display Function State. Used by HART emulation only.
*/
LED_DisplayState_t   LED_GetDisplayState(void)
{
    return Simple_UI_Control.LEDState;
}

/** \brief LED_SetState - set the mode (bliinking off, on etc.) for the LED display
  \param[in] LedSTate - LED to State
*/
void LED_SetState(LED_DisplayState_t LEDState)
{
    MN_DBG_ASSERT(LEDState < LED_DISPLAY_STATE_MAX);

    MN_ENTER_CRITICAL();
        if (Simple_UI_Control.LEDState != LEDState)
        {   // Only if we are changing something
            storeMemberInt(&Simple_UI_Control, LEDState, (u8)LEDState);
            UI_SetPatternByIndex(LEDState);
        }
    MN_EXIT_CRITICAL();
}

/** \brief Display_Status - set the mode (bliinking off, on etc.) for teh specific LED
  \param[in] LedSTate - LED to State
*/
static void Display_Status(LED_DisplayState_t LEDState)
{
    LED_SetState(LEDState);
}

/** \brief LUI_SetNormalDeviceMode - This function is a wrapper:
    to set the Normal mode without BUMPLESS transfer
    \returns    ErrorCode_t - result
*/
/** REQ 31-007; 31-010; 31-048; 31-049; 31-050; 31-051; 31-052 */
ErrorCode_t  LUI_SetNormalDeviceMode(void)
{
    return mode_SetOperateMode();
}

static void SetCallBackID(s16_least node_id);           // Forward declaration

/** REQ 31-002; REQ 31-035; REQ 31-029 */
/** \brief LCX UI Pushbutton parser
    This function also resolves multi-button presses
  \param[in] uiinput PButton    - Pushbutton bitmap
  \return LogicalButtons_t - Parsed logical pushbutton state
*/
static LogicalButtons_t   ParsePB(uiinput_t PButton)
{
    LogicalButtons_t    Index;
    LogicalButtons_t    retval;
    bool_t              Found;
    uiinput_t           Pattern;

    if ((PButton & (uiinput_t)BUTTON_CANCEL) == (uiinput_t)BUTTON_CANCEL)
    {   // If CANCEL - the rest is not important
        retval = BUTTON_LOG_CANCEL;
    }
    else if ((PButton & BUTTON_ACTIVE_MASK) == BUTTON_NO_BUTTON)
    {   // No button pushed
        retval = BUTTON_LOG_NONE;
    }
    else
    {   // Not a CANCEL button, but something
        Index  = BUTTON_LOG_STAR;           // First logical button
        Found  = false;
        retval = BUTTON_LOG_UNSUPPORTED;    // Non-supported buttons/combinations

        while ((Index < MAX_LOGICAL_BUTTONS) && (!Found))
        {
            Pattern = Logical_UI_Buttons[Index].ButtonPattern;
            if (((PButton & Pattern)  == Pattern) &&
                ((PButton & ~Pattern) == BUTTON_NO_BUTTON))
            {   // Got the pattern, check if it is supported
                if (Logical_UI_Buttons[Index].Supported)
                {   // Supported!
                    retval = Index;
                    Found  = true;
                }
            }
            Index++;
        }
    }
    return retval;
}

/** \brief LCX UI Prepare for Process Response
*/
static  void    ClearProcessResponse(void)
{
    CallBackValue.CallBackID = UI_UNDEFINED;
}

/** \brief LCX UI Check if device is in Prohibited mode (Failsafe, Low Power)
    \return bool_t  true - if in Prohibited mode
*/
static bool_t   IsProhibitedMode(void)
{
    const ModeData_t *md = mode_GetMode();              // Get device mode
    bool_t            retval = false;

    if (md->submode != SUBMODE_OOS_NORMAL)
    {   // Prohibited mode: FAILSAFE. LowPower
        retval = true;
    }
    return retval;
}

/** \brief LCX UI Check if device is in FAILSAFE mode
    \return bool_t  true - if in FAILSAFE mode.
*/
static bool_t   IsFAILSAFEMode(void)
{
    const ModeData_t *md = mode_GetMode();              // Get device mod
    bool_t            retval = false;

    if ((md->submode & SUBMODE_OOS_FAILSAFE) != 0)
    {   // FailSafe !
        retval = true;
    }
    return retval;
}

/** \brief LCX UI Check if device is in LOWPOWER mode
    \return bool_t  true - if in LOW POWER mode.
*/
/** REQ 31-043 */
static bool_t   IsLOWPOWERMode(void)
{
    const ModeData_t *md = mode_GetMode();              // Get device mod
    bool_t            retval = false;

    if ((md->submode & SUBMODE_OOS_LOWPOWER) != 0)
    {   // LowPower !
        retval = true;
    }
    return retval;
}

/** \brief LCX UI Check if device is in NORMAL mode
    \return bool_t  true - if in NORMAL mode.
*/
static bool_t   IsNORMALMode(void)
{
    const ModeData_t *md = mode_GetMode();              // Get device mod
    bool_t            retval = false;

    if (md->mode == MODE_OPERATE)
    {   // Normal mode
        retval = true;
    }
    return retval;
}

/** \brief LUI Function - check if it is OK to run process
    Always OK
  \return bool_t - true! ALways OK to run
*/
static bool_t AlwaysOKToRunProcess(void)
{
    return true;
}

/** \brief External UI inteface to get the button state
  \return LogicalButtons_t - Parsed logical pushbutton state
*/
LogicalButtons_t   UI_Button(void)
{
    return Simple_UI_Control.CurrentButton;
}

/** \brief LCX UI Check for TimeOut
  \param[in] TimeStart      Variable to check for timeout
  \param[in] TimeOutValue   Timeout value to compare against
  \return bool_t - true if T/O exceeds the passed value, otherwise - false.
*/
static bool_t   CheckTimeout(tick_t     TimeStart,
                             tick_t     TimeOutValue)
{
    bool_t      retval;
    tick_t      DeltaTicks;

    DeltaTicks = m_LUI_Global_CurrentTime - TimeStart;
    retval = (DeltaTicks > TimeOutValue) ? true : false;
    return retval;
}

/** \brief LCX UI Reset timeout
*/
static void Reset_UI_Timeout(void)
{
    Simple_UI_Control.TimeStart = m_LUI_Global_CurrentTime;
}

/** \brief LCX UI State Machine handling
    Checks the state machine state - if it is idle.
  \return bool_t - true - if the state machine in idle state, otherwise false
*/
bool_t Is_UI_StateMachineIdle(void)
{
    bool_t  retval;

    retval = (Simple_UI_Control.Process_State == UI_PROCESS_NONE) ? true : false;
    return retval;
}

/** \brief LCX UI checks if the device is indicating the DEVICE status
  \return bool_t - true - if the display is set to DEVICE status
*/
static bool_t Is_UI_GlobalStatus(void)
{
    return Simple_UI_Control.UIGlobalStatus;
}

/** \brief LCX UI will start displaying the DEVICE status
*/
static void Enable_GlobalStatus(void)
{
    if (!Is_UI_GlobalStatus())
    {   // Do this only if we are not in DEVICE Status mode
        // Turn OFF the lights and pick up from there
        Display_Status(LED_DISPLAY_STATE_DARK);
        Simple_UI_Control.UIGlobalStatus = true;
    }
}

/** \brief LCX UI will stop displaying the DEVICE status
*/
static void Disable_GlobalStatus(void)
{
    Simple_UI_Control.UIGlobalStatus = false;
}

/** \brief LCX UI Set State Machine IDLE
    Set request to set and hold idle the UI state machine
*/
/** REQ 31-027 */
void Set_UI_StateMachineIdle(void)
{
    MN_ENTER_CRITICAL();
        storeMemberInt(&Simple_UI_Control, RequestToSetIdle, true);
        storeMemberInt(&Simple_UI_Control, RequestToHoldIdle, true);
    MN_EXIT_CRITICAL();
}

/** \brief LCX UI Reset State Machine to IDLE state
    Set request to set UI State Machine Idle, do not set the hold flag
*/
/** REQ 31-027 */
void Reset_UI_StateMachineIdle(void)
{
    MN_ENTER_CRITICAL();
        storeMemberInt(&Simple_UI_Control, RequestToSetIdle, true);
    MN_EXIT_CRITICAL();
}

/** \brief LCX UI Let State Machine GO
    Let the UI state machine go
*/
/** REQ 31-027 */
void Let_UI_StateMachineGo(void)
{
    MN_ENTER_CRITICAL();
        storeMemberInt(&Simple_UI_Control, RequestToSetIdle, false);
        storeMemberInt(&Simple_UI_Control, RequestToHoldIdle, false);
    MN_EXIT_CRITICAL();
}

/** \brief LCX UI Finds the correct UI function
    Finds the UI ACtivity for the button
    \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
    \return Simple_UI_Function_t - Index to Activity
*/
static  Simple_UI_Function_t    FindFunction(LogicalButtons_t ButtonState)
{
    Simple_UI_Function_t    UI_Func;
    Simple_UI_Function_t    Index;
    bool_t                  Found;

    UI_Func = UI_FUNCTION_NONE;
    Index   = UI_FUNCTION_FINDSTOPS;        // First element
    Found   = false;

    while ((Index < UI_MAX_ACTIVE_FUNCTIONS) && (!Found))
    {
        if (Simple_UI_ActivityDesc[Index].LogButton == ButtonState)
        {
            UI_Func = Index;
            Found = true;
        }
        Index++;
    }

    return UI_Func;
}

/** \brief LCX UI Response Process parser
    Parses the response and returns the actual meaning
    \param[in] s16_least - the process's reponse
    \param[in] Simple_UI_Function_t   UI_Func - Function ID
    \return CallBackMeaning_t - Meaning of the response
*/
/** REQ 31-010; REQ 31-035; REQ 31-029 */
static  CallBackMeaning_t   ParseProcessResponse(Simple_UI_Function_t   UI_Func,
                                                 s16_least              ResponseValue)
{
    const CallBackResults_t    *pCallBackResults;
    CallBackMeaning_t           Meaning;

    if (UI_Func != UI_FUNCTION_NONE)
    {   // Legitimate Active Function
        //  Get the ptr to responses
        pCallBackResults = Simple_UI_ActivityDesc[UI_Func].pCallBackResults;

        MN_ASSERT(pCallBackResults != NULL);

        // Scan the responses
        while ((pCallBackResults->CallBackID      != ResponseValue) &&
               (pCallBackResults->CallBackMeaning != CALLBACK_END))
        {
            pCallBackResults++;
        }

        // Get the return Meaning
        Meaning = pCallBackResults->CallBackMeaning;
    }
    else
    {   // This includes FUNCTION_NONE!
        Meaning = CALLBACK_END;
    }

    return Meaning;
}

/** \brief LCX UI Get and Parse Process Response
    Gets and Parses the response and returns the actual meaning
    \param[in] Simple_UI_Function_t   UI_Func - Function ID
    \return CallBackMeaning_t - Meaning of the response
*/
static  CallBackMeaning_t   GetAndParseProcessResponse(Simple_UI_Function_t   UI_Func)
{
    CallBackMeaning_t       Meaning;
    s16_least               CallBackID;

    CallBackID = CallBackValue.CallBackID;
    Meaning = ParseProcessResponse(UI_Func, CallBackID);

    return Meaning;
}

/** \brief LCX UI Start blinking the LED at the button
    Called to start the blinking of an appropriate LED when the button is p[ressed > min but < max
    (as if the process has been started)
*/
/** REQ 31-009 */
static  void Set_ControlAction(LogicalButtons_t ButtonState)
{
    Simple_UI_Function_t    UI_Func;

    UI_Func = FindFunction(ButtonState);
    if (UI_Func != UI_FUNCTION_NONE)
    {   // Legit UI Function
        Disable_GlobalStatus();
        Display_Status(Simple_UI_ActivityDesc[UI_Func].DisplayFunctionOK);
    }
    else
    {
        Enable_GlobalStatus();
    }
}

/** REQ 31-017; REQ 31-013; REQ31-008 */
/** \brief LCX UI Pushbutton Diagnostics
    This function checks if there are keys stuck
  \param[in] uiinput PButton    - Pushbutton bitmap
*/
static void KBD_Diagnostics(uiinput_t PButton)
{
    bool_t      FaultNeeded;
    bool_t      FaultIsSet;

    // Test the Diagnostics structure
    Struct_Test(Simple_UI_KBD_Diag_t, &Simple_UI_KBD_Diag);

    // Get the information about the fault
    FaultIsSet = error_IsFault(FAULT_KEYPAD);

    if (PButton != BUTTON_NO_BUTTON)
    {   // Something is pressed

        if (Simple_UI_KBD_Diag.SomethingPressed)
        {   // Something was pressed - check if we should set the fault

            if (!FaultIsSet)
            {   // The fault not yet set, see if we should set it
                FaultNeeded = CheckTimeout(Simple_UI_KBD_Diag.TimeStart,
                                           Simple_UI_KBD_Diag.MaxTime);
                if (FaultNeeded)
                {    // Something was pressed longer than it shoould have been
                    // Set the fault
                    error_SetFault(FAULT_KEYPAD);
                }
            }
        }
        else
        {   // Nothing was pressed - start watching for the Key Stuck
            storeMemberInt(&Simple_UI_KBD_Diag, TimeStart, m_LUI_Global_CurrentTime);
            storeMemberInt(&Simple_UI_KBD_Diag, SomethingPressed, true);
        }
    }
    else
    {   // Nothing is pressed - clear fault if needed
        if (FaultIsSet)
        {   // Fault is set - clear it
            error_ClearFault(FAULT_KEYPAD);
        }

        storeMemberInt(&Simple_UI_KBD_Diag, SomethingPressed, false);
    }
}

/** \brief LCX UI State Machine handling
    Handle state Buttons Released - button processor
*/
/** REQ 31-002; REQ 31-013; REQ 31-014; REQ 31-015; REQ 31-016; REQ 31-017; REQ 31-35 */
static void ProcessButtonsReleased(void)
{
    if (SUPPORTED_BUTTON_PRESSED(Simple_UI_Control.PrevButton))
    {   // The button was supported -

        if (Simple_UI_Control.PBWithinWindow)
        {   // In the time window - go do something
            /** REQ 31-017 Launching a Function (process) */
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_START;
        }
        else
        {   // Button was pressed LESS THAN MIN or MORE THAN MAX - back to none
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_NONE;
        }
    }
    else
    {   // Unsupported logical button
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }

    Simple_UI_Control.PBWithinWindow = false;               // PB is not in the window
    Simple_UI_Control.PrevButton = BUTTON_LOG_NONE;         // No PB registered
}

/** \brief LCX UI State Machine handling
    Handle state Buttons Released - button processor. Invoked only for Active buttons
    This function handles only DOWN states of the button, we act on release of the PB,
    that action is handled by tehfunction above.

  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
/** REQ 31-002; REQ 31-013; REQ 31-014; REQ 31-015; REQ 31-016; REQ 31-017; REQ 31-035; REQ 31-029 */
static void ProcessButtonsPressed(LogicalButtons_t  ButtonState)
{
    bool_t  IfTimeAtLeast;
    bool_t  IfTimeNoLonger;

    // Handle multibuttons
    if (SUPPORTED_BUTTON_PRESSED(Simple_UI_Control.PrevButton))
    {   // Previous state was supported
        if (Simple_UI_Control.PrevButton != ButtonState)
        {   // Something changed. We process transitions 0 - 1; 1 - 1; 1 - 2.
            // Note, that if nothig was previously pressed, the number of prev. buttons is 0
            u8  NumberPrevButtons = 0;

            if (ANY_ACTIVE_BUTTON_PRESSED(Simple_UI_Control.PrevButton))
            {   // If PrevButton is a legid button
                NumberPrevButtons = Logical_UI_Buttons[Simple_UI_Control.PrevButton].NumberOfButtons;
            }

            if ((NumberPrevButtons <= Logical_UI_Buttons[ButtonState].NumberOfButtons) &&
                (SUPPORTED_BUTTON_PRESSED(ButtonState)))
            {   // Permitted transaction and Supported button
                Simple_UI_Control.UI_Function = FindFunction(ButtonState);
                Simple_UI_Control.PrevButton  = ButtonState;
                Reset_UI_Timeout();
            }
            else
            {   // Unsupported situation
                Simple_UI_Control.PrevButton  = BUTTON_LOG_UNSUPPORTED;
            }
        }
    }

    // Now handle timing
    if (SUPPORTED_BUTTON_PRESSED(Simple_UI_Control.PrevButton))
    {   // We still are supported...

        IfTimeAtLeast = CheckTimeout(Simple_UI_Control.TimeStart,
                                     Simple_UI_Control.TimeAtLeast);

        if (!IfTimeAtLeast)
        {   // Button was not pressed for at least ...
            Simple_UI_Control.PBWithinWindow = false;
        }
        else
        {   // Longer than AtLeast time

            IfTimeNoLonger = CheckTimeout(Simple_UI_Control.TimeStart,
                                          Simple_UI_Control.TimeNoLonger);
            if (!IfTimeNoLonger)
            {   // Button pressed for at least ... and less than ...
                // Start "pre-blinking" LED
                /** REQ 31-014; REQ 31-15; REQ 31-009 */
                Simple_UI_Control.PBWithinWindow = true;
                Set_ControlAction(ButtonState);
            }
            else
            {   // Pressed for way too long - back to Beacon
                /** REQ 31-017; REQ 31-013; REQ31-008 */
                Simple_UI_Control.PBWithinWindow = false;
                Enable_GlobalStatus();
            }
        }
    }
    else
    {   // Unsupported button
        /** REQ 31-008 */
        Enable_GlobalStatus();
    }
}

/** \brief LCX UI State Machine handling
    Handle state NONE
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
/** REQ 31-002; REQ 31-013; REQ 31-014; REQ 31-015; REQ 31-016; REQ 31-017; */
static void Simple_UI_Process_NONE(LogicalButtons_t ButtonState)
{
    bool_t  bLocked;
    bool_t  bProhibited;
    bool_t  bNormal;

    Enable_GlobalStatus();
    ClearProcessResponse();        // Clear Response
    Simple_UI_Control.UI_Function = UI_FUNCTION_NONE;
    Simple_UI_Control.PrevButton  = BUTTON_LOG_NONE;

    /** REQ 31-027 */
    if (Simple_UI_Control.RequestToSetIdle)
    {   // We were told to get here!
        Simple_UI_Control.RequestToSetIdle = false;
    }

    bLocked = UI_IsLUILocked();
    if (ANY_BUTTON_PRESSED(ButtonState) && (!bLocked))
    {   // Something pressed and UI is not locked -- Normal work.
        if (BUTTON_CANCEL_PRESSED(ButtonState))
        {   // Pressed - CANCEL.
            MN_ENTER_CRITICAL();
                bNormal = IsNORMALMode();
                bProhibited = IsProhibitedMode();
            MN_EXIT_CRITICAL();

            if ((!bNormal) && (!bProhibited))
            {   // Not Normal and Not prohibited - set NORMAL mode!!
                if (process_CheckProcess())
                {   // Something is running. Kill it.
                    process_CancelProcess();
                }

                /** REQ 31-007; 31-010; 31-050; 31-051 */
                (void)LUI_SetNormalDeviceMode();
            }

            // Test has a T/O
            Reset_UI_Timeout();
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_TEST_START;
        }
        else
        {   // Something else was pressed - remember and go to process
            // Are we in Prohibited mode (FAILSAFE or LOW POWER)?
            bProhibited = IsProhibitedMode();
            if (!bProhibited)
            {   // No - do the work
                // Run through the Buttons Pressed once, the go to the BUTTONS state
                ProcessButtonsPressed(ButtonState);
                // TRANSITION TO ANOTHER STATE!!
                Simple_UI_Control.Process_State = UI_PROCESS_BUTTONS;
            }
        }
    }

    if ((Simple_UI_Control.RequestToHoldIdle) &&
        (Simple_UI_Control.Process_State != UI_PROCESS_NONE))
    {   // We are told to stay here but someone tries to escape?
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
}

/** \brief LCX UI State Machine handling
    Handle state Buttons - button processor
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
/** REQ 31-002; REQ 31-013; REQ 31-014; REQ 31-015; REQ 31-016; REQ 31-017; REQ 31-035; REQ 31-029 */
static void Simple_UI_Process_BUTTONS(LogicalButtons_t ButtonState)
{
    /** REQ 31-018; REQ 31-003 */
    if (BUTTON_CANCEL_PRESSED(ButtonState))
    {   // Pressed - CANCEL - back to NONE
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
    /** REQ 31-027 */
    else if (Simple_UI_Control.RequestToSetIdle)
    {   // Request to idle - back to NONE
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
    else if (ALL_BUTTONS_RELEASED(ButtonState))
    {   // Button has been released - check the T/O
        // Note, that if the EDGE is detected it means that PB was released and pressed again!
        ProcessButtonsReleased();
    }
    else if (ANY_ACTIVE_BUTTON_PRESSED(ButtonState))
    {   // Something is still pressed
        ProcessButtonsPressed(ButtonState);
    }
    else
    {
        // Nothing
    }
}

/** \brief LCX UI State Machine handling
    Handle state Starting the proper process
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_START(LogicalButtons_t ButtonState)
{
    ErrorCode_t ret;

    /** REQ 31-018; REQ 31-003 */
    if (BUTTON_CANCEL_PRESSED(ButtonState))
    {   // Pressed - CANCEL - back to NONE
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
        Simple_UI_Control.UI_Function   = UI_FUNCTION_NONE;
    }
    /** REQ 31-027 */
    else if (Simple_UI_Control.RequestToSetIdle)
    {   // Request to idle - back to NONE
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
    else
    {   // Check if there COULD be something running
        /** REQ 31-041; REQ 31-042 */
        if (process_CheckProcess())
        {   // Something is running. Kill it.
            /** REQ 31-041; REQ 31-042 */
            process_CancelProcess();
        }
        else
        {   // Nothing is running - everything OK.
            Simple_UI_Function_t   UI_Func = Simple_UI_Control.UI_Function;

            /** REQ 31-008 */
            ClearProcessResponse();        // Prepare Response

            if (UI_Func != UI_FUNCTION_NONE)
            {   // We selected something

                // Check if it is OK to run
                /** REQ 31-040 */
                if (Simple_UI_ActivityDesc[UI_Func].ptrOK_To_Run())
                {
                    /** REQ 31-019; REQ 31-020 */
                    ret = process_SetProcessCommand(Simple_UI_ActivityDesc[UI_Func].ProcessID);
                    if (ret == ERR_OK)
                    {   // No error - TRANSITION TO ANOTHER STATE!!
                        Simple_UI_Control.Process_State = UI_PROCESS_RUNNING;
                    }
                    else
                    {   // TRANSITION TO ANOTHER STATE!!
                        Simple_UI_Control.Process_State = UI_PROCESS_CANCEL_START;
                    }
                }
                else
                {   // Somehow this is not OK to run
                    /** REQ 31-040 */
                    // Display an error and start cancellation
                    SetCallBackID(Simple_UI_ActivityDesc[UI_Func].PrechekFailedID);
                    // TRANSITION TO ANOTHER STATE!!
                    Simple_UI_Control.Process_State = UI_PROCESS_CANCEL_START;
                }
            }
            else
            {   // Selected NONE!
                // TRANSITION TO ANOTHER STATE!!
                Simple_UI_Control.Process_State = UI_PROCESS_CANCEL_START;
            }
        }
    }
}

/** \brief LCX UI State Machine handling
    Handle state process RUNNING (until it stops)
  \param[in] LogicalButtons_t   ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_RUNNING(LogicalButtons_t ButtonState)
{
    Simple_UI_Function_t    UI_Func;
    CallBackMeaning_t       Meaning;
    LED_DisplayState_t      DisplayStatus;

    /** REQ 31-018; REQ 31-003 */
    if (BUTTON_CANCEL_PRESSED(ButtonState))
    {   // Pressed - CANCEL - Cancel
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_CANCEL_START;
        Simple_UI_Control.UI_Function   = UI_FUNCTION_NONE;
    }
    /** REQ 31-027 */
    else if (Simple_UI_Control.RequestToSetIdle)
    {   // Request to idle - CANCEL
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_CANCEL_START;
    }
    else
    {   // Normal process
        if (process_CheckProcess())
        {   // Process Running - may need to do something? Check for Warnings?

            /** REQ 31-035; REQ 31-029 */
            UI_Func = Simple_UI_Control.UI_Function;
            Meaning = GetAndParseProcessResponse(UI_Func);

            // Select Function LED Blinking or WARNING? Did the process send us something?
            DisplayStatus = Simple_UI_ActivityDesc[UI_Func].DisplayFunctionOK;
            if (Meaning == CALLBACK_WARNING)
            {
                DisplayStatus = Simple_UI_ActivityDesc[UI_Func].DisplayFunctionWarning;
            }

            Display_Status(DisplayStatus);
        }
        else
        {   // Process stopped - clean up!
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_CANCEL_START;
        }
    }
}

/** \brief LCX UI State Machine handling
    Handle state Start Process Cancellation
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_CANCEL_START(LogicalButtons_t ButtonState)
{
    UNUSED_OK(ButtonState);

    if (process_CheckProcess())
    {   // If some process is running - cancel it.
        process_CancelProcess();
    }

    // TRANSITION TO ANOTHER STATE!!
    Simple_UI_Control.Process_State = UI_PROCESS_CANCEL;
}

/** \brief LCX UI State Machine handling
    Handle state Cancelling the process
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_CANCEL(LogicalButtons_t ButtonState)
{
    Simple_UI_Function_t    UI_Func;
    CallBackMeaning_t       Meaning;

    if (!process_CheckProcess())
    {   // Process is not running, OK, done

        /** REQ 31-052 */
        (void)LUI_SetNormalDeviceMode();

        if (Simple_UI_Control.RequestToSetIdle)
        {   // Request to idle - back to NONE, internal request.
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_NONE;
        }
        else
        {   // Get the error code
            /** REQ 31-021 */
            UI_Func = Simple_UI_Control.UI_Function;
            Meaning = GetAndParseProcessResponse(UI_Func);
            if (Meaning != CALLBACK_FAILED)
            {   // Process ended successfully
                // Restore DEVICE Indicator now.
                Enable_GlobalStatus();
                if (ALL_BUTTONS_RELEASED(ButtonState))
                {   // Cancel button has been released
                    // TRANSITION TO ANOTHER STATE!!
                    Simple_UI_Control.Process_State = UI_PROCESS_NONE;
                }
            }
            else
            {   // Process ended up with Error
                // Start Error Blinking
                /** REQ 31-008 */
                Display_Status(Simple_UI_ActivityDesc[UI_Func].DisplayFunctionError);
                // TRANSITION TO ANOTHER STATE!!
                Simple_UI_Control.Process_State = UI_PROCESS_ERROR;
            }
        }
    }
}

/** \brief LCX UI State Machine handling
    Handle state Error
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_ERROR(LogicalButtons_t ButtonState)
{
    /** REQ 31-018; REQ 31-010; REQ 31-003 */
    if (BUTTON_CANCEL_PRESSED(ButtonState))
    {   // Button CANCEL has been pressed - Go back to where we came from (CANCEL)
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_CANCEL;
    }
    /** REQ 31-027 */
    else if (Simple_UI_Control.RequestToSetIdle)
    {   // Request to idle - NONE
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
    /** REQ 31-048 */
    //TFS:5464
    else if (IsProhibitedMode())
    {   // Low Power or Failsafe modes --
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
    else
    {
        // Nothing
    }

    ClearProcessResponse();        // Clear Response
}

/** \brief LCX UI State Machine handling
    Handle state Start LED Testing
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_TEST_START(LogicalButtons_t ButtonState)
{
    bool_t  IfTimeOut;

    /** REQ 31-012; REQ 31-003; REQ 31-044 */
    if (!IsFAILSAFEMode())
    {   // Not a FAILSAFE mode - execute TEST state
        if (BUTTON_CANCEL_PRESSED(ButtonState))
        {   // Still pressed
            IfTimeOut = CheckTimeout(Simple_UI_Control.TimeStart,
                                     Simple_UI_Control.TimeAtLeast);
            if (IfTimeOut)
            {   // More than MIN time - move the state machine
                Disable_GlobalStatus();
                Display_Status(LED_DISPLAY_STATE_TEST);

                // TRANSITION TO ANOTHER STATE!!
                Simple_UI_Control.Process_State = UI_PROCESS_TEST;
            }
        }
        else
        {   // Button CANCEL has been released or request to idle - NONE
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_NONE;
        }
    }
    else
    {   // In FAILSAFE MODE!
        // No action -- go back to initial state
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
        ClearProcessResponse();
    }
}

/** \brief LCX UI State Machine handling
    Handle state Run LED test
  \param[in] LogicalButtons_t ButtonState - Parsed pushbutton state
*/
static void Simple_UI_Process_TEST(LogicalButtons_t ButtonState)
{
    if (Simple_UI_Control.RequestToSetIdle)
    {   // Request to idle - NONE
        // TRANSITION TO ANOTHER STATE!!
        Simple_UI_Control.Process_State = UI_PROCESS_NONE;
    }
    else
    {
        /** REQ 31-012; REQ 31-003 */
        if (!BUTTON_CANCEL_PRESSED(ButtonState))
        {   // Button CANCEL has been released - NONE
            // TRANSITION TO ANOTHER STATE!!
            Simple_UI_Control.Process_State = UI_PROCESS_NONE;
        }
    }
}

/** \brief LCX UI Handle Modes and DEVICE Status
  \param[in] uiinput PButton - pushbutton map
*/
/** REQ 31-008; REQ 31-007; REQ 31-028; REQ 31-043 */
static void Simple_UI_HandleGlobalStatus(uiinput_t PButton)
{
    LED_DisplayState_t  DisplayStatus;
    bool_t      bLocked;
    bool_t      bNotFailsafe;

    if (IsFAILSAFEMode())
    {   // Failsafe Mode - reset the state machine
        Reset_UI_StateMachineIdle();
    }

    if (Is_UI_GlobalStatus())
    {   // UI is displaqying the DEVICE Status

        bLocked      = UI_IsLUILocked();
        bNotFailsafe = !IsFAILSAFEMode();

        // Check the device mode - and control the Status LED
        if (bLocked && bNotFailsafe && (PButton != BUTTON_NO_BUTTON))
        {   // UI Locked - if any button pressed - we indicate the Locked state
            /** REQ 31-045; REQ 31-046 */
            DisplayStatus = LED_DISPLAY_LOCKED_UI;
        }
        else
        {   // Display the modes
            if (IsNORMALMode())
            {   // Normal Operating mode - beacon blinking
                DisplayStatus = LED_DISPLAY_STATE_NORMAL;

                // Check for the certain faults and lit the Status LED acordingly
                if (error_IsAnyFaultWithAttributes(FATTRIB_ANNUNCIATE))
                {   // There are some faults - Blink the Fault
                    DisplayStatus = LED_DISPLAY_STATE_FAULT;
                }
            }
            else
            {   // Not Normal mode - check for Failsafe
                if (IsFAILSAFEMode())
                {   // FailSafe !
                    DisplayStatus = LED_DISPLAY_STATE_FAILSAFE;
                }
                else if (IsLOWPOWERMode())
                {   // Low Power Mode
                    DisplayStatus = LED_DISPLAY_STATE_DARK;
                }
                else
                {   // Setup / Manual mode
                    DisplayStatus = LED_DISPLAY_STATE_HART_OVERRIDE;
                }
            }
        }

        // Set Status
        Display_Status(DisplayStatus);
    }
}

//-------------------------------------------------------------------

/** \brief LCX UI State Machine handling
    Main State machine driver
  \param[in] uiinput PB_Function - pushbutton map
  \param[in] uiinput PB_Edges - pushbutton edges map
  \return uiinput - the modified pushbutton map
*/
static uiinput_t    DriveStateMachine(uiinput_t PB_Function, uiinput_t PB_Edges)
{
    LogicalButtons_t    ButtonState;
    LogicalButtons_t    EdgesState;
    uiinput_t           retval;

    //--------------------------------------------------------------------
    // Update Time Outs related vars

    m_LUI_Global_CurrentTime = bios_GetTimer0Ticker();              // Read current Time

    //--------------------------------------------------------------------
    // Execute KBD diagnostics

    KBD_Diagnostics(PB_Function & (uiinput_t)BUTTON_MASK);

    //--------------------------------------------------------------------
    // Check for CANCEL button
    ButtonState = ParsePB(PB_Function & (uiinput_t)BUTTON_MASK);    // Parse PBs
    EdgesState  = ParsePB(PB_Edges & (uiinput_t)BUTTON_MASK);       // Parse PBs edges

    // TFS:5511 -- if we get PB pressed (as it wwas BUT with an edge -- then we can say
    // that the PB has been re-pressed; i.e. released and pressed again.
    // Here we have the Edegs State indicating the edge on PBs...
    if ((Simple_UI_Control.PrevButton != BUTTON_LOG_NONE) &&
        (Simple_UI_Control.PrevButton != BUTTON_LOG_UNSUPPORTED) &&
        (Simple_UI_Control.PrevButton != BUTTON_LOG_CANCEL))
    {   // Something was pressed and now we have an edge

        if (EdgesState == Simple_UI_Control.PrevButton)
        {   // And it is right on that button
            // Indicate a release!
             ButtonState = BUTTON_LOG_NONE;
        }
    }

    Simple_UI_Control.CurrentButton = ButtonState;

    /** EQ 31-003; REQ 31-028 */
    retval = (ButtonState == BUTTON_LOG_CANCEL) ?
                (uiinput_t)BUTTON_CANCEL :
                BUTTON_NO_BUTTON;  // If cancel button is pressed - send the cancel signal

    //--------------------------------------------------------------------
    // Process the state machine
    switch (Simple_UI_Control.Process_State)
    {
        case UI_PROCESS_NONE :                      // Nothing, NONE!
            Simple_UI_Process_NONE(ButtonState);
            break;

        case UI_PROCESS_BUTTONS :                   // Process Function Buttons
            Simple_UI_Process_BUTTONS(ButtonState);
            break;

        case UI_PROCESS_START :                     // Launching the process
            Simple_UI_Process_START(ButtonState);
            break;

        case UI_PROCESS_RUNNING :                   // Process running
            Simple_UI_Process_RUNNING(ButtonState);
            break;

        case UI_PROCESS_CANCEL_START :              // Starting the process
            Simple_UI_Process_CANCEL_START(ButtonState);
            break;

        case UI_PROCESS_CANCEL :                    // Trying to cancel the process
            Simple_UI_Process_CANCEL(ButtonState);
            break;

        case UI_PROCESS_ERROR :                     // Trying to Error processing
            Simple_UI_Process_ERROR(ButtonState);
            break;

        case UI_PROCESS_TEST_START :                // Starting the LED TEST
            Simple_UI_Process_TEST_START(ButtonState);
            break;

        case UI_PROCESS_TEST :                      // LED TEST
            Simple_UI_Process_TEST(ButtonState);
            break;

        default :
            Simple_UI_Control.Process_State = UI_PROCESS_NONE;
            (void)LUI_SetNormalDeviceMode();
            break;
    } // switch

    // Checksum the updated structure
    STRUCT_CLOSE(Simple_UI_Control_t, &Simple_UI_Control);

    return retval;
}

//-------------------------------------------------------------------

static  u32 Simple_UI_main(uiinput_t pushbutton, uiinput_t edges)
{
    uiinput_t retval;

    //--------------------------------------
    // LCX addition - peridoic LED Strucrture test
    LED_PeriodicStructureTest();

    //--------------------------------------------------------------------
    // Peridoic Lock structure test
    UI_PeriodicLockData_Test();

    //--------------------------------------------------------------------
    // Test the control structure
    Struct_Test(Simple_UI_Control_t, &Simple_UI_Control);

    if (Simple_UI_Control.UIEngaged)
    {   // LUI Engaged!

        /** REQ 31-008 */
        Simple_UI_HandleGlobalStatus(pushbutton);

        /** REQ 31-005 */
        Simple_UI_JumperHandler();

        // TFS:5511
        retval = DriveStateMachine(pushbutton, edges);
    }
    else
    {   // LUI IS NOT Engaged - No Action
        retval = 0U;
    }

    return retval;
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------

/** \brief Pushbutton handler of the MNCB LCD UI

  Handles pushbuttons and, internally, node timeout.
  Displays the current node if there is anything new.
*/

u32 ui_main()
{
    uiinput_t c;
    uiinput_t edges;

    c = sysio_GetPushButtons(); //get the buttons no matter what; there is side effect to it

    //-------------------------------------------------------------------
    // Simple UI Emulation ...
    //
    // The Simple_UI_Emulation variable is intentionally defined as boolean constant
    // to permamnently alter the execution flow without dismantling the existing
    // UI code. This way the EMulation code is executed.
    // The line below afffect only PC-Lint warnings

    // TFS:5511
    edges = (c >> BUTTON_WIDTH);

    // Add edges to the PB status - to catch the short presses
    c |= (u8)(c >> BUTTON_WIDTH);

    // TFS:5511
    return Simple_UI_main(c, edges);
    //-------------------------------------------------------------------
}

//============== Public API ================
/** \brief Initializer of the MNCB LCD UI

  Initializes the UI state and finds the initial UI node to set as current.
  (And displays it, too.)
*/

void ui_init()
{
    // Handle LCX UI Main Data Structure
    Struct_Copy(Simple_UI_Control_t, &Simple_UI_Control, &Simple_UI_Control_Default);

    // Handle KBD Diagnostics structure
    Struct_Copy(Simple_UI_KBD_Diag_t, &Simple_UI_KBD_Diag, &Simple_UI_KBD_Diag_Default);
}

/** \brief Internal function to force the callback to certain value
  \parameter[in] s16_least node_id - info from the Process to UI
*/
/** REQ 31-040 */
static void SetCallBackID(s16_least node_id)
{
    CallBackMeaning_t       Meaning;
    Simple_UI_Function_t    UI_Func;

    MN_ENTER_CRITICAL();
        UI_Func = Simple_UI_Control.UI_Function;
        if (UI_Func != UI_FUNCTION_NONE)
        {   // Function is legid.
            Meaning = ParseProcessResponse(UI_Func, node_id);
            if (Meaning != CALLBACK_END)
            {   // Found something meaningful - store it
                CallBackValue.CallBackID = node_id;
            }
        }
    MN_EXIT_CRITICAL();
}

/** \brief "Callback" function fromProcess to UI
  \parameter[in] s16_least node_id - info from the Process to UI
*/
/** REQ 31-035; REQ 31-010; REQ 31-021; REQ 31-029 */
void ui_setNext(s16_least node_id)
{
    Simple_UI_Function_t    UI_Func;
    u8                      CurrentProcess;

    MN_ENTER_CRITICAL();
        UI_Func = Simple_UI_Control.UI_Function;
        if (UI_Func != UI_FUNCTION_NONE)
        {   // Function is legid.
            CurrentProcess = process_GetProcId();
            if (Simple_UI_ActivityDesc[UI_Func].ProcessID == CurrentProcess)
            {   // We are doing something
                SetCallBackID(node_id);
            }
        }
    MN_EXIT_CRITICAL();
}

/** \brief LUI API - Enable the UI controls
*/
void UI_EngageUI(void)
{
    MN_ENTER_CRITICAL();
        // Engage UI
        storeMemberInt(&Simple_UI_Control, UIEngaged, true);
        // And reset to idle
        storeMemberInt(&Simple_UI_Control, RequestToSetIdle, true);
    MN_EXIT_CRITICAL();
}

/** \brief LUI API - Disengage LUI control
*/
void UI_DisengageUI(void)
{
    MN_ENTER_CRITICAL();
        storeMemberInt(&Simple_UI_Control, UIEngaged, false);
    MN_EXIT_CRITICAL();
}

/** \brief LUI API - Check if UI is engaged
    \return bool_t  - true if the UI is engaged, false - otherwise
*/
bool_t  UI_IsUIEngaged(void)
{
    return Simple_UI_Control.UIEngaged;
}

//------------------------------------------------------------------
// STUB functions

/** \brief Extended "Callback" function fromProcess to UI
  \parameter[in] s16_least node_id - info from the Process to UI (passed to ui_setNext)
  \parameter[in] s16_least value - ignored
*/

void ui_setNextExt(s16_least node_id, s16 value)
{
    UNUSED_OK(value);
    ui_setNext(node_id);
}

/** A mandatory API for autotune and such.
Intends to print num/10**decpoint on a dedicated line
\param num - number to write
\param decpoint - number of decimal places
*/
void ui_DebugWriteNumber(s32 num, u8 decpoint)
{
    //No place to write
    UNUSED_OK(num);
    UNUSED_OK(decpoint);
}

/* This line marks the end of the source */
//------------------------------------------------------------------
