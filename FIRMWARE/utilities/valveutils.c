/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file valveutils.c
    \brief Contains functions used by several other modules

    CPU: Any (with standard C compiler)

    OWNER: Schoonover
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/utilities/valveutils.c $
    $Date: 1/30/12 1:39p $
    $Revision: 30 $
    $Author: Arkkhasin $

    \ingroup Diagnostics
*/
/* $History: valveutils.c $
 *
 * *****************  Version 30  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:39p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 *
 * *****************  Version 28  *****************
 * User: Arkkhasin    Date: 12/05/10   Time: 10:54a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4963 (lint consistency).
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 11/09/10   Time: 7:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4504 (Lint - MISRA 20.2)
*/

#include "mnwrap.h"
#include "mnassert.h"
#include "pwm.h"
#include "utility.h"
#include "timebase.h"
#include "control.h"
#include "selftune.h" //for tune_GetPIDData()
#include "process.h"
#include "cycle.h"
#include "param.h"
#include "devicemode.h"
#include "pneumatics.h"

// for Waits
#define WAIT_STABLE_POS_COUNT       4u
#define WAIT_STABLE_POS_PRES_COUNT  3u
#define STANDARD_TENTH_PERCENT (STANDARD_RANGE/1000)
#define DEADBAND_DIVISOR (u16)8    //used to calculate increment of deadband - make a power of 2

//this is a new test condition - it requires that there be a direction change
//    as a condition of "stability" in util_WaitStablePosPres
#define TEST_ON 1
#define NEW_STABLE_CONDITION TEST_ON

/** \brief Waits for the specified number of ticks
  \param[in] u16 nDelay the number of ticks to wait
  \return none
*/
//AK: now process_WaitForTime() instead ofvoid util_WaitForTime(u16 nDelay)

/** \brief This routine waits until the position and pressure quit changing -
            i.e. the valve becomes stable at the setpoint.

  Notes:  In order to prevent the software from waiting forever, after 2 minutes
   the valve is considered stable.

  \param[in] u16 nTime - the time in timer ticks between tests to check if the valve has moved
  \param[in] u16 nPosDB - the amount of position change that is considered just noise
  \param[in] u16 nPresDB - the amount of pressure change that is considered just noise
  \return bool_t true if the valve is stable.  false if user canceled
*/
bool_t util_WaitStablePosPres(u16 nTime, u16 nPosDB, u16 nPresDB)
{
    const BoardPressure_t  *BoardPressure;
    const pres_t       *pres;
    bool_t          havePress;
    //LS: 12/3/04 changed from u16
    pres_t press;
    HardPos_t       pos,
                    NewPos;
    u16             exitTime = 0;
    u8_least        good = 0;
    bool_t          rslt;

#if NEW_STABLE_CONDITION==TEST_ON
    bool_t          DirectionChanged = false;
    bool_t          LastDirectionPlus = false;
    bool_t          DirectionPlus = false;
#endif   //end NEW_STABLE_CONDITION

    MN_ASSERT(nTime > 0);

    havePress = (pneu_GetParams(NULL)->SensorMap[PRESSURE_ACT1_INDEX] >= 0); //present

    BoardPressure    =     cycle_GetPressureData();
    pres = &(BoardPressure->Pressures[PRESSURE_ACT1_INDEX]);

    // Loop forever
    do
    {
        pos = vpos_GetRawPosition();
        press = *pres;

        //If time since last test > nTime then
        rslt = process_WaitForTime(nTime);
        if (rslt)
        {
            //If CheckCancelProcess() then return FALSE
            return false;                       // early exit - user cancelled
        }

        //If total time since call is > 2 minutes return TRUE
        exitTime += nTime;

        //If position change since last test < | nPosDB | then
        NewPos = vpos_GetRawPosition();

#if NEW_STABLE_CONDITION==TEST_ON
        DirectionPlus = false;
        if(NewPos > pos)
        {
            DirectionPlus = true;
        }
#endif   //end NEW_STABLE_CONDITION

            //lint -e{960}  the [mn_]abs function does not have side effects
        if (((u16)mn_abs(pos - NewPos) < nPosDB)
            //If actuator pressure sensor available then
            && ((!havePress)
                //If Pressure change since last test < | nPresDB | then
                || ((u16)mn_abs(press - *pres) < nPresDB)) )
        {
            //Increment stable count
            good++;

#if NEW_STABLE_CONDITION==TEST_ON
            if(DirectionPlus != LastDirectionPlus) //lint !e731 it's ok to compare booleans
            {
              DirectionChanged = true;
            }
#endif   //end NEW_STABLE_CONDITION
        }
        else
        {
            //Reset stable count to 0
            nPosDB += nPosDB/DEADBAND_DIVISOR;     //increase the deadbands so that we never get stuck
            nPresDB += nPresDB/DEADBAND_DIVISOR;
            good = 0;

#if NEW_STABLE_CONDITION==TEST_ON
            DirectionChanged = false;
#endif   //end NEW_STABLE_CONDITION
        }
      //If stable count >= 3 then return TRUE
    }
#if NEW_STABLE_CONDITION==TEST_ON
      while (((good < WAIT_STABLE_POS_PRES_COUNT) || (!DirectionChanged)) && (exitTime < (u16)T120_000));
#else
      while ((good < WAIT_STABLE_POS_PRES_COUNT) && (exitTime < (u16)T120_000));
#endif   //end NEW_STABLE_CONDITION

    return true;
}

/** \brief Waits for the valve to become stable and returns the bias value.
  \param[in] s16 nStdPos - the position in standard units at which to stabilize the valve
  \param[in] u16 nTime - the time in timer ticks between tests to check if the valve has moved
  \param[in] u16 nPosDB - the amount of position change that is considered just noise
  \param[in] u16 nPresDB - the amount of pressure change that is considered just noise
  \return   u16 - the IP output value when the valve is stable
*/
u16 util_GetStableBias(s16 nStdPos, u16 nTime, u16 nPosDB, u16 nPresDB)
{
    //LS: removed 11/29/04
    //const AdDataCompensated_t *adData;
    u16 Bias;

    //changed 11/18/04 LS - removed 12/18/04 ERP
    //MN_ASSERT((nStdPos >= -32768) && (nStdPos <= 32767));

    //put the device in manual mode at the desired position
    mode_SetControlMode(CONTROL_MANUAL_POS, (s32)nStdPos);

    //todo ERP should we allow cancel here?
    //LS - added cancel test
    if(process_WaitForTime((u16)T4_000))
    {
        return BIAS_ERR;        // early return - user cancel
    }
    if(!util_WaitStablePosPres(nTime, nPosDB, nPresDB))
    {
        return BIAS_ERR;        // early return - user cancel
    }
    if(process_WaitForTime((u16)T2_000))
    {
        return BIAS_ERR;        // early return - user cancel
    }

    //LS: removed 11/29/04
    //adData = bios_GetAdCompensatedData();
    Bias = pwm_GetValue();

    //return adData->Ip_Cur;
    return Bias;
}

/** \brief Waits until the valve becomes stable at the current setpoint
  \param[in] u16 nTime - the time (in timer ticks) between tests of the position
  \param[in] u16 nNoiseDB - the amount of position change that is considered just noise
  \param[in] bool_t cFlag - TRUE if waiting for the valve to match the setpoint,
                            FALSE if the waiting for the valve to be stable (at any position)
  \return bool_t true if the valve is stable.  false if user canceled
*/
bool_t util_WaitForPos(u16 nTime,  u16 nNoiseDB , bool_t cFlag)
{
    s32 lastPos, curPos, Setpoint;
    s32 error;
    u16 deadBand;
    u8_least  good;
    const PIDData_t * PID;

    MN_ASSERT(nTime > 0);

    control_GetSetpointPosition(&Setpoint , &lastPos);

    PID       = tune_GetCurrentPIDData(NULL);
    deadBand = PID->DeadZone + STANDARD_TENTH_PERCENT;      /* 0.1% in case nDeadZone  = 0.0% */
    good     = 0u;
    do
    {
        if (process_WaitForTime(nTime) == true)
        {
            return false;                        // early exit - user cancelled
        }

        control_GetSetpointPosition(&Setpoint , &curPos);

        if(cFlag)  // == WAIT_FOR_SETPOINT
        {
            //error is difference from setpoint
            error = curPos - Setpoint;
        }
        else
        {
            //error is difference from last point
            error = curPos - lastPos;
        }

        if( mn_abs(error) < (nNoiseDB + deadBand))
        {
             good++;
        }
        else
        {
            good = 0;
            deadBand += nNoiseDB/DEADBAND_DIVISOR;
            lastPos = curPos;
        }
    } while (good < WAIT_STABLE_POS_COUNT);

    return true;
}


/* end of the source */
