/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file diagctlout.c
    \brief Implementation of diagnostic PWM ourput

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/project_FFAP/diagctlout.c $
    $Date: 1/30/12 1:26p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* $History: diagctlout.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:26p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/services/project_FFAP
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
*/
#include "mnwrap.h"
#include "pwm.h"
#include "control.h"
#include "numutils.h"

//this is a stub to populate as needed
s32 mode_DirectOutput(ctlmode_t ctlmode, s32 ctlout)
{
    if(ctlmode == CONTROL_IP_DIAGNOSTIC)
    {
        ctlout = CLAMP(ctlout, (s32)MIN_DA_VALUE, (s32)MAX_DA_VALUE);
    }
    return ctlout; //no guard in the positioner
}
/* This line marks the end of the source */
