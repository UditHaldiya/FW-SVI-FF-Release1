/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_sp.c
    \brief UI support functions for manual setpoint

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_sp.c $
    $Date: 1/06/12 3:16p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_sp.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:16p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/08/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Repaired AP and ESD builds
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 11:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * A few small glue layer functions extracted from uifunc.{c,h}
*/

#include "mnwrap.h"
#include "devicemode.h"
#include "param.h"
#include "uidef.h"
#include "ui_sp.h"
#include "ctllimits.h"

static uivalvar_t setpoint;
/**  A var node data function
\brief Get manual setpoint for display
\return pointer to a scratchpad location now holding manual setpoint
*/
uivalvar_t *ui_getManSetpoint(void)
{
    s32 sp;
    (void)mode_GetIntendedControlMode(&sp); //ctl mode not used

    setpoint = (s16)CLAMP(sp, POS_SETPOINT_LOW_INT, POS_SETPOINT_HIGH_INT);
    return &setpoint;
}


/** A var node function
    \brief Set manual setpoint from UI input
*/
void ui_setManSetpoint(void)
{
    //We don't care if setpoint fails to set:
    mode_SetControlMode(CONTROL_MANUAL_POS, setpoint);
}

bool_t ui_IPout_low(const uistate_t *state)
{
    UNUSED_OK(state);
    mode_SetControlMode(CONTROL_IPOUT_LOW, SETPOINT_INVALID);
    return false;
}
bool_t ui_IPout_high(const uistate_t *state)
{
    UNUSED_OK(state);
    mode_SetControlMode(CONTROL_IPOUT_HIGH, SETPOINT_INVALID);
    return false;
}

uivalvar_t *ui_GetSetpoint(void)
{
    s32 sp;
    (void)mode_GetIntendedControlMode(&sp); //ctl mode not used
    setpoint = (s16)CLAMP(sp, SETPOINT_MIN, SETPOINT_MAX);
    return &setpoint;
}

/* This line marks the end of the source */
