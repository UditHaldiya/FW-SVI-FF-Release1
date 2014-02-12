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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/project_LCX/diagctlout.c $
    $Date: 1/30/12 1:31p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* $History: diagctlout.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:31p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/services/project_LCX
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
*/
#include "mnwrap.h"
#include "insignal.h"
#include "pwm.h"
#include "control.h"
#include "devicemode.h"

#define IN_DEFAULT_LOW 4000
#define IN_DEFAULT_HIGH 20000
#define IN_CONVERT_COEF1 102
#define IN_CONVERT_COEF2 25

//doc in the project-independent header
s32 mode_DirectOutput(ctlmode_t ctlmode, s32 ctlout)
{
    if(ctlmode == CONTROL_IP_DIAGNOSTIC)
    {
        sig_t signal = sig_GetSignal();
        if (signal < IN_DEFAULT_LOW)
        {
            ctlout = MIN_DA_VALUE;
        }
        else if (signal > IN_DEFAULT_HIGH)
        {
            ctlout = MAX_DA_VALUE;
        }
        else
        {   // 102/25=4.08 is about 4.096 which to convert from 4K-20K to 0-2^16
            ctlout = (s32)((((signal-IN_DEFAULT_LOW))*IN_CONVERT_COEF1)/IN_CONVERT_COEF2);
        }
    }
    return ctlout; //no guard in the positioner
}
/* This line marks the end of the source */
