/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file vsteptest.c
    \brief Generic engine for (virtual) step test

    CPU: Any

    OWNER: AK, LS

    \ingroup Diagnostics
*/
#include "mnwrap.h"
#include "vramptest.h"
#include "ctllimits.h"
#include "devicemode.h"
#include "timebase.h"
#include "valveutils.h"
#include "diagnostics.h"
//#include "timer.h"
#include "mnassert.h"
#include "control.h"
#include "diagnostics.h"
#include "pressures.h"
#include "bufferhandler.h"
#include "timer.h"


//internal declarations
static pos_t GetEndCondition(pos_t EndPosition);


/** \brief routine move valve to starting position and stabalize before starting sampling

    \param bNoSetup - if true, skips initial stabilization of the valve
    \param StartPosition - the valve position at which the test starts

    \return completion code
*/
procresult_t diag_Prepare_VRampTest(bool_t bNoSetup, pos_t StartPosition, pos_t EndPosition)
{
    procresult_t procresult = PROCRESULT_OK;

    //go a bit outside the range to make sure we get the first positions
    s32 startsp = StartPosition - (EndPosition - StartPosition)/10; //10 percent outside the range

    
    //turn off rate limits (leave on but set to max rate)
    CtlLimits_t ctllims;
    (void)control_GetLimits(&ctllims);
    for(u8_least x=Xlow; x<Xends; x++)
    {
        ctllims.EnableSetpointRateLimit[x] = 1;
        ctllims.SetpointRateLimit[x] = INT_PERCENT_OF_RANGE(100.0);
    }
    ErrorCode_t err;
    err = control_SetTentativeLimits(&ctllims);
    if(err == ERR_OK)
    {
        ctllim_Reinit(startsp);

    }
    //stabalize valve at start
    mode_SetControlMode(CONTROL_MANUAL_POS, startsp);
    if(bNoSetup)
    {
        //don't stabalize the valve for this case - this is used
        // to continue from the last
    }
    else
    {
        //Wait for stable pressure and position
        if(process_WaitForTime(T4_000))
        {
            //Legitimate early return: - error cleanup handled in calling routine - diag_Run_RampTest
            return PROCRESULT_CANCELED;
        }

        bool_t s;
        s = util_WaitStablePosPres(T0_500,  STABLE_POS_TEST, STABLE_PRES_TEST);
        //NOTE: if no pressure sensor, pressure will be PRESSURE_INVALID and, of course, stable
        if(!s)
        {
            //Legitimate early return: - error cleanup handled in calling routine - diag_Run_RampTest
            return PROCRESULT_CANCELED;
        }
    }

    return procresult;

}

/** \brief A shell routine to run "virtual" ramp test

    \param bNoSetup - if true, skips initial stabilization of the valve
    \param sp_speed - setpoint ramp speed in %/s
    \param StartPosition
    \param EndPosition
    \param context - task that does sampling (normally, TASKID_CONTROL or TASKID_CYCLE)
    \param init_header_size - number of entries in the buffer reserved for header (must be even)
    \param max_dsamples - max samples to collect
    \param samplerf - a diagnostic sampler function


  \return completion code
*/
procresult_t diag_Perform_VRampTest(
                                           pos_t sp_speed,
                                           pos_t StartPosition,
                                           pos_t EndPosition
                                             )
{
    procresult_t procresult = PROCRESULT_OK;
    //To ramp the setpoint, we'll reuse the rate limits engine

    //(At this point, the limits are saved and disabled)
    CtlLimits_t ctllims;
    (void)control_GetLimits(&ctllims);
    for(u8_least x=Xlow; x<Xends; x++)
    {
        ctllims.EnableSetpointRateLimit[x] = 1;
        ctllims.SetpointRateLimit[x] = sp_speed;
    }
    ErrorCode_t err;
    err = control_SetTentativeLimits(&ctllims);
    if(err == ERR_OK)
    {
        s32 startsp = StartPosition - (EndPosition - StartPosition)/10; //10 percent outside the range
        ctllim_Reinit(startsp);

    }


    /* Now, set the setpont ramping in motion
    */
    s32 endsp = EndPosition + (EndPosition - StartPosition)/10; //10 percent outside the range
    pos_t EndCondition = GetEndCondition(EndPosition);  //endposition but limited to physical range
    
    s32 sp=0;
    s32 pos=0;
    u16 ticktime = 0;
    u16 StartTime = (u16)bios_GetTimer0Ticker();
    u16 ElapsedTime = 0;
    
        //Ramp - go past end position by 10% because the valve will not keep up and will slow down
        //    when the setpoint quits changing and thus might not otherwise get to the end position
        mode_SetControlMode(CONTROL_MANUAL_POS, endsp);

        do
        {
            if (process_WaitForTime(WAIT_TICKS)) ///check for cancel
            {
                procresult = PROCRESULT_CANCELED;
                break;
            }
            control_GetSetpointPosition(&sp, &pos);
            
            //start accumulating elapsed time after sp = endsp
            ticktime = (u16)bios_GetTimer0Ticker();
            if(sp != endsp) 
            {
              StartTime = ticktime;
            }
            ElapsedTime = ticktime - StartTime;

            
            //The terminating conditions are
            //  1.  sp has finished ramping
            //  2.  position has reached endposition or 5s time has elapsed since 1.
            //  was while((sp != endsp) && (pos_err > FINAL_STEP_ERR_THRSH)); //Was: wait for |pos_err|<threshold OR elapsed_extra_time > threshold
        } while(  (sp != endsp) || ( ((EndPosition>StartPosition)?pos<EndCondition:pos>EndCondition) && ElapsedTime < T5_000)  );
        

    return procresult;
}

/** \brief function to limit endposition to physical range

    \param pos_t EndPosition - the entered end position
    \return limited end position
*/
static pos_t GetEndCondition(pos_t EndPosition)
{
  pos_t Range[Xends];
  bool_t bATO = pos_GetStopAdjustment(Range);
  UNUSED_OK(bATO); //we don't care but range could be in reversed order
  if(Range[Xlow] > Range[Xhi]) //make xlo the lowest
  {
    pos_t hold = Range[Xlow];
    Range[Xlow]=Range[Xhi];
    Range[Xhi]=hold;
  }
  pos_t ActualEnd = CLAMP(EndPosition, Range[Xlow], Range[Xhi]);
  
  return ActualEnd;
}


/* This line marks the end of the source */
