/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ctllimits.c
    \brief Implementation of Limits for Position Control

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/control/ctllimits.c $
    $Date: 1/06/12 4:18p $
    $Revision: 11 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* $History: ctllimits.c $
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 4:18p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * TFS:8577 positionsp_range added
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:41p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 2/04/11    Time: 12:41a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:same - recaptured control mode conditions missed in the previous
 * version
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 2/03/11    Time: 7:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:5518 TFS:5160 Setpoint used for tight sutoff is range-limited but
 * not rate-limited; Setpoint used for control is constrained by range
 * values always
 *
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 10/22/10   Time: 11:07a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Improved setpoint step smoothness. This completes TFS:4206
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 10/22/10   Time: 2:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:4206 fix - Step 2: (a, also TFS:2514) checksums in rate limits
 * stuff which moved to ctllimits.c (b) removed /some/ compiled-out code
 * (c, FOB TFS:2616) rate limits can be reset to any setpoint
 *
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 8/26/10    Time: 11:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Lint: Bug 3957; tested by comparing the listing before and after
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 5:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 *         - Add OLD_NVRAM wrappers
 *         - FAULT_NVM_CHECKSUM --> FAULT_NVM_CHECKSUM0
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 1:43a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Lint (Unused headers)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 1:31a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Partial back-port from the trunk
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 2/11/10    Time: 1:54a
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Light linting
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 2/10/10    Time: 7:01p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Refactoring bugs found by Lint and fixed
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 4:36p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * A missing const qualifier added
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 1/04/10    Time: 12:49p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Added ctllim_Reinit API
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 12/14/09   Time: 7:18p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Setpoint rate limits moved from PIDData_t (and ExtPIDData_t) to
 * CtlLimits_t
 * New ctllim_ConditionControlMode() to take care of setpoint range and
 * rate limits and of tight shutoff
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:44a
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:53p
 * Updated in $/MNCB/Dev/FIRMWARE/tasks/control
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 * Module initilaization table types - ModuleDataItem_t and ModuleData_t
 * removed
 * ram2nvram[Atomic] do not take a pointer to the buffer anymore, but now
 * return an error code
 * Also: def_CtlLimits  is now static
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/17/08    Time: 5:32p
 * Updated in $/MNCB/Dev/FIRMWARE/tasks/control
 * Second step toward uniform support of engineering units:
 * Valid range limits for any parameter are owned by a corresponding
 * module and exposed as an array of 2 elements (low and high limits)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/12/08    Time: 11:48a
 * Created in $/MNCB/Dev/FIRMWARE/tasks
 * Control limits (setpoint and tight shutoff) moved from
 * ConfigurationData_t to CtlLimits_t.
 * ESD doesn't have those limits nad doesn't know about CtlLimits_t.
 * New source - ctllimits.{c,h} - instead of configure.c now takes care of
 * keeping track of the limits
*/
#include "mnwrap.h"
#include "mnassert.h"
#include "errcodes.h"
#include "param.h" //pos_least_t should move to the position header
#include "crc.h"
#include "nvram.h"
#include "ctllimits.h"
#include "faultpublic.h"
#include "control.h"
#include "ctlpriv.h"
#if 0
#include "process.h"
#include "devicemode.h"
#endif
#include "oswrap.h"
#include "procpriv.h"

//------------------ Tight shut-off -----------------
//TFS:9578 Add limitats for Tight Cutoff
#define TIGHT_SHUTOFF_LOW_LIMIT -1.0F //! lower tight shutoff limit in % of range
#define TIGHT_SHUTOFF_HIGH_LIMIT 20.0F //! upper tight shutoff limit in % of range
#define TIGHT_CUTOFF_LOW_LIMIT 80.0F //! lower tight cutoff limit in % of range
#define TIGHT_CUTOFF_HIGH_LIMIT 101.0F //! upper tight cutoff limit in % of range
//----------------- setpoint (a.k.a. position) limits -------------------
#define LOW_POSITION_LIMIT_LOW -10.0F
#define LOW_POSITION_LIMIT_HIGH 50.0F
#define UPPER_POSITION_LIMIT_LOW 10.0F
#define UPPER_POSITION_LIMIT_HIGH 110.0F

const pos_t tso_limits[Xends] =
{
    INT_PERCENT_OF_RANGE(TIGHT_SHUTOFF_LOW_LIMIT),
    INT_PERCENT_OF_RANGE(TIGHT_SHUTOFF_HIGH_LIMIT)
};

const pos_t tco_limits[Xends] =
{
    INT_PERCENT_OF_RANGE(TIGHT_CUTOFF_LOW_LIMIT),
    INT_PERCENT_OF_RANGE(TIGHT_CUTOFF_HIGH_LIMIT)
};

//range of manual position setpoint
const pos_t positionsp_range[Xends] =
{
    [Xlow] = POS_SETPOINT_LOW_INT,
    [Xhi] = POS_SETPOINT_HIGH_INT,
};

//ranges of position setpoint limits
const pos_t spposlimits_range[Xends][Xends] =
{
    [Xlow] = //range of lower limit
    {
        [Xlow] = INT_PERCENT_OF_RANGE(LOW_POSITION_LIMIT_LOW),
        [Xhi] = INT_PERCENT_OF_RANGE(LOW_POSITION_LIMIT_HIGH),
    },
    [Xhi] =  //range of upper limit
    {
        [Xlow] = INT_PERCENT_OF_RANGE(UPPER_POSITION_LIMIT_LOW),
        [Xhi] = INT_PERCENT_OF_RANGE(UPPER_POSITION_LIMIT_HIGH),
    },
};

static CtlLimits_t m_CtlLimits;
static CtlLimits_t m_CtlLimits_backup;

//--not valid for FF-- #define  TIGHT_SHUTOFF_DEF (0.0F)
#define  TIGHT_SHUTOFF_DEF (2.0F)  //default for FF project
//--not valid for FF-- #define  TIGHT_CUTOFF_DEF (100.0F)
#define  TIGHT_CUTOFF_DEF (98.0F)  //default for FF project
#define  LOW_POSIITION_LIMIT_DEF (0.0F)
#define  UPPER_POSITION_LIMIT_DEF (100.0F)

//Setpoint Rate limits in %/s
//changed low limit from 0.8 to 0.5 === 1 count per control cycle - LS 4/8/13
#define SP_RATE_LIMIT_LOW 0.5 //reverse engineered from 3.1.x 2 counts/ctl cycle = 0.81380208333333333333333333333333
#define SP_RATE_LIMIT_HIGH 100.1 //reverse engineered from 3.1.x 246 counts/ctl cycle = 100.09765625
/*Other than being artifacts of reverse engineering, the limits are arbitrary and can be changed
to requirements
*/

static const CtlLimits_t def_CtlLimits =
{
    .Protected = 0,
    .EnableTightShutoff = {0,0}, //turned off by default
    .TightShutoff = {INT_PERCENT_OF_RANGE(TIGHT_SHUTOFF_DEF), INT_PERCENT_OF_RANGE(TIGHT_CUTOFF_DEF)},
    .EnableSetpointLimit = {0, 0}, //no setpoint limits
    .SetpointLimit =
    {
        INT_PERCENT_OF_RANGE(LOW_POSIITION_LIMIT_DEF),
        INT_PERCENT_OF_RANGE(UPPER_POSITION_LIMIT_DEF),
    },
    .EnableSetpointRateLimit = {0, 0}, //no setpoint rate limits
    .SetpointRateLimit = //Dummy setpoint rate limits
    {
        INT_PERCENT_OF_RANGE(SP_RATE_LIMIT_HIGH),
        INT_PERCENT_OF_RANGE(SP_RATE_LIMIT_HIGH),
    },
    .CheckWord = 0, //don't care
};

const pos_t spratelim_range[Xends] =
{
    INT_PERCENT_OF_RANGE(SP_RATE_LIMIT_LOW),
    INT_PERCENT_OF_RANGE(SP_RATE_LIMIT_HIGH),
};

/** \brief Tests the validity of a new set of control limits
\param lims - a pointer to limits to test
\return an error code; ERR_OK if no error
*/
static ErrorCode_t control_TestCtlLimits(const CtlLimits_t *lims)
{
    pos_least_t tso;
    pos_least_t lolim;
    pos_least_t hilim;

    /** validate the input */

    //New (TFS:13882) Range limit and Tight shutoff may not be enabled on the same end
    if( (lims->EnableTightShutoff[Xlow] != 0) && (lims->EnableSetpointLimit[Xlow] != 0) )
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }
    if( (lims->EnableTightShutoff[Xhi] != 0) && (lims->EnableSetpointLimit[Xhi] != 0) )
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    tso = lims->TightShutoff[Xlow];
    if( (tso < tso_limits[Xlow]) || (tso > tso_limits[Xhi]))
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    tso = lims->TightShutoff[Xhi];
    if( (tso < tco_limits[Xlow]) || (tso > tco_limits[Xhi]))
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    lolim = lims->SetpointLimit[Xlow];
    hilim= lims->SetpointLimit[Xhi];

    if( (lolim < spposlimits_range[Xlow][Xlow]) || (lolim > spposlimits_range[Xlow][Xhi]) ||
        (hilim < spposlimits_range[Xhi][Xlow]) || (hilim > spposlimits_range[Xhi][Xhi]) ||
        (hilim <= lolim) )
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    pos_least_t ratelim;
    ratelim = lims->SetpointRateLimit[Xlow];
    if((ratelim < spratelim_range[Xlow]) || (ratelim > spratelim_range[Xhi]))
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }
    ratelim = lims->SetpointRateLimit[Xhi];
    if((ratelim < spratelim_range[Xlow]) || (ratelim > spratelim_range[Xhi]))
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    return ERR_OK;
}

/** \brief Atomically sets a new set of limits in RAM only.
Use it for temporary disabling/re-enabling the limits, ramping the setpoint etc.
\param lims - a pointer to limits to set
\return an error code; ERR_OK if no error
*/
ErrorCode_t control_SetTentativeLimits(const CtlLimits_t *lims)
{
    ErrorCode_t err = control_TestCtlLimits(lims);
    if(err == ERR_OK)
    {
        //dumb down the resolution as in 3.1.x
        Struct_Copy(CtlLimits_t, &m_CtlLimits, lims);
    }
    return err;
}

/** \brief Atomically sets a new set of limits in RAM and commits
them to NVMEM.
Use it for permanent change of the limits configuration
\param lims - a pointer to limits to set
\return an error code; ERR_OK if no error
*/
ErrorCode_t TypeUnsafe_control_SetLimits(const void *src)
{
    CtlLimits_t temp;
    if(src == NULL)
    {
        temp = def_CtlLimits;
    }
    else
    {
        temp = *(const CtlLimits_t *)src;
    }
    ErrorCode_t err = control_SetTentativeLimits(&temp);
    if(err == ERR_OK)
    {
        Struct_Copy(CtlLimits_t, &m_CtlLimits_backup, &temp); //create a copy
#ifdef OLD_NVRAM
        MN_FRAM_ACQUIRE();
            ram2nvram(&temp, NVRAMID_CtlLimits);
        MN_FRAM_RELEASE();
#else
        err = ram2nvramAtomic(NVRAMID_CtlLimits);
#endif //OLD_NVRAM
    }
    return err;
}

/** \brief Atomically fills and returns control limits in effect
\param[out] dst - a pointer to limits copy to fill
\return A pointer to persistently existing object
*/
const void *TypeUnsafe_control_GetLimits(void *dst)
{
    return STRUCT_TESTGET(&m_CtlLimits, dst);
}

/** \brief Tests if control limits can be disabled
\param lim - a pointer to limits
\return the test result
*/
static ProtectTest_t  control_TestProtectLimits_Internal(const CtlLimits_t *lim)
{
    ProtectTest_t ret;

    bool_t limits_set =
        (lim->EnableSetpointLimit[Xlow]!=0) || (lim->EnableSetpointLimit[Xhi]!=0) ||
        (lim->EnableTightShutoff[Xlow] != 0) || (lim->EnableTightShutoff[Xhi] != 0) ||
        (lim->EnableSetpointRateLimit[Xlow]!=0) || (lim->EnableSetpointRateLimit[Xhi]!=0);

    ret = NO_LIMITS_SET;
    if( limits_set )
    {
        //limits are set - we must honor them and fail or turn them off
        if(lim->Protected != 0)
        {
            ret =  PROTECTED_LIMITS;
        }
        else
        {
            ret = UNPROTECTED_LIMITS;
        }
    }
    return ret;
}

/** \brief Tests to see if protected limits are set

  used during diagnostics to
        decide if to continue or to disable limits

  \return ProtectTest_t NO_LIMITS_SET - tight shutoff and limit stops are not set,
                    UNPROTECTED_LIMITS - limits are set but allowed to be turned off,
                    PROTECTED_LIMITS - limits are set and should not be turned off
*/
ProtectTest_t  control_TestProtectLimits(void)
{
    return control_TestProtectLimits_Internal(&m_CtlLimits);
}

/** \brief Used by diagnostics and autotune to disable limits in ram (but not FRAM)

  Used by diagnostics and autotune to disable limits in ram (but not FRAM)
  \param[in] bAllowTightShutoff - leave tight shutoff and allow even if limits protected
  \return non-zero if protected and cannot be disabled
*/
ErrorCode_t cnfg_DisableLimits(void)
{
    ProtectTest_t Test;

    Test = control_TestProtectLimits_Internal(&m_CtlLimits);
    if( Test == PROTECTED_LIMITS)
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_CNFG_PROTECTED_LIMITS;
    }

    CtlLimits_t temp = m_CtlLimits;
    temp.EnableTightShutoff[Xlow] = 0;
    temp.EnableTightShutoff[Xhi] = 0;
    temp.EnableSetpointLimit[Xlow] = 0;
    temp.EnableSetpointLimit[Xhi] = 0;
    temp.EnableSetpointRateLimit[Xlow] = 0;
    temp.EnableSetpointRateLimit[Xhi] = 0;

    return control_SetTentativeLimits(&temp);
#if 0 //orphaned piece - see bug 1739 in "ESD Testing BugCollector.mnb"
    //clear the latch trips bit in case this is an ESD with trips latched
        util_ClearBit(LATCH_TRIPS, m_ConfigurationData.ConfigFlags, CONFIGURATION_BYTES);
#endif
}


/** \brief Used by diagnostics and autotune to set limits in ram back to what is in fram

  Used by diagnostics and autotune to set limits in ram back to what is in fram
  \param[in] none
  \return none
*/
void cnfg_ReEnableLimits(void)
{
    Struct_Test(CtlLimits_t, &m_CtlLimits_backup);
    Struct_Copy(CtlLimits_t, &m_CtlLimits, &m_CtlLimits_backup);
}

/** \brief A generic wrapper for a process which wants to run with control limits disabled.
\param procf - a process (function) to execute
\param bAllowTightShutoff - intend not to disable tight shutoff option
\param[out] procdetails - a generic parameter passed to the process
\return process completion code
*/
procresult_t control_ProcWithNoLimits(procresult_t (*procf)(s16 *procdetails), s16 *procdetails)
{
    procresult_t ret;
    if(procf == NULL)
    {
        ret = PROCRESULT_FAILED;
        *procdetails = 0;
    }
    else
    {
        ErrorCode_t err = cnfg_DisableLimits();
        if(err != ERR_OK)
        {
            *procdetails = 0;
            ret = PROCRESULT_CANCELED;
        }
        else
        {
            s32 sp;
            ctlmode_t ctlmode = mode_GetIntendedControlMode(&sp);

            //execute the process
            ret = procf(procdetails);

            //reset the position and rate limit base as necessary
            u8 flags;
            flags = process_GetProcessTermFlags(ret);
            if((flags & PROCTERMFLAG_CTLMODE) != 0)
            {
                mode_SetControlMode(ctlmode, sp);
            }
            else
            {
                //get current setpoint
                (void)mode_GetIntendedControlMode(&sp);
            }

            //For rate limits, reset the base setpoint
            ctllim_Reinit(sp);

            //Now, restore control limits
            cnfg_ReEnableLimits();
        }
    }
    return ret;
}


#define SP_BOOST(sp) ((sp)*SP_RATE_STEP_PER_CTL_CYCLE)
#define SP_REDUCE(sp) ((sp)/SP_RATE_STEP_PER_CTL_CYCLE)


typedef struct CtlLimState_t
{
    pos_least_t setpoint_boosted; //!< boosted representation of rate-limited base
    pos_t setpoint_bounded; //!< range- (but not rate-) limited SP
    u16 CheckWord;
} CtlLimState_t;


CONST_ASSERT(SETPOINT_INVALID<MIN_EFFECTIVE_SETPOINT);


static CtlLimState_t CtlLimState =
{
    .setpoint_boosted = SETPOINT_INVALID,
    .CheckWord = (u16)((u32)SETPOINT_INVALID + ((u32)SETPOINT_INVALID >> 16)+CRC_SEED), //a hack relying on a particular checksum
};


static void ctllim_WriteBoostedSetpoint(pos_least_t sp, pos_least_t sp_bounded)
{
    MN_ENTER_CRITICAL();
        storeMemberInt(&CtlLimState, setpoint_boosted, sp);
        storeMemberInt(&CtlLimState, setpoint_bounded, sp_bounded);
    MN_EXIT_CRITICAL();
}


/** \brief A simple helper to return user-intended range-limited setpoint.
Good for tight shutoff decisions.
\return range-limited but not rate-limited setpoint
*/
pos_t ctllim_GetRangeBoundedSetpoint(void)
{
    return CtlLimState.setpoint_bounded;
}

//For really coarse calculations as per 3.1.x; the inscale32()-based will come later
#define SP_RATE_STEP_PER_CTL_CYCLE ((pos_t)(ONE_SECOND/CTRL_CALCULATION_INTERVAL_MS))

/** \brief Resets the state of rate limits
Must be very fast - may be called from a large(er) critical section
\param sp - new base setpoint
*/
void ctllim_Reinit(pos_least_t sp)
{
    if(sp != SETPOINT_INVALID)
    {
        s32 spb = SP_BOOST(sp);
        MN_ASSERT(SP_REDUCE(spb)==sp); //ensure no overflow
        sp = spb;
    }
    ctllim_WriteBoostedSetpoint(sp, CtlLimState.setpoint_bounded);
}

/** \brief Computes control mode and setpoint for the control algorithm

Convert the setpoint from user domain (adjusted) to
control domain (unadjusted) and apply range and rate limits
\param[out] ctlmode - control mode (before being cooked for control, if different)
\param[in][out] pSetpoint - a pointer to recipient setpoint
\return control mode
*/
ctlmode_t ctllim_ConditionControlMode(ctlmode_t ctlmode, s32 *pSetpoint)
{
    s32 sp = *pSetpoint;
    //ctlmode = ctllim_CheckTightCtl(ctlmode, sp);

    //apply control limits
    const CtlLimits_t *lims = &m_CtlLimits;

    s32 lrv;
    if(lims->EnableSetpointLimit[Xlow] == 0)
    {
        //no user limit
        lrv = MIN_EFFECTIVE_SETPOINT;
    }
    else
    {
        lrv = lims->SetpointLimit[Xlow];
    }

    s32 urv;
    if(lims->EnableSetpointLimit[Xhi] == 0)
    {
        //no user limit
        urv = MAX_EFFECTIVE_SETPOINT;
    }
    else
    {
        urv = lims->SetpointLimit[Xhi];
    }

    pos_least_t sp_bounded = CLAMP(sp, lrv, urv);

    const CtlLimState_t *p = STRUCT_TESTGET(&CtlLimState, NULL);

    if(ctlmode == CONTROL_MANUAL_POS)
    {
        pos_least_t sp_prev = p->setpoint_boosted;
        if(sp_prev == SETPOINT_INVALID)
        {
            //start with a guess = the valve position (in scaled domain)
            //for the trunk: sp_prev = mode_GuessSetpoint(mode_GetMode(), ctlmode);
            sp_prev = control_GetPosition();
            sp_prev = SP_BOOST(sp_prev);
        }

        /* prev_sp needs to be clamped to the range limits so that there is no
        "crawl time" into the range
        */
        sp_prev = CLAMP(sp_prev, SP_BOOST(lrv), SP_BOOST(urv));

        /* In no event can sp_prev be within tight shutoff zone
        */
        if(lims->EnableTightShutoff[Xlow] != 0)
        {
            sp_prev = MAX(sp_prev, SP_BOOST(lims->TightShutoff[Xlow]));
        }
        if(lims->EnableTightShutoff[Xhi] != 0)
        {
            sp_prev = MIN(sp_prev, SP_BOOST(lims->TightShutoff[Xhi]));
        }

        sp = SP_BOOST(sp);

        //limit rate change of setpoint if rate limits are set
        //open end
        if(lims->EnableSetpointRateLimit[Xhi] != 0)
        {
            sp = MIN(sp, sp_prev + lims->SetpointRateLimit[Xhi]);
        }
        //closed end
        if(lims->EnableSetpointRateLimit[Xlow] != 0)
        {
            sp = MAX(sp, sp_prev - lims->SetpointRateLimit[Xlow]);
        }

        s32 rsp = SP_REDUCE(sp);

        //Now we can un-adjust sp
        //N/A in this codebase sp = pos_UnAdjust(sp);

        //limit setpoint to allowable range for control
        rsp = CLAMP(rsp, lrv, urv);

        *pSetpoint = rsp;
    }
    else
    {
        //reset control limits reference
        sp = SETPOINT_INVALID;
    }

    ctllim_WriteBoostedSetpoint(sp, sp_bounded);

    return ctlmode;
}

//---------------- old-style NVMEM interface -------------------
#ifdef OLD_NVRAM
void  ctllim_SaveControlLimits(void)
{
    MN_FRAM_ACQUIRE();
        ram2nvram(&m_CtlLimits, NVRAMID_CtlLimits);
    MN_FRAM_RELEASE();
}

void  ctllim_InitControlLimits(InitType_t Type)
{
	
    MN_ASSERT(!oswrap_IsOSRunning()); //and no FRAM mutex

    CtlLimits_t temp;
    const CtlLimits_t *lims;
    if(Type == INIT_FROM_NVRAM)
    {
        nvram2ram(&temp, NVRAMID_CtlLimits);
        lims = &temp;
    }
    else
    {
        lims = &def_CtlLimits;
    }
    ErrorCode_t err = control_SetTentativeLimits(lims);
    if(Type == INIT_FROM_NVRAM)
    {
        if(err != ERR_OK)
        {
            error_SetFault(FAULT_NVM_CHECKSUM0);
        }
    }
    else
    {
        MN_ASSERT(err == ERR_OK);
    }
}
#endif //OLD_NVRAM

/* This line marks the end of the source */

