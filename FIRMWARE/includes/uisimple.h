/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uisimple.h
    \brief Public UI API specific to LCD-based menued UI system

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/uisimple.h $
    $Date: 1/05/12 3:54p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: uisimple.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:54p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8577 LED-UI-specific API
*/
#ifndef UISIMPLE_H_
#define UISIMPLE_H_

//-------------------------------------------------------------------
// LED - Specific information : LED Blinking States (Pattern Numbers)
// These states define the STate of the whole LED UI display
// Each of the states is assigned an UI LED Display Pattern (state machine)
// that is displayed by the LED driver.
typedef enum {

    LED_DISPLAY_STATE_NORMAL,
    LED_DISPLAY_STATE_HART_OVERRIDE,
    LED_DISPLAY_STATE_FAULT,
    LED_DISPLAY_STATE_FAILSAFE,
    LED_DISPLAY_STATE_TEST,

    LED_DISPLAY_STATE_SELECTED_1,
    LED_DISPLAY_STATE_SELECTED_2,
    LED_DISPLAY_STATE_SELECTED_3,

    LED_DISPLAY_STATE_WARNING_1,
    LED_DISPLAY_STATE_WARNING_2,
    LED_DISPLAY_STATE_WARNING_3,

    LED_DISPLAY_STATE_ERROR_1,
    LED_DISPLAY_STATE_ERROR_2,
    LED_DISPLAY_STATE_ERROR_3,

    // Do not use these patterns! Reserved
    LED_DISPLAY_STATE_DARK,
    LED_DISPLAY_STATE_DEBUG_1,
    LED_DISPLAY_STATE_DEBUG_2,
    LED_DISPLAY_STATE_DEBUG_3,
    LED_DISPLAY_STATE_DEBUG_4,
    //End of reserved patterns

    LED_DISPLAY_LOCKED_UI,

    LED_DISPLAY_STATE_MAX

} LED_DisplayState_t;


extern LED_DisplayState_t   LED_GetDisplayState(void);
extern void LED_SetState(LED_DisplayState_t LEDState);


// Get Jumper parameters
/** REQ 31-004; REQ 31-024; REQ 31-025 */
extern bool_t           UI_Get_ImmediateATO(void);
extern u8               UI_Get_ImmediatePIDIndex(void);

#ifdef OLD_NVRAM
void LUI_Save_Jumpers(void);
void LUI_Init_Jumpers(InitType_t Type);
#endif

typedef struct LUI_Jumpers_t
{
    u8  JumpersBits;
    u8  Reserved;
    u16 CheckWord;
} LUI_Jumpers_t;

SAFESET(LUI_SetJumpers, LUI_Jumpers_t);
SAFEGET(LUI_GetJumpers, LUI_Jumpers_t);

#endif //UISIMPLE_H_
/* This line marks the end of the source */
