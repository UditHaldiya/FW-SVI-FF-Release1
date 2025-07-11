/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file
    \brief Contains functions used by several other modules

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

/* (Optional) $History: calib.c $
 *
 * *****************  Version 69  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:46p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 *
 * *****************  Version 68  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:50a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * TFS:8748 - remove old pressure-related interfaces
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * TFS:8313 Lint
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 4:15p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 64  *****************
 * User: Arkkhasin    Date: 12/05/10   Time: 11:07a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4963 (lint consistency)
 *
 * *****************  Version 63  *****************
 * User: Sergey Kruss Date: 10/26/10   Time: 2:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4294  - OSadj moved from calib.c to osadjust.c
 *
 * *****************  Version 62  *****************
 * User: Sergey Kruss Date: 9/30/10    Time: 2:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * <TFS:3626> Added hand wheels handling with no air and removed reset to
 * 100% by guessing user's intent.
 *
 * *****************  Version 61  *****************
 * User: Sergey Kruss Date: 9/17/10    Time: 1:32p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Fixed bug TFS:3960 -- LINT note 960, MISRA 2004 Rule 11.5
 *
 * *****************  Version 60  *****************
 * User: Anatoly Podpaly Date: 7/16/10    Time: 4:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Bug #3406 - COrrect behavior of Open STop Adjustment so it will behave
 * consistently with other processes when cancelled by the Remote HART
 * Master - indicate an error.
 *
 * *****************  Version 59  *****************
 * User: Anatoly Podpaly Date: 6/09/10    Time: 4:14p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Revisited 3312 and 3324. CHenged the code to use one function call -
 * cnfg_SetPositionStop. The solution is to set Open STop Adjustment (part
 * of PositionSTop_t) and use it to reset Open STop Adjustment.
 * The operation sets the FP constant, no FP calculations.
 *
 * *****************  Version 58  *****************
 * User: Anatoly Podpaly Date: 6/09/10    Time: 12:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Bug #3324: modified function SetOnePos to include the reset of an Open
 * Stop Adj. to 100%, suimilar to FindStops.
 *
 * *****************  Version 57  *****************
 * User: Anatoly Podpaly Date: 6/03/10    Time: 11:30a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * 3312 - removed redundant check for CANCEL button - the cancellation is
 * supported via WaitForTime functionality.
 *
 * *****************  Version 56  *****************
 * User: Anatoly Podpaly Date: 5/28/10    Time: 3:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Added req numbers.
 *
 * *****************  Version 55  *****************
 * User: Anatoly Podpaly Date: 5/28/10    Time: 11:25a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Bug #3290 - FindStops shall reset the Open Stops Adjustment.
 *
 * *****************  Version 54  *****************
 * User: Anatoly Podpaly Date: 5/27/10    Time: 2:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * COrrected the header to MN format.
 *
*/

#include "mnwrap.h" //for intrinsic functions used by the OS port
#include "mnassert.h"
//#include "mncbtypes.h"
#include "errcodes.h"
#include "process.h"
//#include "mncbdefs.h"
#include "charact.h"
#include "charact_tables.h"
#include "control.h"
//#include "configure.h"
//#include "conversion.h"
#include "cycle.h"
#include "faultpublic.h"
#include "timebase.h"
#include "utility.h"
#include "uipublic.h"
#include "uistartnodes.h"
#include "devicemode.h"
#include "pneumatics.h"
#include "posint.h"
#include "adtbl.h"
#include "position.h"


/** \brief A helper, currently for Hall sensor
*/
static bool_t IsRawPosOutOfRange(void)
{
    bool_t ret;
    if (
        !ad_GetSensorType(NULL)->IsRemotePositionSensor
        && (mn_abs(posint_GetPreLinearized()) > MAX_HALL_INPUT_ALLOWED)
       )
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    return ret;
}

static poscalib_t ManStops =
{
    .rangeval = {0,0},
    .valid = {false, false},
};


/** \brief A subfunction of the position calibration routine

   Notes:  Performs calibrate operations at desired stop - high or low.
  \param[in] ctrlMode - the control mode (low-IP or HiGH-IP)
  \param[in] lit - UI place to display
  \return procresult so far
*/
static procresult_t DoHighLow(ctlmode_t ctrlMode, s16_least lit)
{
    //Set the control mode to low/high IP
    mode_SetControlMode(ctrlMode, 0); //setpoint doesn't matter

    //Wait 2 seconds by calling WaitForTime().  If error or cancel then
    //Set n4ErrorCode to PROCESS_CANCELED
    //Error_exit find stops
    if (process_WaitForTime((u16)T2_000))
    {
        return PROCRESULT_CANCELED;            // MN#2 early return from function with no cleanup
    }

    //Wait for stable position and pressure with 1 second tests by calling WaitForStablePositionPressure().
    if (!util_WaitStablePosPres((u16)T1_000, (u16)NOISE_BAND_STABLE, (u16)NOISE_BAND_PRES_STABLE))       // fix me EP
    {
        return PROCRESULT_CANCELED;            // MN#2 early return from function with no cleanup
    }

    ErrorCode_t err = poscal_AcceptStop();
    procresult_t ret;
    if(err==ERR_OK)
    {
        //Display the position (high or low) on the LCD
        HardPos_t hpos = ManStops.rangeval[(ctrlMode==CONTROL_IPOUT_LOW)?Xlow:Xhi];
        ui_setNextExt(lit, (s16)CLAMP(hpos, SHRT_MIN, SHRT_MAX));
        ret = PROCRESULT_OK;
    }
    else
    {
        ret = PROCRESULT_FAILED;
    }

    return ret;
}

/** \brief Performs the position calibration routine

    Notes:  This is a lengthy process and is scheduled by the process event
  \param[in] none
  \return proc result (ok/failed/canceled)
*/
procresult_t poscal_RunFindStops(s16 *procdata)
{
    UNUSED_OK(procdata);
    ErrorCode_t err;
    procresult_t ret;

    //Display the "Running" message on the LCD
    ui_setNext(UINODEID_STOPS_RUNNING); //AK: Do or don't display this 0?

    //(void)poscal_CommitStops(false); //Invalidate - always succeeds

    ret = DoHighLow(CONTROL_IPOUT_HIGH, UINODEID_STOPS_RUNNING_LOW);      //"LOW"
    if(ret == PROCRESULT_OK)
    {
        ret = DoHighLow(CONTROL_IPOUT_LOW, UINODEID_STOPS_RUNNING_HIGH); //"HIGH"
    }
    err = poscal_CommitStops(ret == PROCRESULT_OK);
    if(err != ERR_OK)
    {
        ret = PROCRESULT_FAILED;
    }

    //this call cleans up a bad bias - mostly needed for 1st time units
    control_ResetBiasChangeFlag();

    return ret;     // cleanup
}

ErrorCode_t poscal_AcceptStop(void)
{
    u8_least curr_stop;

    ctlmode_t ctlmode;
    ctlmode = mode_GetIntendedControlMode(NULL);

    //TBD    control_GetControlMode(&ctlmode, NULL);

    if(ctlmode==CONTROL_IPOUT_LOW)
    {
        curr_stop = Xlow;
    }
    else if(ctlmode==CONTROL_IPOUT_HIGH)
    {
        curr_stop = Xhi;
    }
    else
    {
        return ERR_CAL_INVALID_PROCEDURE_CALL;
    }

    //check the non linearized for out of range

    ErrorCode_t err;
    MN_ENTER_CRITICAL();
        if (IsRawPosOutOfRange())
        {
            err = ERR_CAL_POSITION_OUT_OF_RANGE;       // MN#2 early return from function with no cleanup
        }
        else
        {
            ManStops.rangeval[curr_stop] = (HardPos_t)vpos_GetSmoothedRawPosition();
            ManStops.valid[curr_stop] = true;
            err = ERR_OK;
        }
    MN_EXIT_CRITICAL();
    return err;
}

ErrorCode_t poscal_CommitStops(bool_t save)
{
    ErrorCode_t err;
    if(save)
    {
        if(!ManStops.valid[Xlow] && !ManStops.valid[Xhi])
        {
            return ERR_CAL_INVALID_PROCEDURE_CALL;
        }

        PositionConf_t conf;
        (void)pos_GetPositionConf(&conf);
        if(ManStops.valid[Xlow])
        {
            conf.rangeval[Xlow] = ManStops.rangeval[Xlow];
        }
        if(ManStops.valid[Xhi])
        {
            conf.rangeval[Xhi] = ManStops.rangeval[Xhi];
        }
        conf.PositionRange[Xhi] = OPEN_STOP_ADJ_MAX_INT; //we now reset stop adj on find stops
        err = pos_SetPositionConf(&conf);
        if(err == ERR_OK)
        {
            error_ClearFault(FAULT_FIND_STOPS_FAILED);
            //this call cleans up a bad bias - mostly needed for 1st time units
            control_ResetBiasChangeFlag();
        }
        else
        {
            error_SetFault(FAULT_FIND_STOPS_FAILED);
        }
    }
    else
    { //we are canceling
// comment out these lines to avoid displaying IPC OVRD
/*        MN_ENTER_CRITICAL();
            if(process_GetProcId()==PROC_FIND_STOPS)
            {
                process_CancelProcess();
            }
        MN_EXIT_CRITICAL();
*/        //Back to closed-loop control (FF will overwrite setpoint immediately)
        mode_SetControlMode(CONTROL_MANUAL_POS, vpos_GetScaledPosition());
        err = ERR_OK;
    }

    if(err == ERR_OK)
    {
        ManStops.valid[Xlow] = false;
        ManStops.valid[Xhi] = false;
    }

    return err;
}
/* end of the source */

