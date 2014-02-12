/*
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file sysiolopwr.c
    \brief Implementation of the Low Power limiting functions and calculations

     CPU: Any

    $Revision: 5 $
    OWNER: JS
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/sysiolopwr.c $
    $Date: 1/30/12 12:43p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup poscontrol
*/

/* $History: sysiolopwr.c $
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:43p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:28p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 2/23/11    Time: 10:38a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5697 -- added checksum protection inside the critical section.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 2/16/11    Time: 5:15p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Low Power handling

*/

#include "mnwrap.h"
#include "oswrap.h"
//#include "mnassert.h"
#include "errcodes.h"
#include "nvram.h"
#include "pwm.h"           /* WritePwm */
#include "faultpublic.h"    /* fault codes */
#include "tempcomp.h"
#include "configure.h"
#include "tempcomp.h"
#include "smoothing.h"

#include "sysio.h"
#include "sysiolopwr.h"
#include "iplimit.h"
#include "power.h"

//-----------------------------------------------------------------------------------
// Low Power Handling variables

//This two state variable is self-healing - updated every control cycle.
//No checksum needed
static u16 m_n2PWMHighLimit = MIN_DA_VALUE;      // PWM High Limit

static LowPowerData_t m_LowPower;    // Low Power Data structure

#ifndef FAULT_LOW_POWER
//No power monitoring
//lint -esym(768, LowPowerData_t::uiHysteresis)
//lint -esym(768, LowPowerData_t::uiLo)
//lint -esym(768, LowPowerData_t::uiMarginal)
//lint -esym(768, LowPowerData_t::uiLoLo)
#endif

static const LowPowerData_t  def_LowPowerData =
{
//  See iplimit.h for derivation of these constants

    .uiHysteresis       = F2U(SIGNAL_TO_COUNTS(SIG_HYSTERESIS)),
    .uiLo               = F2U(SIGNAL_TO_COUNTS(SIG_MARGINAL_HIGH)),
    .uiMarginal         = F2U(SIGNAL_TO_COUNTS(SIG_MARGINAL_THRESH)),
    .uiLoLo             = F2U(SIGNAL_TO_COUNTS(SIG_MARGINAL_LOW)),

    .nPowerCoef         = F2U(POWER_SLOPE),

    .nPressureCoef      = F2U(PRESSURE_COEF),
    CHECKFIELD_CLEAR()
};

//-----------------------------------------------------------------------------------

/**
    \brief Conforming accessor to low power data.
    \return a pointer to low power data
*/
const LowPowerData_t* sysio_GetLowPwrData(LowPowerData_t *dst)
{
    return STRUCT_GET(&m_LowPower, dst);
}

/**
    \brief This function is externally called to set low power data and write it to NVRAM.
    \param[in] pLowPowerData - the pointer to low power data structure
*/
ErrorCode_t sysio_SetLowPowerData(const LowPowerData_t* pLowPowerData)
{
    if(pLowPowerData == NULL)
    {
        pLowPowerData = &def_LowPowerData;
    }

    // Copy the structure with the checksum
    Struct_Copy(LowPowerData_t, &m_LowPower, pLowPowerData);
    return ram2nvramAtomic(NVRAMID_LowPowerData);
}


/************************************************/

/** \brief This is the calculation for teh PWM High Limits. The calculations are using different
    reference value, so it is passed as the parameter of teh function.

    \param[in] type s32, reference level for PWM High Limit calculations.
    \return - PWM Hi Limit
*/
static u32  CalculateHighLimit(s32 ReferenceSignal)
{
    u16_least            Press_IPRaw;
    s32                  pwmHiLim;
    const AdDataRaw_t   *pAdDataRaw;


    // use the active power coefficient to compute a PWM value equal to 150 ua
    pwmHiLim = intscale32((s32)m_LowPower.nPowerCoef,
                               ReferenceSignal,
                               0,
                               CEIL_SHIFT_VALUE);

    // get a pointer to the raw A/D data
    pAdDataRaw = bios_GetAdRawData();
    // take the Pilot pressure, scale it into PWM units and add it to the PWM value
    Press_IPRaw = pAdDataRaw->AdValue[AD_PRESS_PILOT];

    pwmHiLim   += intscale32((s32)m_LowPower.nPressureCoef,
                                  (s32)Press_IPRaw,
                                  0,
                                  CEIL_SHIFT_VALUE);

    //Protect against conceivable arithmetic overflow
    return (u16)CLAMP(pwmHiLim, (s32)MIN_DA_VALUE, (s32)MAX_DA_VALUE); //cast is valid: see min/max values
}

/** \brief Before allowing Integral activity, check to make sure that someone
    has not upplugged the I/P.  Since it is possible to get a low enough value
    in the PWM that there be no I/P current we will allow integral action to
    a point that would generate up 150 micro amps of I/P current.

    \return - HI limit for PWM
*/
static u16 sysio_CalculateIPLimitsForControl(void)
{
    u16         pwmHiLim;

    pwmHiLim = (u16)CalculateHighLimit((s32)F2U(MIN_IP_IN_SIG_UNITS));

    return pwmHiLim;
}

/** \brief

*/
MN_INLINE bool_t NoItoPCurrent(void)
{
    return (bios_GetAdRawData()->AdValue[AD_IP_CUR] < (u16)TRIP_THRESHOLD);
}


/** \brief Before allowing Integral activity, check to make sure that someone
    has not upplugged the I/P.  Since it is possible to get a low enough value
    in the PWM that there be no I/P current we will allow integral action to
    a point that would generate up 150 micro amps of I/P current.

    \return - true if it appears that we are not in control of the I/P, false if OK to integrate
*/
bool_t sysio_CheckIPDisconnect(void)
{
    bool_t      ret;

    // only do the calculation if there is no (or minimum) I/P current
    if (NoItoPCurrent())
    {
        u16 pwmHiLim = sysio_CalculateIPLimitsForControl();
        // compute whether we already have enough PWM applied to generate 150 microamps of current
        ret = (pwm_GetValue() >= pwmHiLim);
    }
    else
    {
        // There is at least 50 microamps of I/P current - OK to integrate
        ret = false;
    }
    return ret;
}


/**
    \brief This function is main function and externally called to run control.
     It gets raw data of signal, which is then temperature compensated and converted
    to the standard range.
     The signal level will be identified if it is normal (NORMAL_POWER) or
     low power (LOW_POWER) or marginal power (MARGINAL_POWER)
     Position measurement is read in and processed and converted to the standard range.
     Both signal and position data are smoothed due to its noise
     function control_Prepare(data) and control_PID(data) are called to execute PID control.
*/
void sysio_Handle_LowPowerLimiting(void)
{
    u16 LocalPWMHighLimit;

    // integrity test the low power data structure
    Struct_Test(LowPowerData_t, &m_LowPower);  // RAM data test

    // Test the input current minimum value required for normal operation
    // if signal above marginal range clear marginal and low power conditions
#ifdef FAULT_LOW_POWER
    u16 SignalRaw = bios_GetAdRawData()->AdValue[AD_AI_SP];
    if (SignalRaw > m_LowPower.uiLo)
    {
        LocalPWMHighLimit = MAX_DA_VALUE;
        error_ClearFault(FAULT_LOW_POWER);
#ifdef FAULT_MARGINAL_POWER
        error_ClearFault(FAULT_MARGINAL_POWER);
#endif
    }
    // if signal is in the marginal range, clear low poer state and compute the max allowed I/P current
    else if (SignalRaw > m_LowPower.uiLoLo)
    {
        u16     SignalFromLoLo;

        error_ClearFault(FAULT_LOW_POWER);

        SignalFromLoLo    = (u16)(SignalRaw - m_LowPower.uiLoLo);
        LocalPWMHighLimit = (u16)CalculateHighLimit((s32)(SignalFromLoLo));

        // now set marginal power state and faults
#ifdef FAULT_MARGINAL_POWER
        if (SignalRaw < m_LowPower.uiMarginal)
        {
            error_SetFault(FAULT_MARGINAL_POWER);
        }
        // clear marginal power state and fault
        else if (SignalRaw > (m_LowPower.uiMarginal + m_LowPower.uiHysteresis))
#endif
        {
            error_ClearFault(FAULT_MARGINAL_POWER);
        }
        else
        {
            //leave FAULT_MARGINAL_POWER as is in hysteresis zone
        }
    }
    else
    {   // low power state, cut off the I/P altogether
        LocalPWMHighLimit = MIN_DA_VALUE;

        // set low power state
        if (SignalRaw < (m_LowPower.uiLoLo - m_LowPower.uiHysteresis))
        {
            // clear marginal power fault and set low power fault
#ifdef FAULT_MARGINAL_POWER
            error_ClearFault(FAULT_MARGINAL_POWER);
#endif
            error_SetFault(FAULT_LOW_POWER);
        }
        // change to marginal power state, set marginal power fault and clear low power fault
        else
        {
            error_ClearFault(FAULT_LOW_POWER);
#ifdef FAULT_MARGINAL_POWER
            error_SetFault(FAULT_MARGINAL_POWER);
#endif
        }
    }
#else //don't detect low power
    LocalPWMHighLimit = MAX_DA_VALUE;
#endif //def FAULT_LOW_POWER

    // Store calculated values
    m_n2PWMHighLimit = LocalPWMHighLimit;
}

/**REQ 40 (TFS:4108)**/
/**
    \brief A GET-type function returns upper limit of PWM value
           limited by low current.

    \return the upper PWM limit
*/
u16 sysio_GetPWMHightLimit(void)
{
    return m_n2PWMHighLimit;
}

#ifdef FAULT_LOW_POWER
// 3/8/2005 quick fix by EP for apparent write of FRAM during low power
#define SIGNAL_CNT_PER_MA 2621.4F  /* 0.1 volts/ma and 2.5 volts for 65535 counts= 2621.4  */
#define POWER_THRESHOLD (u16)(3.0F*SIGNAL_CNT_PER_MA)   /* raw count */
#endif

/**
  \brief  This function checks to see if the power is currently
  above the operating threshold.
  \return - true if there is enough power
*/
bool_t power_IsOK(void)
{
#ifdef FAULT_LOW_POWER
    const AdDataRaw_t *raw;
    raw  = bios_GetAdRawData();

    // 3/8/2005 quick fix by EP for apparent write of FRAM during low power
    return (raw->AdValue[AD_AI_SP] > POWER_THRESHOLD);
#else
    return true;
#endif
}

/* This line marks the end of the source */
