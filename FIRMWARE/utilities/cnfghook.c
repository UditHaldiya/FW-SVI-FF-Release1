/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file cnfghook.c
    \brief Repairing the things spanning several components when one of them changes
    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/utilities/cnfghook.c $
    $Date: 11/15/11 1:46p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup configure
*/
/* $History: cnfghook.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 1:46p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
*/

#include "mnwrap.h"
#include "position.h"
#include "control.h"
#include "devicemode.h"
#include "ctllimits.h"
#include "configure.h"

/** \brief A required hook to fix things spanning several components when
position configuration changes. This version fixes the setpoint and control limits.

It must be pretty fast as it is called from within a critical section

\param oldposconf - a pointer to old (computed) position settings
\param newposconf - a pointer to new (computed) position settings
*/
void cnfghook_CorrectForNewPositionConf(const ComputedPositionConf_t *oldposconf, const ComputedPositionConf_t *newposconf)
{
    pos_least_t Setpoint;
    ctlmode_t ControlMode;
    control_GetSetpointPosition(&Setpoint, NULL);
    control_GetControlMode(&ControlMode, NULL);
    ControlMode = ctllim_ConditionControlMode(ControlMode, &Setpoint);
    pos_least_t NewSetpoint = pos_UnAdjustExt(Setpoint, oldposconf);
    NewSetpoint = pos_AdjustStdPositionExt(NewSetpoint, newposconf);
    /**TFS:4713 */ //clamp setpoint to -5% -- +160% to avoid its jump when OS Adj is reduced.
    NewSetpoint = CLAMP(NewSetpoint, POS_SETPOINT_LOW_INT, POS_SETPOINT_HIGH_INT);

    if(NewSetpoint != Setpoint)
    {
        mode_SetControlMode(ControlMode, NewSetpoint); //If fails (e.g. normal mode), that's fine, too
        ctllim_Reinit(NewSetpoint);
    }
}
/* This line marks the end of the source */
