/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file led.h
    \brief header for BIOS LED routines

    CPU: NXP LPC2114 LPC2124

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc/led.h $
    $Date: 8/18/10 5:13p $
    $Revision: 12 $
    $Author: Anatoly Podpaly $
*/

/* (Optional) $History: led.h $
 * 
 * *****************  Version 12  *****************
 * User: Anatoly Podpaly Date: 8/18/10    Time: 5:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 3923 - incorporate results of the code review: add DOXYGEN comments
 * and info; Type ticks_t is redundant, remove and use tick_t; LED_STATUS<
 * First LED - shold be removed, green*led defs belong to app layer, not
 * to the driver. (uimain.c); Add ALL _OFF LED name (Placeholder).
 * 
 * 
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 8/16/10    Time: 6:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 3871 - removed blinking patterns from the driver level to
 * application level.
 * 
 * *****************  Version 10  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 11:11a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 3627: Added new LED Debug state.
 * 
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 7/29/10    Time: 9:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 3627: Updated blinking patters based on customer request.
 * Introduced new model of handling LEDs in the process. 
 * 
 * This header file contains the list of LED Display states (treated as a
 * cluster of indicators) and updated API.
 * 
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 7/16/10    Time: 6:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 3536 - replaced Reserved blinking pattern with FAULT indicating
 * blinking pattern.
 * 
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 6/02/10    Time: 4:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * 3311: matched LED blinking patterns with LUI emulator
 * 
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 3:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * 3279 - corrected blinking patrterns. Added ALL OFF function prototype.
 * 
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 3/19/10    Time: 6:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug #2815: code review mods.
 * 
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 3/04/10    Time: 1:24p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Added pubic comments re: LED blinking modes
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 2/26/10    Time: 5:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Added error type blinking mode.
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:48a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Ongoing development - corrected LED handling, added two new blinking
 * modes.
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:27p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * LCX implementation : LED driver.
 * 
*/

#ifndef LED_H_
#define LED_H_

//--------------------------------------------------------------------
/** \brief H/W LEDs 
    These are H/W definitions for the direct drive.
*/
typedef enum {
    HARDWARE_LED_0,
    HARDWARE_LED_1,
    HARDWARE_LED_2,
    HARDWARE_LED_3,

    NUMBER_OF_LEDS                  // !< Number of Active LEDs

} LEDNumber_t;

/** \brief Logical LED - ALL OFF!!
    This LED does not exist, indicates that ALL LEDS shall be OFF.
*/
#define ALL_LEDS_OFF                (NUMBER_OF_LEDS)

//--------------------------------------------------------------------
/** \brief The structure below defines the states for the LED blinking state machine 
*/
typedef struct BlinkingStateMachine_t
{
    LEDNumber_t     Led;            // !< LED to operate, ALL_LEDS_OFF - All LEDs are OFF
    tick_t          StateTicks;     // !< How long to be in this state
    bool_t          LastState;      // !< The specified LED is ON for this many ticks.

} BlinkingStateMachine_t;

//--------------------------------------------------------------------
/** \brief API Functions */

/** \brief LED Initialize - init the LED driver
*/
extern void LED_Initialize(void);

/** \brief LED Driver crankshaft - maintains the LED statuses and executes the blinking
    Called from CONTROL task
*/
extern void LED_PeriodicUpdate(void);

/** \brief LED Driver - periodically called from PROCESS task
           Test the LED control; structures
*/
extern void LED_PeriodicStructureTest(void);

/** \brief LED_SetDisplayState - set the mode (bliinking off, on etc.) for teh specific LED
  \param[in] BlinkingStateMachine_t    *NewPattern - pointer to APp specific blinking patters.
*/
extern void LED_SetPattern(const BlinkingStateMachine_t    *NewPattern);

#endif // LED_H_
//--------------------------------------------------------------------

/* End of the source */
