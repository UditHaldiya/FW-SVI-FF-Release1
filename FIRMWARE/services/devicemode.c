/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup Modeman Mode Change/Management module
\brief Device mode and control mode management

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_ModeChange Module.doc"> Design document </a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/devicemode1HTML.html"> Unit Test Report [1]</a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/devicemode2HTML.html"> Unit Test Report [2]</a><br>
\endhtmlonly
*/
/**
    \file devicemode.c
    \brief Mode change functions

    This module contains the functions needed for changing modes

This version abandons the notion of the stored target mode.
(The target mode is the mode where the device goes on power-up (or any other reset,
including in-place reset) AND if low power condition disappears.)
A valid mode can be:
- Normal
- Manual
- Setup (a.k.a. OOS/Normal)
- Failsafe

A valid submode can be present in any mode (which is different from the previous
design where only out-of-service mode had submodes). Submodes are
- Normal - no special condition
- Low power
- Failhigh
- Trip (the output is held to low limit)

Submodes flavor the modes but when the corresponding conditions go away, the behavior
corresponding to the mode resumes.

Failsafe mode is, conceptually, almost a submode (and was a submode in the previous
design). On (cold) reset, if there are no failsafe faults, the device resumes its
previous mode (or goes to setup - TBD).
However, clearing the failsafe mode says nothing about the mode the
device is going to.


The behavior of bumpless transfer is as follows.
- The bumpless process saves in FRAM the fact it was running
- Before completion, it sets normal mode and clears the FRAM info
- On reset, the initialization sets normal mode if FRAM says so.

- It is allowed from the setup mode as well as from the manual mode
- If the position error changes sign, the transfer is considered successfully completed

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/services/devicemode.c $
    $Date: 12/14/09 7:29p $
    $Revision: 132 $
    $Author: Arkkhasin $

    \ingroup Modeman
*/
/* (Optional) $History: devicemode.c $
 *
 * *****************  Version 132  *****************
 * User: Arkkhasin    Date: 12/14/09   Time: 7:29p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Removed duplicate code from mode_GetEffectiveControlMode()
 *
 * *****************  Version 131  *****************
 * User: Arkkhasin    Date: 6/24/09    Time: 1:33a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Repaired mode initialization for correct trap processing (bug 1240)
 *
 * *****************  Version 130  *****************
 * User: Arkkhasin    Date: 5/01/09    Time: 12:49a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Implemented functional requirements 2.0, 2.1 of
 * http://mymasoneilan/en/Engineering/innovations/Team%20Documents1/Projec
 * ts/HDLT12400/Common%20Links/Feature%20Descriptions/Fail%20Safe%20Behavi
 * or/FailSafe.doc:
 * "Out of failsafe" means "follow the sensor"
 * Fixed bugs 782, 830
 *
 * *****************  Version 129  *****************
 * User: Arkkhasin    Date: 4/17/09    Time: 1:38p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Startup mode resolution separated between positioners and transmitters
 *
 * *****************  Version 128  *****************
 * User: Arkkhasin    Date: 3/31/09    Time: 6:41p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * 1. Low power submode set by fault attributes (not by a fault)
 * 2. Wicked kludge to deal with the disappearing FAULT_ESD_TRIP.
 * Needs improvement.
 *
 * *****************  Version 127  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 126  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 1:01a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Removed unused control.h
 * Removed other unused headers.
 *
 * *****************  Version 125  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:10p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Removed wrapper header utility.h
 *
 * *****************  Version 124  *****************
 * User: Arkkhasin    Date: 3/04/09    Time: 3:46p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Allowed mode_GuardSetpoint() to change control mode
 *
 * *****************  Version 123  *****************
 * User: Arkkhasin    Date: 2/25/09    Time: 7:09p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Wicked kludge to deal with the disappearing FAULT_MARGINAL_POWER. Needs
 * improvement.
 *
 * *****************  Version 122  *****************
 * User: Arkkhasin    Date: 2/13/09    Time: 1:54p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Replaced #error with a much milder comment
 *
 * *****************  Version 121  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 8:07p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Removed mode_Correct()
 * Added mode_Mopup
 * Improved internal representation of device modes
 * Device mode is now formatted for HART independently from internal
 * representation
 * Removed mode_Correct()
 * Added NVMEM interface for mode
 *
 * *****************  Version 120  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 2:02p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Light linting
 *
 * *****************  Version 119  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 12:33a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Removed unused obsolete "valvepos.h"
 *
 * *****************  Version 118  *****************
 * User: Arkkhasin    Date: 5/23/08    Time: 10:04a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * TRAPID_BAD_NVMEM removed - now handled by setting a fault only
 *
 * *****************  Version 117  *****************
 * User: Arkkhasin    Date: 4/24/08    Time: 12:18p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * mode_TransitionHook() moved from devicemode.c to okctlmodes.c (too
 * project-dependent)
 *
 * *****************  Version 116  *****************
 * User: Arkkhasin    Date: 4/18/08    Time: 12:45a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * The API to clear low power condition is now simply
 * error_ClearFault(FAULT_LOW_POWER)
 *
 * *****************  Version 115  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:44a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 114  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 5:55p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Adapted to new API
 *
 * *****************  Version 113  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 6:03p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Device mode is now a consistent set of handy wrappers around fault bit
 * representation of modes.
 * Removed mode_Mopup() - last version did nothing at all
 * Implemented simplified mode_{Set,Get}Mode() API
 * Moved all bumpless stuff to bumpless.c
 * Removed mode_SetOOSMode() - rely on fault bits in the inew
 * mode_GetSubmode()
 *
 *
 *
 * *****************  Version 112  *****************
 * User: Arkkhasin    Date: 2/26/08    Time: 4:20p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * mode_ClearLowPower does not fix the UI to init node anymore (new bug
 * fix)
 *
 * *****************  Version 111  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 3:15p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Went back from error_ClearLowPower() to mode_ClearLowPower() because of
 * UI notification
 *
 * *****************  Version 110  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 10:09p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Adapted to new sysio interfaces
 *
 * *****************  Version 109  *****************
 * User: Arkkhasin    Date: 1/25/08    Time: 9:23a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Lint
 *
 * *****************  Version 108  *****************
 * User: Arkkhasin    Date: 1/23/08    Time: 6:55p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * 1. Added definitions for TIGHT_SHUTOFF (still need the corresponding
 * fix in control.c) and for mode-related and DO-related and
 * posmon-related things
 * 2. Fault properties generator is no longer in the business of
 * discovering fault categories; a category must be supplied explicitly
 * 3. Fault properties are now clearly separated into storage attributes
 * (FSATTRIB_xxx) and behavior attributes (FATTRIB_yyy).
 * 4. Device mode is no longer stored explicitly: it is derived from the
 * fault bit indicators set.
*/

#ifdef _MSC_VER
#   include <time.h>
#endif
#include "mnwrap.h"
#include "errcodes.h"
#include "devicemode.h"
#include "process.h"
#include "nvram.h"
#include "uipublic.h"
#include "uistartnodes.h"
#include "faultpublic.h"

#include "devfltpriv.h"

#include "oswrap.h"
#include "ctllimits.h" //for SETPOINT_INVALID; move it to a better place

static ModeData_t m_Mode_;

MN_DECLARE_API_DATA(mode_ControlModeMap) //May or may not be used elsewhere (such as okctlmodes.c)
const ctlmode_t mode_ControlModeMap[] = //!< Map of raw control modes to cooked control modes
{
    [CONTROL_INVALID] = CONTROL_IPOUT_LOW, //Don't care: must expire on the first run of mode_GuardMode() !!!
    [CONTROL_MANUAL_POS] = CONTROL_MANUAL_POS,
    [CONTROL_IPOUT_LOW] = CONTROL_IPOUT_LOW,
    [CONTROL_IPOUT_LOW_FAIL] = CONTROL_IPOUT_LOW,
    [CONTROL_IPOUT_HIGH] = CONTROL_IPOUT_HIGH,
    [CONTROL_IPOUT_HIGH_FAIL] = CONTROL_IPOUT_HIGH, //valid for one control cycle; needs to be refreshed
    [CONTROL_IP_DIAGNOSTIC] = CONTROL_IP_DIAGNOSTIC, //AK:TODO ??
    [CONTROL_IPOUT_HIGH_FACTORY] = CONTROL_IPOUT_HIGH,
    [CONTROL_IPOUT_HIGH_PROC] = CONTROL_IPOUT_HIGH,
    [CONTROL_IPOUT_LOW_PROC] = CONTROL_IPOUT_LOW,
    [CONTROL_OFF] = CONTROL_OFF,
};

#if 0 //not used
/*
Shortest "Trip over" detection takes the (longest) cycle task time, + 2-3 more control task
cycles in the worst case, depending on HW delays (1) and timing of relative runs (1 or 2).
This good for all purposes for I/P setting outside a trip, except manual setpoint controlled from local UI:
To get out of trip in manual or setup when no external trip condition exists, we simply need to
change the setpoint. To do it from the local UI where we can only ramp it from -Much, we need the
manual pos control mode to be longer than the UI cycle which is UITIME_IN_MS/(CTRL_TASK_DIVIDER*TICK_MS).
We'll give it a generous fudge to account for jitters in UI invocations.
The timeout for CONTROL_MANUAL_POS is the maximum of the two.
*/
#define TMOUT_CONTROL_PROC (AD_MAX_LONG_CYCLE + 3) //!< common refresh period vs. trip detection
#define TMOUT_SETPOINT_CONTROL MAX(\
    TMOUT_CONTROL_PROC, /*max lifetime of SP control mode in short (control task) cycles */ \
        (u32)((UITIME_IN_MS*1.5)/(CTRL_TASK_DIVIDER*TICK_MS))+1) //!< UI-imposed refresh period vs. trip detection
#endif

static const u32 modetmout[NELEM(mode_ControlModeMap)] = //!< timeouts of raw control modes
{
    [CONTROL_INVALID] = 1, //Do not change: must expire on the first run of mode_GuardMode() !!!
    [CONTROL_MANUAL_POS] = 0, //TMOUT_SETPOINT_CONTROL,
    [CONTROL_IPOUT_LOW] = 0,
    [CONTROL_IPOUT_LOW_FAIL] = 0,
    [CONTROL_IPOUT_HIGH] = 0, //TMOUT_CONTROL_PROC, //Must refresh every time
    [CONTROL_IPOUT_HIGH_FAIL] = 0, //No refresh: device must go failsafe if tripped
    [CONTROL_IP_DIAGNOSTIC] = 0, //TMOUT_CONTROL_PROC,
    [CONTROL_IPOUT_HIGH_FACTORY] = 0, //TMOUT_CONTROL_PROC, //Must refresh every time
    [CONTROL_IPOUT_HIGH_PROC] = 0, //TMOUT_CONTROL_PROC, //TMOUT_SETPOINT_CONTROL,
    [CONTROL_IPOUT_LOW_PROC] = 0,
    [CONTROL_OFF] = 0, //TMOUT_CONTROL_PROC,
};

typedef struct modeguard_t
{
    u32 tmout;
    s32 setpoint;
    ctlmode_t ctlmode;
    u16 CheckWord;
} modeguard_t;

static modeguard_t modeguard;


/** \brief Returns mode data
*/
const void *TypeUnsafe_mode_GetModeData(void *dst)
{
    return STRUCT_GET(&m_Mode_, dst);
}

ErrorCode_t TypeUnsafe_mode_SetModeData(const void *src)
{
    ModeData_t md;
    devmode_t mode;
    if(src == NULL)
    {
        //default init from mode_PreInit() holds, unless replaced by trap processing
    }
    else
    {
        const ModeData_t *pmd = src;
        mode = pmd->tmode;
        mode = mode_RepairStartupMode(mode);
        md.tmode = mode;

        if(mode_GetMode() == MODE_FAILSAFE)
        {
            mode = MODE_FAILSAFE;
        }
        md.mode = mode;
        Struct_Copy(ModeData_t, &m_Mode_, &md);
    }
    return ram2nvramAtomic(NVRAMID_ModeData);
}


static bool_t modereq = false;

/** \brief Delayed write to NVMEM
*/
void mode_Mopup(void)
{
    if(modereq)
    {
        if(ram2nvramAtomic(NVRAMID_ModeData) == ERR_OK)
        {
            modereq = false;
        }
    }
}



/** \brief Returns device mode.
*/
devmode_t mode_GetMode(void)
{
    //do the test periodically in modeguard STRUCT_TEST(ModeData_t, &m_Mode_);
    return m_Mode_.mode;
}

/** \brief A helper that may well belong to the interface layer
\return a bitmap of device submodes (generic conditions inside the mode)
*/
devsubmode_t mode_GetSubmode(void)
{
    devsubmode_t mode = SUBMODE_NORMAL;
    if(error_IsAnyFaultWithAttributes(FATTRIB_LOWPWR))
    {
        mode = SUBMODE_LOWPOWER;
    }
    if(error_IsAnyFaultWithAttributes(FATTRIB_FAILHIGH))
    {
        mode |= SUBMODE_FAILED_HIGH;
    }
#ifdef FAULT_MARGINAL_POWER
    if(error_IsFault(FAULT_MARGINAL_POWER))
    {
        mode |= SUBMODE_MARGINAL_POWER;
    }
#endif //defined FAULT_MARGINAL_POWER
#ifdef FAULT_ESD_TRIP
    if(error_IsFault(FAULT_ESD_TRIP))
    {
        mode |= SUBMODE_ESD_TRIP;
    }
#endif
    return mode;
}


/**
\brief Mandatory initializer of the startup mode.
C environment initialization puts mode to OOS/OOS_NORMAL (a.k.a. setup).
Any faults logged so far can change to failsafe, and that must be preserved
while reading the mode from FRAM.

\param itype - source of initialization (INIT_FROM_NVRAM or INIT_TO_DEFAULT)
*/
void mode_PreInit(void)
{
    //Control mode will be set later (outside) based on initialization results
    modeguard.ctlmode = CONTROL_INVALID; //setpoint is don't care
    STRUCT_CLOSE(modeguard_t, &modeguard);
    //Mode will be replaced during initialization or trap processing
    m_Mode_.mode = MODE_SETUP;
    m_Mode_.tmode = MODE_SETUP;
    STRUCT_CLOSE(ModeData_t, &m_Mode_);
}

ErrorCode_t mode_SetMode(devmode_t dmode)
{
    return mode_SetModeEx(dmode, true);
}

ErrorCode_t mode_SetModeEx(devmode_t dmode, bool_t save)
{
    ErrorCode_t err;
    devmode_t mode = mode_GetMode();
    if(mode == dmode)
    {
        return ERR_OK; //nothing to do
    }

    //AK:TODO: "Perhaps do a better process treatment"
    //For now, either no process must be running or the running process
    //requests the mode change, unless we go failsafe which is always allowed
    if(dmode != MODE_FAILSAFE)
    {
        if(!oswrap_IsContext(TASKID_PROCESS) && process_CheckProcess())
        {
            return ERR_MODE_CANNOT_CHANGE;
        }
    }

    MN_ENTER_CRITICAL();
        /* Mode change is allowed if
            - the new mode is failsafe, or
            - the old mode is not failsafe, or
            - the old mode is failsafe but there are no faults to keep it in failsafe
        */
        //Now allow mode change in low power. This is a big TBD!
        bool_t can_change =
            ((dmode == MODE_FAILSAFE)
             || (mode != MODE_FAILSAFE)
                 || !error_IsAnyFaultWithAttributes(FATTRIB_FAILSAFE|FATTRIB_FAILHIGH));
        if(can_change)
        {
            can_change = mode_TransitionHook(dmode);
        }
        if(can_change)
        {
            //Depending on the running process (or rather its flags - TBD) we may need to check for it or cancel it
#if 0
            if(dmode != MODE_FAILSAFE)
#else
            if(mode_IsPersistent(dmode))
#endif
            {
                storeMemberU8(&m_Mode_, tmode, dmode);
            }
            storeMemberU8(&m_Mode_, mode, dmode);
            ui_setNext(UINODEID_INITNODE); //bring UI in sync with the mode
            err = ERR_OK;
            modereq = save;
        }
        else
        {
            err = ERR_MODE_CANNOT_CHANGE;
        }
    MN_EXIT_CRITICAL();
    return err;
}


/** \brief Clears low power mode and returns to the previously saved (=Target) mode

NOTE: The Target Mode itself is not affected
NOTE: Must be called from within a critical section

*/
void mode_ClearLowPower(void)
{
    ui_setNext(UINODEID_INITNODE);
}


//---------------- new additions to protect control mode ---------------------------

/**
\brief Checks if the requested control mode is compatible with the device mode
\param ctlmode - the requested control mode
\return true if ctlmode is compatible, false otherwise
*/
static bool_t mode_IsCompatible(devmode_t mode, devsubmode_t submode, ctlmode_t ctlmode)
{
    for(u8_least i=0; ctlmodelist[i].count!=0; i++)
    {
        if((mode == ctlmodelist[i].mode) && ((submode & ctlmodelist[i].submode_mask) == ctlmodelist[i].submode))
        {
            const ctlmode_t *pctl = ctlmodelist[i].list;
            for(u8_least ctln=0; ctln<ctlmodelist[i].count; ctln++)
            {
                if(pctl[ctln]==ctlmode)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
\brief Guesses the best default control mode to repair an invalid one with.
By convention, the best control mode is the first in the effective list
of compatible modes
\return the best control mode
*/
static ctlmode_t mode_GuessControlMode(devmode_t mode, devsubmode_t submode)
{
    for(u8_least i=0; ctlmodelist[i].count!=0; i++)
    {
        if((mode == ctlmodelist[i].mode) && ((submode & ctlmodelist[i].submode_mask) == ctlmodelist[i].submode))
        {
            return ctlmodelist[i].list[0];
        }
    }
    return CONTROL_IPOUT_LOW; //last-ditch attempt; should never occur
}

/**
\brief Sets the intended raw control mode requested if compatible with the device mode.

The actual control mode is not set until the next run of mode_GuardControlMode
This scheme is necessary in order to work around places where device mode test
cannot be made atomic with control mode request.
If the control mode is incompatible with the device mode, it will not be set.

Note that mode_SetControlMode() is a void function; if the app needs
to check to see if it went through (and usually it doesn't),
call mode_Get{Intended or Effective}ControlMode() to get the actual control mode.
*/
void mode_SetControlMode(ctlmode_t ctlmode, s32 setpoint)
{
    MN_ENTER_CRITICAL();
        if(mode_IsCompatible(mode_GetEffectiveMode(mode_GetMode()), mode_GetSubmode(), ctlmode))
        {
            //Actual control mode is not set until the next mode_GuardControlMode -- control_SetControlMode(ctlmode, setpoint);
            modeguard.ctlmode = ctlmode;
            modeguard.tmout = modetmout[ctlmode];

            if(setpoint != SETPOINT_INVALID)
            {
                modeguard.setpoint = setpoint;
            }
            STRUCT_CLOSE(modeguard_t, &modeguard);
        }
    MN_EXIT_CRITICAL();
}

//458, unprotected read access to m_Mode_ - Lint (9.00h) mistake - or prove otherwise :)
//Suppressed in codebase.lnt
/**
\brief Checks the validity and fixes the control mode structure (modeguard) as necessary.
Propagates the (cooked) control mode to the Control Module.
*/
void mode_GuardControlMode(void)
{
    ctlmode_t ctlmode;

    Struct_Test(modeguard_t, &modeguard);
    Struct_Test(ModeData_t, &m_Mode_);


    MN_ENTER_CRITICAL();
        bool_t need_guess;
        ctlmode = modeguard.ctlmode;

        devsubmode_t submode = mode_GetSubmode();
        devmode_t mode =mode_GetMode();
        mode = mode_GetEffectiveMode(mode);
        need_guess = !mode_IsCompatible(mode, submode, ctlmode);
        if(!need_guess)
        {
            if(modeguard.tmout > 0)
            {
                modeguard.tmout--;
                if(modeguard.tmout == 0)
                {
                    need_guess = true;
                }
            }
        }

        //This absolutely comes first; may delay effective trip command once
        if(need_guess)
        {
            ctlmode = mode_GuessControlMode(mode, submode);
            if(ctlmode != modeguard.ctlmode)
            {
                //refresh didn't work out; we have control mode change
                modeguard.setpoint = mode_GuessSetpoint(m_Mode_.mode, ctlmode);
            }
            modeguard.tmout = modetmout[ctlmode];
        }

        //mode_GuardSetpoint may insist on a different control mode so save the known one first
        modeguard.ctlmode = ctlmode;

        //modeguard.setpoint = mode_GuardSetpoint(mode_GetEffectiveMode(m_Mode_.mode), ctlmode, modeguard.setpoint);
        modeguard.setpoint = mode_GuardSetpoint(m_Mode_.mode, ctlmode, modeguard.setpoint);
        STRUCT_CLOSE(modeguard_t, &modeguard);
    MN_EXIT_CRITICAL();

}

/** \brief Reads the intended raw control mode and setpoint
\param[out] sp - a pointer to the receiver of the setpoint (if NULL, will not be filled)
\return the intended raw control mode
*/
ctlmode_t mode_GetIntendedControlMode(s32 *sp)
{
    ctlmode_t ctlmode;
    MN_ENTER_CRITICAL();
        if(sp != NULL)
        {
            *sp = modeguard.setpoint;
        }
        ctlmode = modeguard.ctlmode;
    MN_EXIT_CRITICAL();
    return ctlmode;
}

/** \brief Reads the intended cooked control mode and setpoint
\param[out] sp - a pointer to the receiver of the setpoint (cannot be a NULL)
\return the intended cooked control mode
*/
ctlmode_t mode_GetEffectiveControlMode(s32 *sp)
{
    ctlmode_t ctlmode = mode_GetIntendedControlMode(sp);

    ctlmode = ctllim_ConditionControlMode(ctlmode, sp);
    return mode_ControlModeMap[ctlmode];
}

/* This line marks the end of the source */
