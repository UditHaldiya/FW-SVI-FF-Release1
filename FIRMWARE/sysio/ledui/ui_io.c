/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_io.c
    \brief Implementation of system-level I/O related to local UI (LCX flavor)

    CPU: Any (with standard C compiler)

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/ledui/ui_io.c $
    $Date: 1/10/12 12:19p $     $Revision: 6 $     $Author: Arkkhasin $

    $History: ui_io.c $
 * 
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 1/10/12    Time: 12:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio/ledui
 * (Partially) restored LCX build broken by work on AP build (TFS:8577).
 * No LCX code change except fault table where flash test timeout is still
 * inactive
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:10p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio/ledui
 * TFS:8577 - header refactoring
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 8/16/10    Time: 6:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/sysio/ledui
 * Bug 3871 - removed unused header file.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 8/11/10    Time: 4:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/sysio/ledui
 * Bug 3776 - Show Crash now will lit Green LED 1 continiously ON to
 * indicate an assert.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/15/10    Time: 1:05a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/sysio/ledui
 * Extracted sysio_ShowCrash from mnassert.c
 * Project-specific sysio_GetPushButtons
*/

#include "config.h"
#include "mnwrap.h"
#include "mncbtypes.h"
#include "sysio.h"

#include "uipublic.h"
#include "uiinput.h"
#include "uisimple.h"

#include "oswrap.h"
#include "watchdog.h"

// -------------------- UI support --------------------------
/** \brief Gets buttons state/edge info
    \return buttons info usable by the UI
*/
uiinput_t sysio_GetPushButtons(void)
{
    uiinput_t edge;
    u8 status =
#if KBD_OPTION_EDGE_UP == OPTION_OFF
        PUSHBUTTON_DOWN_EDGE;
#endif
#if KBD_OPTION_EDGE_UP == OPTION_ON
        PUSHBUTTON_RELEASE_EDGE;
#endif
    edge = bios_ReadPushButtonStatus(&status); //also for side effects
    edge = (edge << BUTTON_WIDTH)|(uiinput_t)status;
    return edge;
}


#ifndef NDEBUG
/** \brief Stays in a tight loop until a key is pressed
\param message - ignored
\param line - ignored
*/
void sysio_ShowCrash(const void *message, s32 line)
{
    UNUSED_OK(message);
    UNUSED_OK(line);
    u8 edge;

    //---------------------------------------
    // LED LUI - Green LED 1 is continiously ON
    LED_SetState(LED_DISPLAY_STATE_DEBUG_1);

    do
    {
        wdog_WatchdogTrigger();

        bios_HandleDigitalInput(); //reads the input and tickles the LCD watchdog; don't care if skipped processing
        edge = PUSHBUTTON_DOWN_EDGE;
        (void)bios_ReadPushButtonStatus(&edge); //don't care about the state
    } while(edge == 0);
}
#endif //NDEBUG

/* This line marks the end of the source */
