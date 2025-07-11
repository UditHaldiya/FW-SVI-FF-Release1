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

    \ingroup control
*/

#include "mnwrap.h"
#include "errcodes.h"
#include "devicemode.h"
#include "devfltpriv.h"
#include "position.h"
#include "nvram.h"
#include "timer.h"
#include "digitalsp.h"
#include "ctllimits.h"
#include "faultpublic.h"
#include "ipcdefs.h"
#include "mnassert.h"
#include "oswrap.h"
#include "timedef.h"
#include "poscharact.h"
#include "control.h"
#ifdef AK //define AK to continue work on accounting for warm reset
#include "reset.h"
#endif

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
    {MODE_OPERATE, SUBMODE_NORMAL, SUBMODE_NORMAL, LENLIST(NormalControlModes)},
    {MODE_MANUAL, SUBMODE_NORMAL, SUBMODE_NORMAL, LENLIST(ManualControlModes)},
    {MODE_SETUP, SUBMODE_NORMAL, SUBMODE_NORMAL, LENLIST(OOSNormalControlModes)},
    {MODE_FAILSAFE, SUBMODE_NORMAL, SUBMODE_NORMAL, LENLIST(OOSFailsafeControlModes)},
    //not needed - see previous SETUP entry {MODE_SETUP, SUBMODE_LOWPOWER, SUBMODE_LOWPOWER, LENLIST(OOSFailsafeControlModes)},
    CTLMODELIST_END,
};

const s32 digitalsp_range[Xends] =
{
    [Xlow] = INT_PERCENT_OF_RANGE(-1000.0),
    [Xhi] = INT_PERCENT_OF_RANGE(1000.0)
};

#define MAX_SHED_TIME TICKS_FROM_SHEDTIME_SEC(2.0E6) //23.(148) days; that's about the limit of FP conv. capability
CONST_ASSERT(MAX_SHED_TIME < INT32_MAX);

static const DigitalSpConf_t DigitalSpConf_Default =
{
    .FixedSetpoint = INT_PERCENT_OF_RANGE(-20.0), //stay closed
    .ShedTime = MAX_SHED_TIME,
    .InitTime = MAX_SHED_TIME,
    .IsTargetToManual = true, // On SP loss go to manual
    .sp_option = SSO_current_position,
    .CheckWord = 0, //don't care
};

const s32 DigitalSP_ShedTime_range[Xends] =
{
    [Xlow] = TICKS_FROM_SHEDTIME_SEC(0.1),
    [Xhi] = MAX_SHED_TIME,
};

static DigitalSpConf_t DigitalSpConf;
static digitalsp_t digitalsp;

//Required function: doc in header
bool_t mode_TransitionHook(devmode_t newmode)
{
    bool_t can_change = true;
    if(newmode == MODE_OPERATE)
    {
        if(!digitalsp.isValid)
        {
            //if(!DigitalSpConf.IsTargetToManual)
            {
                can_change = false;
            }
        }
    }
    if(can_change)
    {
        //Cancel any stateful things here
        if((newmode & (MODE_MANUAL|MODE_SETUP)) != 0)
        {
            MN_ENTER_CRITICAL();
                //u8 xmode = digsp_GetExternalMode();
                storeMemberInt(&digitalsp, xmode, IPC_MODE_LOVERRIDE);
                /* EXPLANATION:
                This is not a good xmode but we don't care - all we want
                is to get out of OOS internally but not monitor the setpoint arrival.
                */
            MN_EXIT_CRITICAL();
        }
    }
    return can_change;
}


const DigitalSpConf_t *digsp_GetConf(DigitalSpConf_t *dst)
{
    return STRUCT_TESTGET(&DigitalSpConf, dst);
}

ErrorCode_t digsp_SetConf(const DigitalSpConf_t *src)
{
    if(src==NULL)
    {
        src = &DigitalSpConf_Default;
    }
    //Add input checking
    if((src->FixedSetpoint < digitalsp_range[Xlow]) || (src->FixedSetpoint > digitalsp_range[Xhi]))
    {
        return ERR_INVALID_PARAMETER;
    }

    //Casts OK since range limits are positive
    if((src->ShedTime < (tick_t)DigitalSP_ShedTime_range[Xlow]) || (src->ShedTime > (tick_t)DigitalSP_ShedTime_range[Xhi]))
    {
        return ERR_INVALID_PARAMETER;
    }
    if((src->InitTime < (tick_t)DigitalSP_ShedTime_range[Xlow]) || (src->InitTime > (tick_t)DigitalSP_ShedTime_range[Xhi]))
    {
        return ERR_INVALID_PARAMETER;
    }

    if((src->sp_option >= SSO_last_setpoint))
    {
        return ERR_INVALID_PARAMETER;
    }

    Struct_Copy(DigitalSpConf_t, &DigitalSpConf, src);
    return ram2nvramAtomic(NVRAMID_DigitalSpConf);
}

static bool_t digsp_req = false; //!< mopup request flag

#define SP_RATE_FUDGE 0 //As Vlad wanted.

/* \brief returns an error code indicating limits
\param [in][out] p_sp - a pointer to original setpoint replaced by clamped setpoint
\return an error code
*/
static ErrorCode_t digsp_ClampSetpoint(s32 *p_sp)
{
    ErrorCode_t err = ERR_OK;
    //apply control limits
    const CtlLimits_t *lims = control_GetLimits(NULL);

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

    s32 sp = CLAMP(*p_sp, lrv, urv);

    if(sp == *p_sp)
    {
        //No clamping: sp within range; check rate limits
        s32 worksp;
        control_GetControlMode(NULL, &worksp);
        //Open side
        if((lims->EnableSetpointRateLimit[Xhi] != 0) && (sp > (worksp + SP_RATE_FUDGE)))
        {
            err = ERR_UPPERLIM;
        }
        //Close side
        if((lims->EnableSetpointRateLimit[Xlow] != 0) && (sp < (worksp - SP_RATE_FUDGE)))
        {
            err = ERR_LOWERLIM;
        }
    }
    else
    {
        *p_sp = sp;
        if(sp == urv)
        {
            err = ERR_UPPERLIM;
        }
        else
        {
            //sp == lrv
            err = ERR_LOWERLIM;
        }
    }

    return err;
}

/** \brief Sets target TB mode and digital setpoint and captures its timestamp.
Saving to NVMEM defered to mopup
\param xmode - external TB target mode; ignored if IPC_MODE_NO_CHANGE
\param sp - digital setpoint (ignored if SETPOINT_INVALID)
\return an error code (allowed actions are still performed)
*/
ErrorCode_t digsp_SetDigitalSetpointEx(u8 xmode, s32 sp)
{
    ErrorCode_t err = ERR_OK;
    digitalsp_t digsp;
    (void)digsp_GetData(&digsp);

    if(sp == SETPOINT_INVALID)
    {
        //Do not use the setpoint
    }
    else
    {
        sp = poscharact_Direct(sp);
        err = digsp_ClampSetpoint(&sp);

        digsp_req = (mn_abs(sp - digsp.setpoint) > INT_PERCENT_OF_RANGE(0.5));
        digsp.setpoint = sp;
        digsp.isValid = true;
        error_ClearFault_External(FAULT_SETPOINT_TIMEOUT);
        digsp.timestamp = bios_GetTimer0Ticker();
    }

    if(xmode != IPC_MODE_NO_CHANGE)
    {
        if( (xmode & (xmode - 1)) == 0) //Ritchie trick: test for a single bit set
        {
            //A wart: do not accept AUTO if find stops failed
            //A wart: do not accept AUTO if we don't have setpoint
            bool_t accept = true;
            if(xmode == IPC_MODE_AUTO)
            {
                if(!digsp.isValid || error_IsFault(FAULT_FIND_STOPS_FAILED))
                {
                    accept = false;
                }
            }
            if(accept)
            {
                if(digsp.xmode != xmode)
                {
                    //mode change requested - handle the running process, if any
                    ProcId_t proc = process_GetProcId();
                    switch(proc)
                    {
                        case PROC_NONE:
                            //process is not running: OK
                            digsp_req = true;
                            break;
                        case PROC_CANCEL_PROC:
                            //process is being canceled: don't accept mode
                            accept = false;
                            break;
                        default:
                            //a process is running - cancel it
                            process_CancelProcess();
                            accept = false;
                            break;
                    }
                }
            }
            if(accept)
            {
                digsp.xmode = xmode;
            }
        }
        else
        {
            //leave mode alone but raise an error
            err = ERR_INVALID_PARAMETER;
        }
    }

    Struct_Copy(digitalsp_t, &digitalsp, &digsp);
    digsp_req = true;
    return err;
}

/** \brief Returns actual transducer block mode, perhaps OR'ed with a request to set it as target mode
\return actual TB mode OR'ed with request to set target mode
*/
u8 digsp_GetExternalMode(void)
{
    u8 xmode;
    switch(mode_GetMode())
    {
        case MODE_SETUP:
        case MODE_MANUAL:
            xmode = IPC_MODE_LOVERRIDE;
            break;
        case MODE_OPERATE:
            xmode = digsp_GetData(NULL)->xmode;
            if((xmode == IPC_MODE_AUTO) && error_IsFault(FAULT_SETPOINT_TIMEOUT))
            {
                xmode = IPC_MODE_MANUAL;
            }
            break;
        case MODE_FAILSAFE:
        default://?
            xmode = IPC_MODE_OOS;
            break;
    }
    if(error_IsAnyFaultWithAttributes(FATTRIB_FAILSAFE))
    {
        xmode = IPC_MODE_OOS;
    }
    return xmode;
}

/** \brief Returns effective device mode, backward-compatible
\param mode - local device mode
\return effective device mode
*/
devmode_t mode_GetEffectiveMode(devmode_t mode)
{
    u8 xmode = digsp_GetData(NULL)->xmode;
    if(xmode == IPC_MODE_OOS)
    {
        mode = MODE_FAILSAFE;
    }
    else
    {
        if((mode == MODE_OPERATE) && (xmode != IPC_MODE_AUTO))
        {
            /* EXPLANATION:
            xmode, since
                * not IPC_MODE_OOS nor IPC_MODE_AUTO, can conceivably be
                * IPC_MODE_LOVERRIDE or IPC_MODE_MANUAL.
            Of them,
                * IPC_MODE_MANUAL is requested by FF and is good while the setpoint is coming
                * IPC_MODE_LOVERRIDE is set when transitioning to Setup locally with lost FF setpoint
            Both cases look like MODE_SETUP locally
            */
            mode = MODE_SETUP;
        }
    }
    return mode;
}

/** \brief A helper to invalidate digital setpoint
*/
static void digsp_Invalidate(const DigitalSpConf_t *conf)
{
    MN_ENTER_CRITICAL();

        s32 sp;
        if(conf->sp_option == SSO_fixed_setpoint)
        {
            sp = conf->FixedSetpoint;
        }
        else if(conf->sp_option == SSO_last_setpoint)
        {
            sp = digsp_GetData(NULL)->setpoint;
        }
        else
        {
            //SSO_current_position
            sp = vpos_GetScaledPosition();
        }
        storeMemberInt(&digitalsp, setpoint, sp);
    MN_EXIT_CRITICAL();
}


/** \brief digital setpoint mopup service
*/
void digsp_Mopup(void)
{
    if(digsp_req)
    {
        ErrorCode_t err = ram2nvramAtomic(NVRAMID_digitalsp);
        if(err == ERR_OK)
        {
            digsp_req = false;
        }
    }

    DigitalSpConf_t conf;
    (void)digsp_GetConf(&conf);

    /* Q: What's wrong with this?
    if( (bios_GetTimer0Ticker() - digsp_GetData(NULL)->timestamp) >= conf.ShedTime ) ...
    A: The compiler has a right (and happens to use it) to evaluate bios_GetTimer0Ticker()
    BEFORE digsp_GetData(NULL)->timestamp. If the setpoint came in between the two
    places (sequence points or slightly in the functions) the ticker would be slightly
    smaller than timestamp (modulo 2**32) so the difference will be a huge number.
    So, the order matters a lot.
    */
    tick_t tstamp = digsp_GetData(NULL)->timestamp;
    tick_t tdiff = bios_GetTimer0Ticker() - tstamp;
    /* tdiff is (logically) negative - timestamp in the future - if it is >INT_MAX)
    */
    if( (tdiff < INT_MAX) && (tdiff >= conf.ShedTime) )
    {
        bool_t isValid = digitalsp.isValid;
        if(isValid)
        {
            u8 xmode;
            if(conf.IsTargetToManual)
            {
                xmode = IPC_MODE_MANUAL;
            }
            else
            {
                //target is OOS == Failsafe
                xmode = IPC_MODE_OOS;
            }
            //devmode_t m = mode_GetEffectiveMode(mode_GetMode());
            u8 xm = digsp_GetExternalMode();
            MN_ENTER_CRITICAL();
                storeMemberBool(&digitalsp, isValid, false);
                digsp_Invalidate(&conf);
                //if(m  == MODE_OPERATE)
                if((xm & (IPC_MODE_AUTO|IPC_MODE_MANUAL))!=0)
                {
                    storeMemberInt(&digitalsp, xmode, xmode);
                }
            MN_EXIT_CRITICAL();
            error_SetFault(FAULT_SETPOINT_TIMEOUT);
        }
    }
}

const digitalsp_t *digsp_GetData(digitalsp_t *dst)
{
    const digitalsp_t *p;
#ifdef _lint //up to 9.00j, lint erroneously counts taking an address as thread-unsafe access
    MN_ENTER_CRITICAL();
#endif
        p = &digitalsp;
#ifdef _lint
    MN_EXIT_CRITICAL();
#endif
    return STRUCT_TESTGET(p, dst);
}

static const digitalsp_t digitalsp_default =
{
    .setpoint = 0, //don't really care
    .timestamp = 0, //don't really care
    .xmode = IPC_MODE_OOS,
    .CheckWord = 0, //don't care
};

CONST_ASSERT(NVRAMID_DigitalSpConf<NVRAMID_digitalsp);

//lint -sem(digsp_SetData, thread_not)
ErrorCode_t digsp_SetData(const digitalsp_t *src)
{
    if(src==NULL)
    {
        src = &digitalsp_default;
    }
    //range check?
    Struct_Copy(digitalsp_t, &digitalsp, src);
    MN_ENTER_CRITICAL();
        tick_t init_timestamp = DigitalSpConf.InitTime - DigitalSpConf.ShedTime; //pretend setpoint received in the future
        storeMemberInt(&digitalsp, timestamp, init_timestamp);
        if((digsp_GetConf(NULL)->sp_option == SSO_current_position))
        {
#ifdef AK //looks like a better behavior but needs support elsewhere
            if(reset_IsWarmstart() && digitalsp.isValid)
            {
                //keep last setpoint
            }
            else
#endif
            {
                //pick up the valve position later
                storeMemberS32(&digitalsp, setpoint, SETPOINT_INVALID);
            }
        }
        storeMemberBool(&digitalsp, isValid, true); //Let the init timeout expire to declare it false
    MN_EXIT_CRITICAL();
    MN_DBG_ASSERT(!oswrap_IsOSRunning());
    return ram2nvramAtomic(NVRAMID_digitalsp);
}

/** \brief Returns characterized position setpoint
\return  position setpoint (characterized to position domain)
*/
s32 digsp_GetDigitalPosSetpoint(void)
{
    s32 sp = digsp_GetData(NULL)->setpoint;
    return sp;
}

/** \brief Returns position setpoint as set (with inverse characterization applied)
\return inversely characterized position setpoint (in digital setpoint domain)
*/
s32 digsp_GetDigitalSetpoint(void)
{
    s32 sp = digsp_GetDigitalPosSetpoint();
    sp = poscharact_Inverse(sp); //apply inverse characterization
    return sp;
}


//doc in the project-independent header
s32 mode_GuardSetpoint(devmode_t mode, ctlmode_t ctlmode, s32 sp)
{
    UNUSED_OK(ctlmode);
    digitalsp_t digsp;
    (void)digsp_GetData(&digsp);
    if(digsp.setpoint == SETPOINT_INVALID) //This should only happen on the first run
    {
        digsp_Invalidate(digsp_GetConf(NULL));
    }
    //not! already done by caller -
    mode = mode_GetEffectiveMode(mode);
    if(mode == MODE_OPERATE)
    {
        sp = digsp_GetData(NULL)->setpoint;
    }
    return sp;
}

/* take the previous setpoint and guard it
*/
s32 mode_GuessSetpoint(devmode_t mode, ctlmode_t ctlmode)
{
    s32 pos;
    UNUSED_OK(mode);
    UNUSED_OK(ctlmode);
#ifdef AK
    if(digitalsp.isValid)
    {
        pos = digsp_GetDigitalPosSetpoint();
    }
    else
#endif
    {
        pos = vpos_GetScaledPosition();
    }
    return pos; //current valve position is the (manual) setpoint
}

devmode_t mode_RepairStartupMode(devmode_t mode)
{
    if((mode == MODE_SETUP) || (mode == MODE_MANUAL) || (mode == MODE_FAILSAFE))
    {
        //OK
    }
    else if(mode == MODE_OPERATE)
    {
        //Depends on configuration, so still OK
    }
    else
    {
        //treat it as NVMEM faulure
        error_SetFault(FAULT_NVM_CHECKSUM0);
        mode = MODE_FAILSAFE;
    }
    return mode;
}

bool_t mode_IsPersistent(devmode_t mode)
{
    UNUSED_OK(mode); //in other (HART) projects, failsafe is not persistent
    return true;
}

/* This line marks the end of the source */

