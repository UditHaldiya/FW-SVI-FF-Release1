/*
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file fullthrottle.c
    \brief Implementation of the full-throttle runs caused by Tight Shutoff,
           Tight Open and FF Full Open/Close

     CPU: Any

    $Revision: $
    OWNER: SK
    $Archive: $
    $Date: $
    $Revision: $
    $Author: $

    \ingroup
*/
/* $History: $

*/
#include "mnwrap.h"
#include "oswrap.h"
#include "cycle.h"
#include "devicemode.h"
#include "pwm.h"           /* WritePwm */
#include "sysio.h"
#include "control.h"
#include "ctllimits.h"
//#include "pneumatics.h"
LINT_PURE(pneu_IsSensorInvalid)
#include "position.h"
#include "fullthrottle.h"


/** private defines */
#define SLOW_APPROACH    FIVE_PCT_819
#define TSO_HYSTERESIS   HALF_PCT_82


/** \brief This function is called just prior to the call to control_control().
           control_control() will be skipped if it returns true.
    \parmam[in] void
    \return bool_t m_bBypassControl:
            if true, control_control() call is skipped.
*/
bool_t full_BypassControl(void)
{
    return  full_GetFullThrottleEnabled();
}

typedef struct CutoffConf_t
{
    faultcode_t fcode;
    u16 output[Xends];
} CutoffConf_t;

static const CutoffConf_t CutoffConf[Xends] =
{
    [Xlow] = //cutoff lo
    {
        .fcode = FAULT_POS_CUTOFF_LO,
        .output =
        {
            [0] = MAX_DA_VALUE, //for ATC
            [1] = MIN_DA_VALUE, //for ATO
        },
    },
    [Xhi] = //cutoff hi
    {
        .fcode = FAULT_POS_CUTOFF_HI,
        .output =
        {
            [0] = MIN_DA_VALUE, //for ATC
            [1] = MAX_DA_VALUE, //for ATO
        },
    },
};

/** \brief This function is externally called to determine if the valve is in Full Open/Close
    \parmam[in] void
    \return bool_t m_bFullThrottleEnabled:
            if true, valve is in Full Open/Close state, otherwise it is controlled.
*/
bool_t full_GetFullThrottleEnabled(void)
{
    return  error_IsFault(CutoffConf[Xlow].fcode) || error_IsFault(CutoffConf[Xhi].fcode);
}


/** \brief This function is called from control task prior to control_control()
     It detects whether there exist conditions for Tight Shutoff or Open (under AO
     block) or for Full Open/Close (under DO block).
     If conditions exist it allows control_control() to take over until the
     defined position is reached. After that it assumes control and pushes the
     valve "full-throttle" to the physical limit.
    \param[in/out] pointer to boolean bBypassControl - used to bypass control_control()
    \return    FF compatible status.
*/
void full_FullThrottle(void)
{
    CtlLimits_t         ctlLimits;

    //get control limits (with "Tight" limits)
    (void)control_GetLimits(&ctlLimits);

    //find Setpoint and Position
    s32 Setpoint = ctllim_GetRangeBoundedSetpoint();
    s32 Position = vpos_GetScaledPosition(); //implicit cast is safe (s16 --> s32)
    bool_t bATO = pos_GetPositionConf(NULL)->bATO;
    ctlmode_t ctlmode = mode_GetIntendedControlMode(NULL);


    s32 testval1 = Setpoint;
    s32 thresh1 = ctlLimits.TightShutoff[Xlow];
    s32 testval2 = Position;
    s32 thresh2 = thresh1;

    for(u8_least x=Xlow; x<Xends; x++)
    {
        //Cutoff works only in closed-loop mode (where setpoint makes sense)
        if((ctlLimits.EnableTightShutoff[x] == 0) || (ctlmode != CONTROL_MANUAL_POS))
        {
            error_ClearFault(CutoffConf[x].fcode);
        }
        else
        {
            //we are or aren't in tight shutoff - see regardless, out if not in zone
            if( testval1 > (thresh1 + TSO_HYSTERESIS))
            {
                /* Condition tested:
                [Xlo]
                Setpoint > ctlLimits.TightShutoff[Xlow] + TSO_HYSTERESIS
                [Xhi]
                ctlLimits.TightShutoff[Xhi] > Setpoint + TSO_HYSTERESIS, or
                Setpoint < ctlLimits.TightShutoff[Xhi] - TSO_HYSTERESIS
                */
                error_ClearFault(CutoffConf[x].fcode);
            }
            else
            {
                if( (testval1 <= thresh1) && (testval2 <= (thresh2 + SLOW_APPROACH) )  )
                {
                    /* Condition 1 tested:
                    [Xlo]
                    Setpoint <= ctlLimits.TightShutoff[Xlow]
                    [Xhi]
                    ctlLimits.TightShutoff[Xhi] <= Setpoint, or
                    Setpoint >= ctlLimits.TightShutoff[Xhi]
                    */
                    /* Condition 2 tested:
                    [Xlo]
                    Position <= ctlLimits.TightShutoff[Xlow] + SLOW_APPROACH
                    [Xhi]
                    ctlLimits.TightShutoff[Xhi] <= Position + SLOW_APPROACH, or
                    Position >= ctlLimits.TightShutoff[Xhi] - SLOW_APPROACH
                    */
                    error_SetFault(CutoffConf[x].fcode);
                }
            }
            if(error_IsFault(CutoffConf[x].fcode))
            {
                u16 CtlOutputValue = CutoffConf[x].output[bool2int(bATO)];
                (void)sysio_WritePwm(CtlOutputValue, PWMSTRAIGHT); //don't care about limiting
            }
        }
        thresh1 = testval1;
        testval1 = ctlLimits.TightShutoff[Xhi];
        thresh2 = testval2;
        testval2 = testval1;
    }
} // ----- end of full_FullThrottle() -----


/* This line marks the end of the source */
