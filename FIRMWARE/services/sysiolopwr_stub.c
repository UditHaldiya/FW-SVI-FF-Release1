/*
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file sysiolopwr_stub.c
    \brief Dummy Implementation of the Low Power limiting functions and calculations

     CPU: Any

    $Revision: 1 $
    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/sysiolopwr_stub.c $
    $Date: 1/30/12 12:46p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup poscontrol
*/

/* $History: sysiolopwr_stub.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:46p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
*/

#include "mnwrap.h"
#include "errcodes.h"
#include "pwm.h"

#include "control.h"
#include "sysiolopwr.h"
#include "power.h"

//-----------------------------------------------------------------------------------
// Low Power Handling variables


/************************************************/

/** \brief Before allowing Integral activity, check to make sure that someone
    has not upplugged the I/P.  Since it is possible to get a low enough value
    in the PWM that there be no I/P current we will allow integral action to
    a point that would generate up 150 micro amps of I/P current.

    \return - HI limit for PWM
*/
u16 sysio_CalculateIPLimitsForControl(void)
{
    return MAX_DA_VALUE;
}

/**
    \brief This function is main function and externally called to run control.
     It gets raw data of signal, which is then temperature compensated and converted
    to the standard range.
     The signal level will be identified if it is normal (NORMAL_POWER) or
     low power (LOW_POWER) or marginal power (MARGINAL_POWER)
     Position measurement is read in and processed and converted to the standard range.
     Both signal and position data are smoothed due to its noise
     function control_Prepare(data) and control_PID(data) are called to execute PID control.
*/
void sysio_Handle_LowPowerLimiting(void)
{
    u8  LowPowerState;

    LowPowerState = control_GetLowPowerState();

    if (LowPowerState != NORMAL_POWER)
    {
        LowPowerState = NORMAL_POWER;
        control_SetLowPowerState(LowPowerState);
    }

    UNUSED_OK(MARGINAL_POWER);
}

/**
    \brief A GET-type function returns upper limit of PWM value
           limited by low current.

    \return the upper PWM limit
*/
/**REQ 40 (TFS:4108)**/
u16 sysio_GetPWMHightLimit(void)
{
    return MAX_DA_VALUE;
}

/**
  \brief  This function checks to see if the power is currently
  above the operating threshold.
  \param[in] none
  \return - bool_t - true if there is enough power
*/
bool_t power_IsOK(void)
{
    return true;
}

/* This line marks the end of the source */
