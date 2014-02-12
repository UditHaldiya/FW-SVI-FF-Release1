/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_devmodes.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App. Only the UI graph tables need to see any functions here.
    Device mode interface part.

    The glue functions here are designed to work in concert with the macro
ProtectedEntry (see uilocks.inc) to implement a state machine as follows:
Password prompt (timed out in a standard timeout interval) and password entry
are both allowed if:
- the "area" is locked, i.e. 3rd param of ProtectedEntry is >= UI lock level, and
- UI password is enabled.
The "area" is considered unlocked if
- it was not locked, or
- UI password is enabled and the entered password matches UI password

Heavy inter-dependency between node functions is uncharacteristic of the UI design
where the complexity is typically hidden in the UI graph itself, rather than in
node functions.
The reason for this dependency is that ProtectedEntry is a macro which is expected
to appear multiple times, so the size of a subgraph outweighs the size of the
supporting code.

         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_devmodes.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_devmodes.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Derek Li     Date: 6/10/09    Time: 5:25p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Updated ui_setOperMode
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:08p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted from uifunc.{c,h}
 * Eliminated FAULT_LOW_POWER from DLT
*/

#include "mnwrap.h"
#include "bitutils.h"
#include "uidef.h"
#include "faultpublic.h"
#include "ui_devmodes.h"
#include "devicemode.h"
#include "reset.h"
#include "devicemode.h"

static devmode_t mimicryMode;

/** A node function
    \brief switches to normal mode
    \return always true to use for its side effect.
*/
bool_t ui_setOperMode(const uistate_t *state)
{
    UNUSED_OK(state);
    //(void)mode_SetNormalMode();
    //return true; //stay in the node; mode change will force UI to the right place
    ErrorCode_t err =
#ifdef OLD_DEVMODE
        mode_SetOperateMode();
#else
        mode_SetNormalMode();
#endif
    return err == ERR_OK;
}

/** A node function
    \brief switches to manual mode and reinits UI on success of actual change
    \return always true to use for its side effect.
*/
bool_t ui_setManMode(const uistate_t *state)
{
    UNUSED_OK(state);
#ifdef OLD_DEVMODE
    (void)mode_SetManualMode(); //whatever the outcome!
#else
    (void)mode_SetMode(MODE_MANUAL); //whatever the outcome!
#endif
    /* NOTE: mode_SetManualMode() forces UI to the initial node if the
       mode is actually changed.
       The usage of this function must therefore account for it
    */
    return true; //keep the failure display node disabled
}

/** A node function
    \brief switches to manual mode and preserves UI state by suppressing what the mode change may have done
    \return false on failure to use for its side effect; true on success for navigating the UI graph.
*/
bool_t ui_setManModeNaked(const uistate_t *state)
{
    UNUSED_OK(state);
    bool_t ret = false; //enable the failure display node
    MN_ENTER_CRITICAL();
#ifdef OLD_DEVMODE
        if(mode_SetManualMode() == ERR_OK)
#else
        if(mode_SetMode(MODE_MANUAL) == ERR_OK)
#endif
        {
            /* NOTE: mode_SetManualMode() forces UI to the initial node if the
               mode is actually changed.
               We need to undo it
            */
            ui_setNext(-1); //Cancel forced node change
            ret = true; //keep the failure display node disabled
        }
    MN_EXIT_CRITICAL();
    return ret;
}

/** A node function
    \brief sets normal OOS (setup) mode
    \return true if mode is set
*/
bool_t ui_setOOSMode(const uistate_t *state)
{
    UNUSED_OK(state);
#ifdef OLD_DEVMODE
    (void)mode_SetOOSMode(SUBMODE_OOS_NORMAL);
    return mode_ClearFailsafeMode();
#else
    ErrorCode_t err =
        mode_SetMode(MODE_SETUP);
    return err == ERR_OK;
#endif
}

/** A node function
    \brief sets Failsafe OOS mode
    \return true if mode is set (always!)
*/
bool_t ui_setFailSafeMode(const uistate_t *state)
{
    UNUSED_OK(state);
#ifdef OLD_DEVMODE
    (void)mode_SetOOSMode(SUBMODE_OOS_FAILSAFE);
#else
    (void)mode_SetMode(MODE_FAILSAFE);
#endif
    return true;
}


//--------- reset -----------------
/** A node function
    \brief implements access to HW reset (with required prototype)
    \return false (actually, doesn't return)
*/
bool_t ui_reset(const uistate_t *state)
{
    UNUSED_OK(state);
    reset_HardwareReset();        // does not return
    return false;                   // make compiler happy
}

//device mode
const void *ui_GetModeData(void)
{
#ifdef OLD_DEVMODE
    return mode_GetMode();
#else
    return mode_GetModeData(NULL);
#endif
}

//device mode
const void *ui_GetXModeData(void)
{
    mimicryMode = mode_GetMode();
    mimicryMode = mode_GetEffectiveMode(mimicryMode);
    return &mimicryMode;
}

//device submode
const void *ui_GetDevSubmode(void)
{
    static devsubmode_t s;
#ifdef OLD_DEVMODE
    s = mode_GetMode()->submode;
#else
    s = mode_GetSubmode();
#endif
    return &s;
}

/* This line marks the end of the source */
