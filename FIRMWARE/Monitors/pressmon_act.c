/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors/pressmon_act.c $
    $Date: 12/21/10 2:03p $
    $Revision: 6 $
    $Author: Justin Shriver $

    CPU: Any

    \file pressmon_act.c
    \brief Performs pressure monitoring on actuator pressure.
           Including comparisions with low values and supply pressure

*/
#include "mnwrap.h" //for basic types

#include "param.h" //for PRESSURE_SCALE_RANGE




//#define IN_ActuatorOutputMonitors
#include "pressmon_sys.h"
#include "pressmon_sub.h"
#include "pressmon_act.h"  //included first to prove independence
#include "pressmon_act_hw.h"

//xlint -e960
//#undef IN_ActuatorOutputMonitors
//xlint +e960



typedef struct actpressdiag_t{
    pressure_t main_pressure;
    pressure_t low_pressure_limit;
    pressure_t high_pressure_limit;
    pressure_t diff_pressure_limit;
    bool_t dbl_press_high_is_act1;
    limit_t pressure_limit_type;
    bool_t is_single_acting;
    bool_t is_act_fault;
    bool_t limit;
} actpressdiag_t;

static actpressdiag_t actpressdiag; //debug trace info


/** \brief Checks if Pressure Is At Minimum Or Maximum And Checks Faults

  \param[out] limit_t pressure_limit - is the pressure limit on the low, high or none
  \return bool_t - false if pressure is not limited or if pressure is calculated to be limited but
                    any of the required sensors have a fault
*/
bool_t actmon_isActuatorPressureAtLimit(limit_t* pressure_limit)
{
    UNUSED_OK(actpressdiag);
    bool_t  is_actuator_fault_present;
    bool_t  result;
    bool_t  is_act_single_acting_cache;
    s16 diffLimit;

    const s16* raw_pressures = cycle_GetRawPressureData()->Pressures;
    // Establish method for determining saturation.  Use pilot pressure if LowCost
    // or if any of the needed pressure sensors are deemed faulty.
    s16     actuatorLimit, mainPressure;

    // compare with  cycle_GetSupplyPressure();
    actuatorLimit  = raw_pressures[PRESSURE_SUPPLY_INDEX];
    actuatorLimit -= ACTUATOR_TO_SUPPLY_PRESS_TOLERANCE(actuatorLimit);
    actpressdiag.high_pressure_limit =  actuatorLimit;
    mainPressure   = cycle_GetRawMainPressure();       // ACT1 for SA; ACT1 - ACT2 for DA
    actpressdiag.main_pressure =  mainPressure;

    result = false;
    *pressure_limit = actmon_ACT_LIM_NONE;
    // single acting && sensors OK
    is_act_single_acting_cache = isSingleActing();
    actpressdiag.is_single_acting =  is_act_single_acting_cache;
    if (is_act_single_acting_cache == true)
    {
        if (mainPressure <= (ACTUATOR_LOW_PRESS_LIMIT))
        {
            actpressdiag.low_pressure_limit = ACTUATOR_LOW_PRESS_LIMIT;
            //SAR 4.1 1 psi lower limit for single acting and additional counts
            //if (m_Integral <= (Low_Integral_Limit - INTEGRAL_SAFETY_MARGIN))
            {
                result = true;
                *pressure_limit = actmon_ACT_LIM_LOW;
            }
        }
        if (mainPressure >= actuatorLimit)
        {
            //SAR 4.2 actuator limit plus 200 counts
            //if (m_Integral >= (High_Integral_Limit + INTEGRAL_SAFETY_MARGIN))
            {
                result = true;
                *pressure_limit = actmon_ACT_LIM_HIGH;
            }
        }
    } /*End single acting*/
    else
    {
        /* For double acting atmospheric pressure cancels out
        However, we now compare the difference of two sensors with a third
        for this we expect a higher average error so we reduce the
        actuatorLimit accordingly
        */
        actuatorLimit = actuatorLimit - DOUBLE_ACTING_ADJUSTMENT;
        actpressdiag.high_pressure_limit =  actuatorLimit;
        diffLimit         = MIN(actuatorLimit, ACTUATOR_DIFFERENTIAL_LIMIT);
        actpressdiag.diff_pressure_limit =  diffLimit;

        if ((mainPressure <= -diffLimit) && (raw_pressures[PRESSURE_ACT2_INDEX] >= actuatorLimit))
        {
            actpressdiag.low_pressure_limit = -diffLimit;
            actpressdiag.dbl_press_high_is_act1 = true;
            //SAR 5.1 lower double acting pressure and integral limit
            //                    if (m_Integral <= (Low_Integral_Limit - INTEGRAL_SAFETY_MARGIN))
            {
                result = true;
                *pressure_limit = actmon_ACT_LIM_LOW;
            }
        }
        if ((mainPressure >=  diffLimit) && (raw_pressures[PRESSURE_ACT1_INDEX] >= actuatorLimit))
        {
            actpressdiag.high_pressure_limit = diffLimit;
            actpressdiag.dbl_press_high_is_act1 = false;
            //SAR 5.2 upper double acting pressure and integral limit
            //if (m_Integral >= (High_Integral_Limit + INTEGRAL_SAFETY_MARGIN))
            {
                result = true;
                *pressure_limit = actmon_ACT_LIM_HIGH;
            }
        }
    } /*End double acting*/
    is_actuator_fault_present =
        isLowCost() ||                           // -1 has only pilot sensor
            error_IsFault(FAULT_PRESSURE1) ||       //lint !e960 error_IsFault does not have side effects
                error_IsFault(FAULT_PRESSURE2) ||       //lint !e960 error_IsFault does not have side effects
                    (!is_act_single_acting_cache &&
                     error_IsFault(FAULT_PRESSURE3));    //lint !e960 error_IsFault does not have side effects

    if ( is_actuator_fault_present  )
    {
        result = false;
    }
    actpressdiag.is_act_fault = is_actuator_fault_present;
    actpressdiag.pressure_limit_type = *pressure_limit;
    actpressdiag.limit = result;
    return (result);
}

#define AK_A 0

#if AK_A
This stuff was never linked to anything in the FF project.
If you want to restore the debug information over HART (or such),
1. Provide read accessors to the local structures
2. Create commands and implement command processing functions to use the accessors

/** \brief Debugging function

  \return u8* - pointer to a valid hart buffer
*/
void DebugPressmonAct1(u8* dst)
{
    u16 bitpack=0;
    util_PutU16(dst +   0, (u16)actpressdiag.main_pressure);
    util_PutU16(dst +   2, (u16)actpressdiag.low_pressure_limit);
    util_PutU16(dst +   4, (u16)actpressdiag.high_pressure_limit);
    util_PutU16(dst +   6, (u16)actpressdiag.diff_pressure_limit);
    util_PutU16(dst +   8, (u16)actpressdiag.pressure_limit_type);
    util_PutU16(dst +  10, (u16)cycle_GetRawPressureData()->Pressures[PRESSURE_ACT1_INDEX]);
    util_PutU16(dst +  12, (u16)cycle_GetRawPressureData()->Pressures[PRESSURE_ACT2_INDEX]);
    util_PutU16(dst +  14, (u16)cycle_GetRawPressureData()->Pressures[PRESSURE_SUPPLY_INDEX]);
    util_PutU16(dst +  16, (u16)cycle_GetRawPressureData()->Pressures[PRESSURE_PILOT_INDEX]);
    util_PutU16(dst +  18, (u16)cycle_GetRawPressureData()->Pressures[PRESSURE_ATM_INDEX]);
    if (actpressdiag.dbl_press_high_is_act1)
    {
        bitpack+= 1;
    }
    if (actpressdiag.is_single_acting)
    {
        bitpack+= 2;
    }
    if (actpressdiag.is_act_fault)
    {
        bitpack+= 4;
    }
    if (actpressdiag.limit)
    {
        bitpack+=8;
    }
    util_PutU16(dst +  20, bitpack);
}
#endif //AK

// E O F
//-------------------------------------------------------------------------------------

