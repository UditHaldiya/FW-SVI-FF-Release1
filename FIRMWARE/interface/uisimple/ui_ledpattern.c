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

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/

/*
    $History: ui_ledpattern.c $
 * 
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 5:14p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8140 OS PORT upgrade
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 3:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4359 - Added definition for the Platform Hardware. Added
 * conditional compilation based on the type of the Platform H/W.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 10/08/10   Time: 4:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Addressing LINT.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 10/08/10   Time: 3:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 1:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * LINT.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 11:14a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * This file contains the LED blinking patterns (definition) and one
 * service function (between BIOS and App) to set the LED pattern.
 *
 * These patterns were extracted from the uimain to separate the
 * functionality.
*/

//-------------------------------------------------------------------
// Simple (LED) UI LED Blinking patterns and LED Driver interface

#include "mnwrap.h" //for MN_NOINIT
#include "oswrap.h"
#include "mnassert.h"
#include "bios_def.h"
#include "bios.h"
#include "uipublic.h"
#include "uiprivate.h"
#include "timebase.h"

//------------------------------------------------
// Defines for blinking patterns

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
// LCX Proto
#define     LED_GREEN_1     (HARDWARE_LED_0)
#define     LED_GREEN_2     (HARDWARE_LED_1)
#define     LED_GREEN_3     (HARDWARE_LED_2)
#define     LED_RED         (HARDWARE_LED_3)

#else
// For REAL H/W
#define     LED_GREEN_1     (HARDWARE_LED_3)
#define     LED_GREEN_2     (HARDWARE_LED_1)
#define     LED_GREEN_3     (HARDWARE_LED_2)
#define     LED_RED         (HARDWARE_LED_0)

#endif

//------------------------------------------------

#define TIME_GREEN_NORMAL_ON        (T0_050)
#define TIME_NORMAL_PAUSE           (T1_000)

#define TIME_RED_DOT_FAULT_ON       (T0_050)
#define TIME_RED_DOT_FAULT_OFF      (T0_050)

#define TIME_OVERRIDE_ON            (T0_500)
#define TIME_OVERRIDE_OFF           (T0_500)

#define TIME_TEST_ON                (T0_250)

#define TIME_SELECT_ON              (T0_250)
#define TIME_SELECT_OFF             (T1_000 - T0_250)

#define TIME_WARNING_ON             ((T0_250 + (T0_050 / 2u)) / 2u)
#define TIME_WARNING_OFF            ((T0_250 + (T0_050 / 2u)) / 2u)

#define TIME_ERROR_GREEN_ON         (T0_250)
#define TIME_ERROR_RED_ON           (T0_250)

#define TIME_LOCKED_PAUSE           (T0_250)

//------------------------------------------------
// Blinking patterns
static const BlinkingStateMachine_t NormalStatePattern[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_NORMAL_PAUSE,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t OverrideStatePattern[] =
{
    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_OVERRIDE_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_OVERRIDE_OFF,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t FaultStatePattern[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_RED_DOT_FAULT_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_RED_DOT_FAULT_OFF,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_RED_DOT_FAULT_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_RED_DOT_FAULT_OFF,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_RED_DOT_FAULT_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_NORMAL_PAUSE,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t FailsafeStatePattern[] =
{
    {   // R ON
        .Led        = LED_RED,
        .StateTicks = T2_500,
        .LastState  = true
    }
};

/** REQ 31-045; REQ 31-046; REQ 31-008; REQ 31-011 */
static const BlinkingStateMachine_t LockedStatePattern[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_RED_DOT_FAULT_ON,
        .LastState  = false
    },

    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_GREEN_NORMAL_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_LOCKED_PAUSE,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t DarkStatePattern[] =
{
    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = T2_500,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t DebugStatePattern_1[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = T2_500,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t DebugStatePattern_2[] =
{
    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = T2_500,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t DebugStatePattern_3[] =
{
    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = T2_500,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t DebugStatePattern_4[] =
{
    {   // R ON
        .Led        = LED_RED,
        .StateTicks = T2_500,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t TestStatePattern[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_TEST_ON,
        .LastState  = false
    },

    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_TEST_ON,
        .LastState  = false
    },

    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_TEST_ON,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_TEST_ON,
        .LastState  = true
    }
};

// Functional blinking patterns
// Selected / Running
static const BlinkingStateMachine_t SelectedRunningStatePattern_1[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_SELECT_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_SELECT_OFF,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t SelectedRunningStatePattern_2[] =
{
    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_SELECT_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_SELECT_OFF,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t SelectedRunningStatePattern_3[] =
{
    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_SELECT_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_SELECT_OFF,
        .LastState  = true
    }
};

// -- Warnings
static const BlinkingStateMachine_t WarningStatePattern_1[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_WARNING_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_WARNING_OFF,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t WarningStatePattern_2[] =
{
    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_WARNING_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_WARNING_OFF,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t WarningStatePattern_3[] =
{
    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_WARNING_ON,
        .LastState  = false
    },

    {   // ALL OFF
        .Led        = ALL_LEDS_OFF,
        .StateTicks = TIME_WARNING_OFF,
        .LastState  = true
    }
};

// Errors
static const BlinkingStateMachine_t ErrorStatePattern_1[] =
{
    {   // G1 ON
        .Led        = LED_GREEN_1,
        .StateTicks = TIME_ERROR_GREEN_ON,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_ERROR_RED_ON,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t ErrorStatePattern_2[] =
{
    {   // G2 ON
        .Led        = LED_GREEN_2,
        .StateTicks = TIME_ERROR_GREEN_ON,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_ERROR_RED_ON,
        .LastState  = true
    }
};

static const BlinkingStateMachine_t ErrorStatePattern_3[] =
{
    {   // G3 ON
        .Led        = LED_GREEN_3,
        .StateTicks = TIME_ERROR_GREEN_ON,
        .LastState  = false
    },

    {   // R ON
        .Led        = LED_RED,
        .StateTicks = TIME_ERROR_RED_ON,
        .LastState  = true
    }
};

//------------------------------------------------
// List of all patterns
// Pointer to a Blinking Pattern
typedef const BlinkingStateMachine_t *PointerToPattern_t;
// Array of pointers to Blinking Patterns
static const PointerToPattern_t PatternArray[] =
{
    NormalStatePattern,
    OverrideStatePattern,
    FaultStatePattern,
    FailsafeStatePattern,
    TestStatePattern,

    //---------------------
    SelectedRunningStatePattern_1,
    SelectedRunningStatePattern_2,
    SelectedRunningStatePattern_3,
    WarningStatePattern_1,
    WarningStatePattern_2,
    WarningStatePattern_3,
    ErrorStatePattern_1,
    ErrorStatePattern_2,
    ErrorStatePattern_3,

    //---------------------
    DarkStatePattern,

    //---------------------
    DebugStatePattern_1,
    DebugStatePattern_2,
    DebugStatePattern_3,
    DebugStatePattern_4,
    //---------------------

    LockedStatePattern
};

// Check the size of the array vs number of elements in it
// Number of pointers shall match the Number of Display States.
CONST_ASSERT((sizeof(PatternArray) / sizeof(PointerToPattern_t)) == (u16)LED_DISPLAY_STATE_MAX);

//-------------------------------------------------------------------

/** \brief LED_SetState - set the state (bliinking off, on etc.) for the LED display
    This function isolates LED blinkig patterns from LED Display States.
  \param[in] LedSTate - LED state to Set
*/
void UI_SetPatternByIndex(LED_DisplayState_t LEDState)
{
    MN_DBG_ASSERT(LEDState < LED_DISPLAY_STATE_MAX);

    if (LEDState >= LED_DISPLAY_STATE_MAX)
    {   // Check the range - outside of the range
        LEDState = LED_DISPLAY_STATE_DARK;
    }

    // Set the pattern
    LED_SetPattern(PatternArray[LEDState]);
}

//------------------------------------------------------------------
