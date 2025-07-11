/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file led.c
    \brief Driver for Low-Cost LEDs

    CPU: NXP LPC21x4 (ARM)

    OWNER: Anatoly Podpaly

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/src/led.c $
    $Date: 11/04/11 4:06p $
    $Revision: 25 $
    $Author: Arkkhasin $

    \ingroup test
*/

/* $History: led.c $
 * 
 * *****************  Version 25  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:06p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/src
 * TFS:8140 OS PORT upgrade
 * Headers fixed
 *
 * *****************  Version 23  *****************
 * User: Arkkhasin    Date: 1/28/11    Time: 1:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Undo TFS:5505
 *
 * *****************  Version 21  *****************
 * User: Anatoly Podpaly Date: 12/16/10   Time: 11:06a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 5067 - Init LED now sets LED OFF.
 *
 * *****************  Version 20  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 3:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 4359 - Added definition for the Platform Hardware. Added
 * conditional compilation based on the type of the Platform H/W.
 *
 *
 * *****************  Version 19  *****************
 * User: Justin Shriver Date: 9/16/10    Time: 11:01a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * TFS:4121 Fix led drive for final hardware
 *
 * *****************  Version 18  *****************
 * User: Anatoly Podpaly Date: 8/18/10    Time: 5:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3923 - incorporated results of code review: Add doxygen comments;
 * GPIO0->IOSET = (u8)index << LED_SHIFT; - correct cast; Function
 * LED_Initialize - remove return (void function); Add ALL _OFF LED name
 * (Placeholder); MN_DBG_ASSERT in set pattern function should be full
 * assert; correct critical sections.
 *
 *
 * *****************  Version 17  *****************
 * User: Anatoly Podpaly Date: 8/16/10    Time: 6:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3871 - removed blinking patterns from driver level to app level.
 *
 * *****************  Version 16  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 11:12a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3627: Added new Debug LED state and blinking pattern.
 *
 * *****************  Version 15  *****************
 * User: Anatoly Podpaly Date: 7/29/10    Time: 9:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3627: Updated blinking patterns and introduced a different model of
 * handling of LED display. The LED display is treated as a cluster of
 * indicators, reflecting the state of the ckluster vs. the state of an
 * individual LED. The blinking pattern were reviewed by the custiomer
 * (marketing) and are intrduced below (they are subject to change without
 * warning and comments):
 *
 * Normal: G1(50ms) -- G2(50ms)    -- G3(50ms) – ALL_OFF(1000ms) à
 * Beginning
 * Fault: G1(50ms) -- G2(50ms) -- G3(50ms) – R (50ms) -- ALL_OFF(50ms) –
 * R(50ms) – ALL_OFF(50ms) -- R(50ms) – ALL_OFF(1000ms) à Beginning
 * FAILSAFE: R(Forever)
 * Error: G*(250ms) – R(250ms) à Beginning
 * Warning: G*(137ms) – ALL_OFF(138ms) à Beginning
 * Select / Running: G*(250ms) – ALL_OFF(750ms) à Beginning
 * HART Override: R(500ms) – ALL_OFF(500ms) à Beginning
 * Test: G1(250ms) -- G2(250ms) -- G3(250ms) – R(250ms) à Beginning
 *
 * *****************  Version 14  *****************
 * User: Anatoly Podpaly Date: 7/16/10    Time: 6:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3536 - Added blinking pattern for FAULT indication.
 *
 * *****************  Version 13  *****************
 * User: Anatoly Podpaly Date: 6/02/10    Time: 4:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * 3311: matched LED blinking patterns with LUI emulator
 *
 * *****************  Version 12  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 3:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3279 - added new warning blinking mode; added All OFF function,
 * removed unused blinking patterns.
 *
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 5:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * ADded more commentsre:requirements.
 *
 * *****************  Version 10  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 5:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Added commens to indicate the LCX LUI requirements. Requirements
 * (Mod_31) are listed in an appropriate document.
 *
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 5/14/10    Time: 1:31p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3156:code mode so all LEDs could be turned OFF.
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 3/19/10    Time: 6:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug #2815: code review mods.
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 3/01/10    Time: 10:19a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Corrected LED driver for clarity.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 2/26/10    Time: 5:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Added ERROR type blinking - alternate blinking.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 4:26p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Added and include file mnassert.h
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 4:24p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Added ASSERT to check LED # and LED state validity.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:53a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LCX development - corrected LED handling.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 2/04/10    Time: 7:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Code review updates: replaced Struct_Copy with STRUCT_CLOSE.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:29p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LCX impelmentation : LED handler.
 *
*/

#include "mnwrap.h"
#include "oswrap.h"
#include "mnassert.h"
#include "lpc2114io.h"
#include "bios.h"
#include "bios_def.h"

//--------------------------------------------------------
// Local LED control structures

/** \brief LED control Structure
*/
typedef struct LED_Controls_t
{
    const BlinkingStateMachine_t   *BlinkingPattern;        // !< Ptr to the blinking pattern
    u8                              BlinkingPatternIndex;   // !< Index of the blinking pattern state
    tick_t                          LatchedTicks;           // !< Elapsed tick counter latched at the beginning of teh state
    tick_t                          StateTicks;             // !< LED State Tick counter
    u16                             CheckWord;              // !< Checkword

} LED_Controls_t;

static LED_Controls_t  LED_Controls =
{
    .BlinkingPattern        = NULL,
    .BlinkingPatternIndex   = 0u,
    .LatchedTicks           = 0u,
    .StateTicks             = 0u,
    .CheckWord              = 0u
};

//--------------------------------------------
/** \brief LED Driver - Set LED address and turn it ON
*/
static void LED_SetON(LEDNumber_t index)
{   // Set GPIOs: LED_0 and LED_1
    GPIO0->IOCLR = (GPIO0_LED0 | GPIO0_LED1);
    GPIO0->IOSET = ((u32)index) << LED_SHIFT;
    // Set GPIO LED_CTL to ON (LO)
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
    GPIO0->IOCLR = GPIO0_LED_CTL;
#else
    GPIO0->IOSET = GPIO0_LED_CTL;
#endif
}

/** \brief LED Driver - Turn LED OFF
*/
static void LED_SetOFF(void)
{   // Set GPIO LED_CTL to OFF (HI)
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
    GPIO0->IOSET = GPIO0_LED_CTL;
#else
    GPIO0->IOCLR = GPIO0_LED_CTL;
#endif
}

/** \brief LED Initialize - init the LED driver
*/
void LED_Initialize(void)
{
    // Turn all the LEDs OFF
    LED_SetOFF();

    // Checksum the structure
    STRUCT_CLOSE(LED_Controls_t, &LED_Controls);
}

/** \brief LED Driver - periodically called from PROCESS task
           Test the LED control; structures
*/
void LED_PeriodicStructureTest(void)
{
    Struct_Test(LED_Controls_t, &LED_Controls);
}

static void LED_DisplayPatternState(void)
{
    if (LED_Controls.BlinkingPattern[LED_Controls.BlinkingPatternIndex].Led < ALL_LEDS_OFF)
    {   // Some LED ON
        LED_SetON(LED_Controls.BlinkingPattern[LED_Controls.BlinkingPatternIndex].Led);
    }
    else
    {   // ALL OFF
        LED_SetOFF();
    }
}

/** \brief LED_SetDisplayState - set the mode (bliinking off, on etc.) for teh specific LED
  \param[in] BlinkingStateMachine_t    *NewPattern - pointer to APp specific blinking patters.
*/
void LED_SetPattern(const BlinkingStateMachine_t    *NewPattern)
{
    MN_ASSERT(NewPattern != NULL);

    Struct_Test(LED_Controls_t, &LED_Controls);

    if (LED_Controls.BlinkingPattern != NewPattern)
    {   // Only if we are changing something
        MN_ENTER_CRITICAL();
            LED_Controls.BlinkingPattern      = NewPattern;
            LED_Controls.LatchedTicks         = bios_GetTimer0Ticker();
            LED_Controls.BlinkingPatternIndex = 0u;
            LED_Controls.StateTicks           = LED_Controls.BlinkingPattern[LED_Controls.BlinkingPatternIndex].StateTicks;
            STRUCT_CLOSE(LED_Controls_t, &LED_Controls);

            LED_DisplayPatternState();
        MN_EXIT_CRITICAL();
    }
}

/** \brief LED Driver crankshaft - maintains the LED statuses and executes the blinking
    Called from CONTROL task
*/
void LED_PeriodicUpdate(void)
{
    tick_t      ElapsedTicks;
    tick_t      CurrentTimerTicks;
    tick_t      LocalStateTicks;
    u8          StateIndex;

    if (LED_Controls.BlinkingPattern == NULL)
    {   // Non-initialized blinking pattern - set LED OFF
        LED_SetOFF();
    }
    else
    {
        CurrentTimerTicks = bios_GetTimer0Ticker();

        ElapsedTicks = CurrentTimerTicks - LED_Controls.LatchedTicks;
        if (ElapsedTicks >= LED_Controls.StateTicks)
        {   // Time to move!
            MN_ENTER_CRITICAL();
                StateIndex = LED_Controls.BlinkingPatternIndex;

                // Move to the new state in the blinking pattern
                if (LED_Controls.BlinkingPattern[StateIndex].LastState)
                {   // This is the last state in the pattern - wrap
                    StateIndex = 0u;
                }
                else
                {   // Just go to the next state
                    StateIndex++;
                }

                // Store new values
                LocalStateTicks = LED_Controls.BlinkingPattern[StateIndex].StateTicks;

                // Store new values...
                storeMemberInt(&LED_Controls, StateTicks,           LocalStateTicks);
                storeMemberInt(&LED_Controls, LatchedTicks,         CurrentTimerTicks);
                storeMemberInt(&LED_Controls, BlinkingPatternIndex, StateIndex);

                LED_DisplayPatternState();

            MN_EXIT_CRITICAL();
        }
    }
}

// end of source
