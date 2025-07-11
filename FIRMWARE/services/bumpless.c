/*
Copyright 2004-2008 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file bumpless.c
    \brief bumpless transfer process support

Modeled after trunk::bumpless.c,14

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/bumpless.c $
    $Date: 1/30/12 12:51p $
    $Revision: 12 $
    $Author: Arkkhasin $

    \ingroup Modeman
*/
/* $History: bumpless.c $
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:51p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 5:09p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 11/24/10   Time: 1:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4845 - power cancellation of transfer didn't result in normal mode
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 11/17/10   Time: 2:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4760 - code review issues
 * TFS:4758 - Correct handling of open-loop control modes
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 11/09/10   Time: 4:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4506 (Lint)
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 11/09/10   Time: 4:00p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4426 Lint: for loop irregularity
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 10/15/10   Time: 2:38p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Oops. Ark, compile before check-in
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 10/15/10   Time: 2:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4350 - bumpless transfer wrapper
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 5/14/10    Time: 11:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Update for new process API
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 6:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Constrained the use of nvramtypes.h
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 5:16p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Bumpless is back but now standalone
*/

#include "config.h"
#include "mnwrap.h"
#ifdef OLD_OSPORT
#include "oswrap.h"
#endif
#include "mnassert.h"
#include "errcodes.h"
#include "devicemode.h"
#include "uipublic.h"
#include "uistartnodes.h"
#include "process.h"
#include "bumpless.h"

#include "devfltpriv.h"
#include "faultpublic.h"

#include "signalsp.h"
#include "insignal.h"
#include "timebase.h"

#define SP_BUMPLESS_HILIMIT ((2*STANDARD_RANGE)-1) //!< This is max possible limit for the setpoint to reach
#define SP_BUMPLESS_LOLIMIT (-STANDARD_RANGE) //!< a min setpoint limit

#define  BUMPLESS_SPEED_DEF (0)


// ------------------------ Bumpless transfer business ----------------------------------
#define BUMPLESS_SLOPE_SHIFT (18) //artificial scaler
#define TIME_BUMPLESS_STEP_IN_MS 15U //to match the control cycle
/* AK:IMPORTANT When changing this number, make sure that an artificial
number BUMPLESS_SLOPE_SHIFT is such that the following number is computed correctly
as a 32-bit quantity:
SLOPE_NUMERATOR =

                  TIME_BUMPLESS_STEP_IN_MS     BUMPLESS_SLOPE_SHIFT
STANDARD_RANGE * ------------------------- * 2
                            1000
*/
#define SLOPE_NUMERATOR (( ((u32)STANDARD_RANGE * (u32)TIME_BUMPLESS_STEP_IN_MS) * ((1U<<BUMPLESS_SLOPE_SHIFT)/1000U) ))

static s32 pos_diff(s32 currpoint)
{
    s32 diff;
    s32 sp;
    sig_t sig = sig_GetSignal();
    sp = sigsp_ConvertSignalToSetpoint(sig);
    sp = MAX(sp, SP_BUMPLESS_LOLIMIT);
    sp = MIN(sp, SP_BUMPLESS_HILIMIT);
    diff = sp - currpoint;
    return diff;
}


static BumplessConf_t BumplessConf;

static const BumplessConf_t def_BumplessConf =
{
    .Required = 0, //optional
    .BumplessSpeed = BUMPLESS_SPEED_DEF,
    .CheckWord = CRC_SEED, //don't care
};

/**
\brief Performs the bumpless transfer procedure

This version implements new design whereby we move the manual setpoint
in the direction of the (original) signal setpoint; when the two trajectories
cross, we transfer to normal mode.

For smoothness, we now run the update every 15 ms, time permitting.
If we are running short of time, small jitters and/or extended stays
at one spot are possible, so the time of transfer is still NOT GUARANTEED,
even though it feels much better now.

Pre-requisites:
This function runs in the context of the process task.
\return PROCRESULT_FAILED or PROCRESULT_OK
*/
static procresult_t Run_Bumpless(void)
{
    u16_least StrokeTime; //in seconds per 100% stroke
    s32 stepN;
    procresult_t ret;

    s32 slope; //speed-dependent scaler
    s32 diff;
    s32 mansp, pos;

    StrokeTime = bumpless_GetConf(NULL)->BumplessSpeed;

    if(StrokeTime == 0U)
    {
        StrokeTime = 1U; //regularize
    }

    slope = (s32)(SLOPE_NUMERATOR/StrokeTime);

    MN_ENTER_CRITICAL();
        //Initialize Bumpless transfer;

        //If we can't set closed-loop control  mode, we'll fail inside the loop
        (void)mode_GetIntendedControlMode(&mansp);

        ErrorCode_t err = mode_SetModeEx(MODE_OPERATE, false);
        MN_DBG_ASSERT(err == ERR_OK);
        UNUSED_OK(err); //for release build

        // (2) Set closed loop control mode
        mode_SetControlMode(CONTROL_MANUAL_POS, mansp);
        pos = vpos_GetScaledPosition();
    MN_EXIT_CRITICAL();

    diff = pos_diff(pos);
    if(diff < 0)
    {
        slope = - slope;
    }

    //Loop
    stepN = 1;
    for(ret = PROCRESULT_CANCELED; ret == PROCRESULT_CANCELED;) // PROCRESULT_CANCELED is hijacked here as a flag to continue
    {
        if(process_WaitForTimeExt(TIME_VALUE(TIME_BUMPLESS_STEP_IN_MS), CANCELPROC_POWERMARGIN) != 0U)
        {
            ret = PROCRESULT_FAILED; //fail anything except marginal power
        }
        else
        {
            ctlmode_t ctlmode;
            mansp = intscale32(slope, (2*stepN)+1, pos, (u8)(BUMPLESS_SLOPE_SHIFT+1)); //halfstep resolution
            diff = pos_diff(mansp);
            MN_ENTER_CRITICAL(); //need to work on consistent snapshot; even a failsafe fault can't interject
                ctlmode = mode_GetIntendedControlMode(NULL);
                //AK: sanity check, in case things happen in the meantime
                if(ctlmode != CONTROL_MANUAL_POS)
                {
                    ret = PROCRESULT_FAILED; //fail
                }
                else
                {
                    //Per difference between manual setpoint and signal setpoint; direction taken into account
                    if( ((slope >= 0) && (diff > 0)) || ((slope < 0) && (diff < 0 )) )
                    {
                        mode_SetControlMode(CONTROL_MANUAL_POS, mansp);
                        //and leave ret at PROCRESULT_CANCELED to continue
                    }
                    else
                    {
                        //success: we are almost done
                        if(mode_SetMode(MODE_OPERATE) == ERR_OK)
                        {
                            ret = PROCRESULT_OK; //done with success indeed
                        }
                        else
                        {
                            ret = PROCRESULT_FAILED; //failed in the end but still failed
                        }
                    }
                }

            MN_EXIT_CRITICAL();
        }
        ++stepN;
    } //end for
    return ret;
}

//Need a wrapper for MISRA's dislikes of breaks
procresult_t mode_RunBumpless(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    procresult_t result;
    ui_setNext(UINODEID_BUMPLESS_XFER);
    result = Run_Bumpless(); //Fix of the UI node is now via proctable

    //Cleanup
    if(result != PROCRESULT_OK)
    {
        /* We failed to transfer; if setting manual mode succeeds, we stay in manual.
        Otherwise, we are in low power - or failed during the process;
        In these cases setting manual mode would fail and the (normal) target mode
        will remain unchanged.
        [NOTE: A momentary low power that canceled the process but is gone by now
        **would** leave us in manual mode, so we slap an additional test
        ]
        */
        MN_ENTER_CRITICAL();
            if(!error_IsAnyFaultWithAttributes(FATTRIB_LOWPWR))
            {
                (void)mode_SetMode(MODE_MANUAL);
                ui_setNext(UINODEID_BUMPLESS_XFER_FAILED);
            }
        MN_EXIT_CRITICAL();
    }

    return result;
}

/** \brief Set normal mode honoring bumpless transfer
\return error code
*/
ErrorCode_t mode_SetNormalModeEx(void)
{
    ErrorCode_t err = ERR_OK;
    u8 req = bumpless_GetConf(NULL)->Required; //have it outside the critsec
    MN_ENTER_CRITICAL();
        if(mode_GetMode() != MODE_OPERATE)
        {
            if(req == 0)
            {
                if(mode_SetMode(MODE_OPERATE) != ERR_OK)
                {
                    err = ERR_MODE_CANNOT_CHANGE;
                }
            }
            else
            {
                process_ForceProcessCommand(PROC_BUMPLESS);
            }
        }
    MN_EXIT_CRITICAL();
    return err;
}

const void *TypeUnsafe_bumpless_GetConf(void *dst)
{
    return STRUCT_TESTGET(&BumplessConf, dst);
}

ErrorCode_t TypeUnsafe_bumpless_SetConf(const void *src)
{
    const BumplessConf_t *bconf = src;
    if(bconf == NULL)
    {
        bconf = &def_BumplessConf;
    }

    //Any data is valid
    Struct_Copy(BumplessConf_t, &BumplessConf, bconf);
#ifdef OLD_NVRAM
    if(oswrap_IsOSRunning())
    {
        bumpless_SaveConf();
    }
#endif
    return ram2nvramAtomic(NVRAMID_BumplessConf);
}

/** \brief A feature-independent API for setting normal mode.
For projects with bumpless transfer
\return an error code
*/
ErrorCode_t mode_SetNormalMode(void)
{
    return mode_SetNormalModeEx();
}




#ifdef OLD_NVRAM

void bumpless_InitConf(InitType_t itype)
{
    BumplessConf_t bc;
    const BumplessConf_t *p;
    if(itype == INIT_FROM_NVRAM)
    {
        nvram2ram(&bc, NVRAMID_BumplessConf);
        p = &bc;
    }
    else
    {
        p = NULL; //default init
    }
    ErrorCode_t err = bumpless_SetConf(p);
    MN_ASSERT(err == ERR_OK);
}

void bumpless_SaveConf(void)
{
    BumplessConf_t bc;
    (void)bumpless_GetConf(&bc);
    MN_FRAM_ACQUIRE();
        ram2nvram(&bc, NVRAMID_BumplessConf);
    MN_FRAM_RELEASE();
}

#endif //OLD_NVRAM
/* This line marks the end of the source */
