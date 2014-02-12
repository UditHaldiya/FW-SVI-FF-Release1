/**
Copyright 2006 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file okctlmodes.c
    \brief Collections of control modes compatible with device modes (for SVI2AP project).
            The first comatible mode is the default (guessed) control mode
            for a given device mode.

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/project_LCX/okctlmodes.c $
    $Date: 1/30/12 1:30p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* (Optional) $History: okctlmodes.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:30p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services/project_LCX
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 12:28p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/project_LCX
 * 4.
 * Get services\project_LCX from the trunk
 *     Remove powermncb.c services\project_LCX\conf.mak
 *
 * *****************  Version 18  *****************
 * User: Arkkhasin    Date: 11/16/09   Time: 3:17p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Light linting
 *
 * *****************  Version 17  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 6:20p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Removed obsolete conversion.{c,h}
 *
 * *****************  Version 16  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 12:20a
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * (internally used) setpoint is now in "hard stops" (unscaled) domain
 * Position-related headers refactored
 *
 * *****************  Version 15  *****************
 * User: Arkkhasin    Date: 4/24/08    Time: 12:06p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * mode_TransitionHook() moved from devicemode.c to okctlmodes.c (too
 * project-dependent)
 *
 * *****************  Version 14  *****************
 * User: Arkkhasin    Date: 3/27/08    Time: 3:26p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Characterization (of signal setpoint) moved to charact_sig.{c,h}
 *
 * *****************  Version 13  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 5:34p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * modes and submodes renamed to more appropriate names
 * mode_GuardSetpoint() and mode_GuessSetpoint() take a devmode_t instead
 * of const ModeData_t *
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 2/27/08    Time: 1:06p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Removed deprecated control_GetSetpointPosition()
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 10:06p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Adapted to new sysio interfaces
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 1/03/08    Time: 6:56p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * A massive but interim check-in:
 * - Initialization is handled with a common routine
 * - ATO/ATC moved to PositionStop to improve modularity
*/
#include "mnwrap.h"
#include "errcodes.h"
#include "devicemode.h"
#include "devfltpriv.h"
#include "signalsp.h"
#include "charact.h"
#include "insignal.h"
#include "position.h"
//#include "posctlmode.h"

/* Arrays of various possible device mode-compatible control modes
*/
static const ctlmode_t NormalControlModes[] =
{
    CONTROL_MANUAL_POS
};
static const ctlmode_t ManualControlModes[] =
{
    CONTROL_MANUAL_POS,
    CONTROL_IPOUT_LOW,
    CONTROL_IPOUT_HIGH,
    CONTROL_IP_DIAGNOSTIC //AK:RED_FLAG: Quiestionable!
        //CONTROL_IPOUT_LOW_PROC, CONTROL_IPOUT_HIGH_PROC are not (yet?) allowed in AP
};

static const ctlmode_t OOSNormalControlModes[] =
{
    CONTROL_MANUAL_POS,
    CONTROL_IPOUT_LOW,
    CONTROL_OFF,
    CONTROL_IPOUT_HIGH,
    CONTROL_IPOUT_LOW_PROC,
    CONTROL_IPOUT_HIGH_PROC,
    CONTROL_IPOUT_HIGH_FACTORY,
    CONTROL_IP_DIAGNOSTIC
};

static const ctlmode_t OOSFailsafeControlModes[] =
{
    CONTROL_IPOUT_LOW,
    CONTROL_IP_DIAGNOSTIC,
    CONTROL_IPOUT_HIGH_FACTORY
};

#define LENLIST(array) NELEM(array),array
#define CTLMODELIST_END {MODE_SETUP, SUBMODE_NORMAL, .count = 0, NULL}

/* A list of allowed control modes pointed to by an entry is applicable if
mode==entry.mode and (submode & entry.submode_mask) == entry.submode.
See project-independent mode_IsCompatible()
*/
const ctlmodelist_t ctlmodelist[] =
{
    {MODE_OPERATE, SUBMODE_NORMAL, SUBMODE_LOWPOWER|SUBMODE_FAILED_HIGH, LENLIST(NormalControlModes)},
    {MODE_MANUAL, SUBMODE_NORMAL, SUBMODE_ESD_TRIP|SUBMODE_LOWPOWER|SUBMODE_FAILED_HIGH, LENLIST(ManualControlModes)},
    {MODE_SETUP, SUBMODE_NORMAL, SUBMODE_ESD_TRIP|SUBMODE_LOWPOWER|SUBMODE_FAILED_HIGH, LENLIST(OOSNormalControlModes)},
    {MODE_FAILSAFE, SUBMODE_NORMAL, SUBMODE_NORMAL, LENLIST(OOSFailsafeControlModes)},
    {MODE_SETUP, SUBMODE_LOWPOWER, SUBMODE_LOWPOWER, LENLIST(OOSFailsafeControlModes)},
    CTLMODELIST_END,
};

/** \brief A wrapper function encapsulating actions required on device mode change
MUST be called from within a critical section.
\param newmode - new device mode
*/
bool_t mode_TransitionHook(devmode_t newmode)
{
    UNUSED_OK(newmode);
    return true;
}

/** \brief Returns position setpoint as set (with inverse characterization applied)
\return inversely characterized position setpoint (in digital setpoint domain)
*/
s32 digsp_GetDigitalSetpoint(void)
{
    s32 sp;
    (void)mode_GetIntendedControlMode(&sp);
    //apply inverse characterization here
    return sp;
}

/** \brief Returns effective device mode, backward-compatible
\param mode - local device mode
\return effective device mode
*/
devmode_t mode_GetEffectiveMode(devmode_t mode)
{
    return mode; //no conversion
}



//doc in the project-independent header
s32 mode_GuardSetpoint(devmode_t mode, ctlmode_t ctlmode, s32 sp)
{
    UNUSED_OK(ctlmode);
    if(mode == MODE_OPERATE)
    {
        sig_t signal = sig_GetSignal();
        sp = sigsp_ConvertSignalToSetpoint(signal);
    }
    return sp; //no guard in the positioner
}

/* take the previous setpoint and guard it
*/
s32 mode_GuessSetpoint(devmode_t mode, ctlmode_t ctlmode)
{
    s32 pos;
    UNUSED_OK(mode);
    UNUSED_OK(ctlmode);
    pos = vpos_GetScaledPosition();
    return pos; //current valve position is the (manual) setpoint
}

devmode_t mode_RepairStartupMode(devmode_t mode)
{
    if((mode == MODE_SETUP) || (mode == MODE_MANUAL))
    {
        //OK
    }
    else if(mode == MODE_OPERATE)
    {
        //Depends on configuration, so still OK
    }
    else
    {
        //treat it as setup - can only occur if invalid or FAILSAFE was commited
        mode = MODE_SETUP;
    }
    return mode;
}


bool_t mode_IsPersistent(devmode_t mode)
{
    return mode != MODE_FAILSAFE;
}


/* This line marks the end of the source */

