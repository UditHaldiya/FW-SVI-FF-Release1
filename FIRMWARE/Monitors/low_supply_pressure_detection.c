/*
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file low_supply_pressure_detection.c
    \brief Implements improved algorithm for detecting low supply pressure
           from IP pressure and IP current. Old algorithm found in 3.1.3
           and beyond has the potential of getting "stuck" if the bias
           becomes so low that the pressure drops below the threshold

     CPU: Any

    $Revision: 2 $
    OWNER: JS
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors/low_supply_pressure_detection.c $
    $Date: 3/21/11 7:41p $
    $Revision: 2 $
    $Author: Justin Shriver $

    \ingroup monitors
*/
/* $History: low_supply_pressure_detection.c $
 * 
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 3/21/11    Time: 7:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:5914
 *
*/

#include "mnwrap.h"
#include "oswrap.h"
#include "mncbtypes.h"
#include "strong_types.h"
#include "pneumatics.h"
#include "cycle.h"
#include "tempcomp.h"
#include "ad.h"
#include "faultpublic.h"
#include "low_supply_pressure_public.h"


    // Note we assume that getting sensor reads is cheap and
    // always has a good public interface we can retrieve
    // we assume that getting parameters may be expensive because
    // it may be subject to a checksum or similar

    // This is unambigious without other sensors the only pressure sensor
    // we have is pilot
    /* This is a more interesting decision.  Each potential solution has various merits
       ip_current has the following advantages
       1. It is not subject to errors due to limiting before sending PWM, that is
          control desired PWM may not be what is sent.  This is only true in low
          power now but may be true in more conditions later.
       2. If control PWM is high but current is low (and can be made high)
          this design is robust to that failure
       3. ip_current is a physical constant for the design, control output is
          subject to arbitrary rescaling (in the future)
       control_output has the following advantages
       1. It should provide a tighter limit in SVi1000 because we have
          normalized this output so that all units produce similar pressure
          for similar PWM.  Due to variation in pilot pressure sensors this won't
          be true for ip current
       2. If the PWM loop is damaged this will not windup we will output
          the desired.  Note we could have a seperate windup limit to stop this
          we would need to examine its merits.
    */

    //const PneumaticParams_t const * = getPneumaticParametersFast();
    //

    //        Original code from control V337
    //        if( m_n2Press_IP < data->pPneumaticParams->SupplyLossThreshold_Pilot)
    //        {
    //            // SAR 3.3 partial
    //            cstate.BiasChangeFlag = AIR_LOLO_3;                // air loss
    //            error_SetFault(FAULT_AIR_SUPPLY_LOW);
    //            Pilot_Low = true;
    //        }
    //        else
    //        {
    //            if(cstate.BiasChangeFlag == AIR_LOLO_3)
    //            {
    //                cstate.BiasChangeFlag = 0;
    //            }
    //            error_ClearFault(FAULT_AIR_SUPPLY_LOW);
    //        }
        /* lowPilotlim must be
           1. Greater than minimum value for noise and minimum system value
           2. Less than a reasonable supply pressure for detection
        */

bool_t mon_checkLowSupplyFromIPandI(const PneumaticParams_t  * const pneumatic_parameters)
{

    bool_t    low_supply_detected = false;

    //Check if pressure is low, otherwise we are fine
    s16 IPpressure = cycle_GetPressureData()->Pressures[PRESSURE_PILOT_INDEX];
    if ( IPpressure < pneumatic_parameters->SupplyLossThreshold_Pilot)
    {
        //Straight forward return of pointer
        const AdDataRaw_t* pRawData = bios_GetAdRawData();
        //Moderate cost
        s16 current = comp_CorrectForTemperature((u32)AD_IP_CUR, (u32)pRawData->AdValue[AD_IP_CUR]);

        if (current > pneumatic_parameters->SupplyLossThreshold_Current)
        {
            /* lowIPlim (in PWM)
           1. Greater than the worst case value to get above
              lowPilotLim
           2. Less than the acceptable windup detection window,
              that is the larger it is the longer detection takes
              and the more vunerable you are to short pilot drops
            */
            /*Minimal range seperation is driven by
              1. If you have large seperation then this situation will
             not occure momentarily on negative steps because ipcur will be too low
             This make instantenious detection of the fault viable
              2. As the range decreases (lowIPlim decreases) chance of not having enough ipcur to
             overcome maximum range of pneumatics becomes and issue
            */
            //low_supply_ip_pilot++;
            low_supply_detected = true;
            error_SetFault(FAULT_AIR_SUPPLY_LOW);
        }
        else
        {
            /* Doing this on IP pressure has the advantage that
           clearing the fault is not hostage to how high lowIPlim
           is.  This decouples the clearing from the detection, and
           minimizes the chance of false detection
            */
            error_ClearFault(FAULT_AIR_SUPPLY_LOW);
        }
    }
    return (low_supply_detected);
}
