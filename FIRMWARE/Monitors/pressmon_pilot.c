/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors/pressmon_pilot.c $
    $Date: 12/21/10 2:05p $
    $Revision: 6 $
    $Author: Justin Shriver $

    CPU: Any

    \file pressmon_pilot.c
    \brief Performs pilot pressure monitoring for out of control detection
           Note does not currently make use of supply pressure so that it
           works for both -1 and -2->-5.

*/

#include "mnwrap.h" //for basic types


#include "bufutils.h" //for cnfg_GetComputedConfigurationData()
// #include "errcodes.h" //needed for configure.h
#include "param.h" //for PRESSURE_SCALE_RANGE



//#define IN_ActuatorOutputMonitors
#include "pressmon_sys.h"
#include "pressmon_sub.h"  //included first to prove independence
#include "pressmon_pilot.h"  //included first to prove independence
#include "pressmon_pilot_hw.h"

//xlint -e960
//#undef IN_ActuatorOutputMonitors
//xlint +e960



typedef struct pilotpressdiag_t{
    pressure_t pilot_pressure;
    pressure_t low_pressure_limit;
    pressure_t high_pressure_limit;
    limit_t limittype;
    bool_t islowcost;
    bool_t issingleacting;
    bool_t pilotfault;
    bool_t limit;
} pilotpressdiag_t;

static pilotpressdiag_t pilotpressdiag; //debug trace



/** \brief Checks if Pilot Is At Minimum Or Maximum And Checks Faults

  \param[out] limit_t pressure_limit - is the pressure limit on the low, high or none
  \return bool_t - false if pressure is not limited or if pressure is calculated to be limited but
                    any of the required sensors have a fault
*/
bool_t actmon_isPilotPressureAtLimit(limit_t* pilot_limit)
{
    UNUSED_OK(pilotpressdiag);
    bool_t  result;
    s32 pilot_low_limit;
    s32 pilot_high_limit;
    s16 pilot;

    /*Get pointer to pressures (raw_pressures are compensated, but they don't
    include user calibration */

    const s16* raw_pressures = cycle_GetRawPressureData()->Pressures;

    /* Initilize limits to false */
    result = false;
    *pilot_limit = actmon_ACT_LIM_NONE;

    /*Read Pilot pressure*/
    pilot = raw_pressures[PRESSURE_PILOT_INDEX];
    /*Check if Pilot pressure is not faulty, note that this test is currently redundant
    as a pilot pressure fault would put the system in
    failsafe */
    pilotpressdiag.pilot_pressure = pilot;

    if (isLowCost())
    {
        pilotpressdiag.islowcost = true;
        //SAR 6.1 6psi lower limit for AP-1
        pilot_low_limit  = PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY;
        //SAR 6.2 19.715psi upper limit for AP-1
        pilot_high_limit = PILOT_HIGH_PRESS_LIMIT_AP_1;
    }
    else
    {
        pilotpressdiag.islowcost = false;
        if ( isSingleActing())
        {
            pilotpressdiag.issingleacting = true;
            //SAR 4.3 6psi lower limit for AP-2
            pilot_low_limit  = PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY;
            //SAR 4.4 22.28psi upper limit for AP-2
            pilot_high_limit = PILOT_HIGH_PRESS_LIMIT_AP_2;
        }
        else
        {
            pilotpressdiag.issingleacting = false;
            //Double acting
            //SAR 5.3 lower limit 4psi for double acting
            pilot_low_limit  = PILOT_LOW_PRESS_LIMIT_DOUBLE_ACTING;
            //SAR 5.4 upper limit 30psi for double acting
            pilot_high_limit = PILOT_HIGH_PRESS_LIMIT_DOUBLE_ACTING;
        }
    }

    pilotpressdiag.low_pressure_limit = (s16)pilot_low_limit;
    pilotpressdiag.high_pressure_limit = (s16)pilot_high_limit;

    // SAR 4.3, 5.3, 6.1
    // This relies on the fact that for single acting there is a minimum
    // relay input below which  no actuator pressure is generated.  Since
    // this occurs above zero PSI it forms a useful lower limit.  Note
    // for double acting the minimum relay output is ~ 0 psi and is thus
    // not significantly different than looking for minimum bias/IP
    if ( pilot <= pilot_low_limit )
    {
        result = true;
        *pilot_limit = actmon_ACT_LIM_LOW;
    }
    else
    {
        /* Limit not active do nothing */
    }


    // In the filling direction we can calculate an equivalent actuator pressure
    // from the pilot pressure.
    // SAR 4.4, 5.4, 6.2
    if ((pilot > pilot_high_limit))
    {
        result = true;
        *pilot_limit = actmon_ACT_LIM_HIGH;
    }
    else
    {
        /* Limit not active*/
    }


    //Check only at end so that we still can return direction
    pilotpressdiag.pilotfault = false;
    if (error_IsFault(FAULT_PRESSURE4))
    {
        pilotpressdiag.pilotfault = true;
        result = false;
    }
    pilotpressdiag.limit = result;
    pilotpressdiag.limittype = *pilot_limit;
    return(result);
}


#define AK_P 0

#if AK_P
This stuff was never linked to anything in the FF project.
If you want to restore the debug information over HART (or such),
1. Provide read accessors to the local structures
2. Create commands and implement command processing functions to use the accessors

/** \brief Debugging function

  \return u8* - pointer to a valid hart buffer
*/
void DebugPressmonPilot1(u8* dst)
{
    util_PutU16(dst +   0, (u16)pilotpressdiag.pilot_pressure);
    util_PutU16(dst +   2, (u16)pilotpressdiag.low_pressure_limit);
    util_PutU16(dst +   4, (u16)pilotpressdiag.high_pressure_limit);
    util_PutU16(dst +   8, (u16)pilotpressdiag.limittype);
    util_PutU16(dst +  10, (u16)pilotpressdiag.islowcost);
    util_PutU16(dst +  12, (u16)pilotpressdiag.issingleacting);
    util_PutU16(dst +  14, (u16)pilotpressdiag.pilotfault);
    util_PutU16(dst +  16, (u16)pilotpressdiag.limit);
    util_PutU16(dst +  18, (u16)cycle_GetRawPressureData()->Pressures[PRESSURE_PILOT_INDEX]);
}

#endif //AK

// E O F
//-------------------------------------------------------------------------------------

