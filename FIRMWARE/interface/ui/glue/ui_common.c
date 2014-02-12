/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_uisetup.c
    \brief UI glue layer functions to support UI configuration

    (Originally Extracted from uifunc.c ver. 138)

         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_common.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_common.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/09/09    Time: 11:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * If level measurement is not available, UI displays "!LEVEL" without
 * units or number in the cycling menu.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:47a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Commonly used UI glue hookups extracted here
*/

#include "mnwrap.h"
#include "uidef.h"
#include "ui_common.h"
#include "uiglue_common.h"
#include "process.h"
#include "uigluehelpers.h"
#include "configure.h"
#include "mncbdefs.h"
#include "position.h"

uivalvar_t var_misc; //! to support var display of long processes etc.

/** A node data function.
\return a pointer to an internal location presumably (see UI graph) prepared before.
NOTE: RED_FLAG: the internal location is NOT chosen prudently: a node change
request can overwirite it. It may cause momentary garbage value on the LCD.
*/
s16 *ui_getMiscVar(void)
{
    return &var_misc;
}

const s16 *ui_GetMNCBVerNumber(void)
{
    return &VerNumber[0];
}

//legacy compatibility
const void *ui_GetOptionConfig(void)
{
    return cnfg_GetOptionConfig();
}


//-------------------- process support --------------------------
/**  A node function
\brief launch "self-tune" from pushbuttons
\return true if OK, false if cannot launch
*/
bool_t ui_process_SelfTune(const uistate_t *state)
{
    UNUSED_OK(state);
    return !ui_process(PROC_AUTOTUNE);
}

static uivalvar_t proc_percent;

bool_t ui_ReadProcPercentComplete(const uistate_t *state)
{
    UNUSED_OK(state);
    proc_percent = process_CheckProcessProgress();
    return false;
}

uivalvar_t *ui_GetProcPercentComplete(void)
{
    return &proc_percent;
}

const void *ui_getProcId(void)
{
    static u8 id;
    CONST_ASSERT(sizeof(id)==sizeof(ProcId_t));
    id = process_GetProcId();
    return &id;
}

bool_t ui_HavePosition(const uistate_t *state)
{
    UNUSED_OK(state);
    return pos_GetPosState(NULL)->lifeleft != 0;
}

bool_t ui_DontHavePosition(const uistate_t *state)
{
    return !ui_HavePosition(state);
}
/* This line marks the end of the source */
