/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_sigsp.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App. Only the UI graph tables need to see any functions here.

    If a function is used as an entry function of an (extended) navigation node,
    the node is still disabled if it is en-table-enabled but the entry function
    returns non-zero. (IMPORTANT: the function *is not* called if the node is
    table-disabled, so the function's side-effects, if any, are not computed.)

    If a function is used as an exit function of an (extended) navigation node,
    it is called every time the node is about to be left to follow the SELECT link
    (usually associated with a * but defined by the node's input translation
    function); the node cannot be left if the function returns non-0.

    NOTE: There is a design pattern where a table-enabled node with an entry
    function serves as a transient UI state, e.g., to show data save error or
    (future?) operator error and such. E.g., with xlate_NextIfEnabled() input
    translation function, such a node falls through if disabled or follows the
    advance link if enabled. If a node is designed to be always disabled, it can
    be achieved by a node function always returning a non-zero (and thus called
    for its side effects). This can do clever things. :)

         CPU: Any

    OWNER: AK

    \ingroup UI
*/
#include "mnwrap.h"
#include "ui_sigsp.h"
#include "insignal.h"
#include "signalsp.h"

//local options
//UI display options
#define UI_DISPLAY_FILTERED_SIGNAL OPTION_ON

hiloSignal_t hiloSignal; //!< signal data for UI
/** A helper
    \brief adjusts signal limits according to the limits and reguired min difference
    \return false (always success)
*/
static void sig_lim(void)
{
    hiloSignal.lowupperlim = MIN(hiloSignal.nHighSignal-INTSIGVAL(SIGNAL_DIFFERENCE), INTSIGVAL(LOW_SIGNAL_HIGH_LIMIT));
    hiloSignal.highlowerlim = MAX(hiloSignal.nLowSignal
    +INTSIGVAL(SIGNAL_DIFFERENCE),
    INTSIGVAL(HIGH_SIGNAL_LOW_LIMIT));
}

/** A node function
    \brief adjusts signal limits according to the limits and reguired min difference
    \return false (always success)
*/
bool_t ui_AdjustSigLimits(const uistate_t *state)
{
    UNUSED_OK(state);
    sig_lim();
    return false;
}

/** A node function
    \brief gets cal data set in the menus (not in long processes)
    \return false (always success)
*/
bool_t ui_getHiLoSignalData(void)
{
    const ComputedSignalData_t* csd = sig_GetComputedSignalData();

    hiloSignal.nLowSignal = csd->nLowSignal;
    hiloSignal.nHighSignal = csd->nHighSignal;

    sig_lim();
    return false;
}

//--------- put signal data -----------------
/** A node function
    \brief saves cal data set in the menus (not in long processes)
    \return false on success, true otherwise
*/
bool_t ui_setHiLoSignalData(void)
{
    //AK:TODO: Do not call cnfg_Set... if data didn't change!
    SignalData_t sd;
    sd.LowSignal = hiloSignal.nLowSignal;
    sd.HighSignal = hiloSignal.nHighSignal;
    return (cnfg_SetSignalData(&sd) == ERR_OK);
}

//lint -emacro(740, FIRSTVAR_IN_STRUCT) unusual pointer cast; C std guarantees correctness
#define FIRSTVAR_IN_STRUCT(s) ((s16 *)&(s))


/** A node var accessor function for the UI graph
*/
s16 *ui_getHiLoSignal(void)
{
    return FIRSTVAR_IN_STRUCT(hiloSignal);
}

/** A node data function.
\return a pointer to the signal value from an internal location
NOTE: RED_FLAG: the internal location is NOT chosen prudently: a node change
request can overwirite it. It may cause momentary garbage value on the LCD.
*/
uivalvar_t *ui_GetSignal(void)
{
#if UI_DISPLAY_FILTERED_SIGNAL == OPTION_OFF
    var_misc = sig_GetSignal();
#endif
#if UI_DISPLAY_FILTERED_SIGNAL == OPTION_ON
    var_misc = sig_GetSmoothedSignal();
#endif //UI_DISPLAY_FILTERED_SIGNAL
    return &var_misc;
}

/* This line marks the end of the source */
