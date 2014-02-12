/**
Copyright 2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file sysio.c
    \brief Implementation of system-level PWM I/O

    CPU: Any (with standard C compiler)
    OWNER: SK
    Created:  02/24

    $Revision: 27 $
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/sysio.c $
    $Date: 1/30/12 12:31p $
    $Author: Arkkhasin $History: sysio.c $
*/

#include "mnwrap.h"
#include "mnassert.h"
#include "errcodes.h"
#include "sysio.h"

#include "pwm.h"

#include "devicemode.h"
#include "oswrap.h"
#include "tempcomp.h"  /**REQ 40 (TFS:3303)**/

#include "sysiolopwr.h"     // TFS:5607 Include SYSIO Low Power headers

#include "pwmcomp.h"
#include "watchdog.h"
#include "pbypass.h"
#include "numutils.h"


//Sysio-level functionality (HW-independent)

static MN_NO_INIT u16 wsSignature, wsValue;
static bool_t isWarmstart = false;

#define PWM_SIGNATURE (0x5a5a)


void sysio_InitPwm(void)
{
    bios_InitPwm();
    if (wsSignature == PWM_SIGNATURE)
    {
        bios_SetBypassSwitch(false);
        bios_WritePwm(wsValue);
        isWarmstart = true;
        wsSignature = 0;
    }
}

/** \brief Report warmstart state
\return true iff warm start
*/
bool_t pwm_IsWarmstart(void)
{
    return isWarmstart;
}


/** \brief Set up the PWM for pending warmstart
*/
void pwm_DoWarmstart(void)
{
    MN_ENTER_CRITICAL();
        wsSignature = PWM_SIGNATURE;
        wsValue     = pwm_GetValue();
        if(wsValue > MIN_DA_VALUE)
        {
            u16 inplace;
            CONST_ASSERT((MIN_DA_VALUE + INPLACE_RESET_PWMBOOST) < MAX_DA_VALUE);
            if(wsValue < (MAX_DA_VALUE - INPLACE_RESET_PWMBOOST))
            {
                inplace = wsValue + INPLACE_RESET_PWMBOOST;
            }
            else
            {
                inplace = MAX_DA_VALUE;
            }
            bios_WritePwm(inplace);
        }
        wdog_HardwareReset();
    MN_EXIT_CRITICAL();
}



//----------------------------- I/P output -----------------------------
/**
\brief A wrapper around BIOS-level I/P output.
       \Returns the last value actually written to PWM registers
*/
/**REQ 40 (TFS:3303)**/
u16 sysio_GetRealPWM(void)
{
    return pwm_GetValue();
}


/**
\brief A safe wrapper around BIOS-level I/P output.
Intended for use by HART function which forcefully enters Control Output value to be normalized.
    \param ctlOut - a value to write as CtlOut
    The function is added per TFS:6074
*/
ErrorCode_t sysio_SetForcedCtlOutput(u16 ctlOut, enumPWMNORM_t output_mode)
{
    ErrorCode_t err = ERR_OK;
    mode_SetControlMode(CONTROL_OFF, 0);
    MN_ENTER_CRITICAL();
        if(mode_GetIntendedControlMode(NULL) == CONTROL_OFF)
        {
            //mode accepted
            (void)sysio_WritePwm(ctlOut, output_mode); //don't care about limiting
        }
        else
        {
            err = ERR_MODE_CANNOT_CHANGE;
        }
    MN_EXIT_CRITICAL();
    return err;
} //-----  end of sysio_SetForcedCtlOutput() ------


/** \brief Writes to PWM a value either calculated by the control program
    or corrected (normalized), depending on the value of the parameter
    bPWMcorr

    \param[in] value - the PWM value 0..65535
    \parma[in] bool_t bPWMcorr - determins whether to normalize PWM value
    \return bitmask of low/high limit event
*/
/**REQ 40 (TFS:3303)**/
u8 sysio_WritePwm(const s32 value, const enumPWMNORM_t enPWMcorr)
{
    s32 valueToWriteS32 = value;    /** TFS:4995 remove explicit casts -- allow LINT find type faults */
    s32 valueToWrite;

    /** TFS:8756 */
    MN_DBG_ASSERT( (enPWMcorr == PWMSTRAIGHT) || (enPWMcorr == PWMNORMALIZED) || (enPWMcorr == PWMEXACT) );
    u16 highPWMlimit = sysio_GetPWMHightLimit();
    switch ( enPWMcorr)
    {
        case PWMNORMALIZED:
            /**REQ 40 (TFS:4108) */
            /**REQ 40 (TFS:3303)**/ /** TFS:4108 */
            valueToWriteS32 = sysio_CtlOut2PWMdomain(value);    // TFS:5700
            valueToWrite = CLAMP(valueToWriteS32, (s32)MIN_DA_VALUE, highPWMlimit);    //TFS:4995
            break;

        case PWMSTRAIGHT:
            /** TFS:4108 */
            valueToWrite = CLAMP(valueToWriteS32, (s32)MIN_DA_VALUE, highPWMlimit);
            break;

        case PWMEXACT: //this case is used by non-power limited HART Cmd 160
        default:
            valueToWrite = CLAMP(valueToWriteS32, (s32)MIN_DA_VALUE, (s32)MAX_DA_VALUE);
            break;
    }//end switch

    bios_WritePwm((u16)valueToWrite); //cast is valid because of clamps to in-range values

    u8 ret = 0;
    if(valueToWrite <= (s32)MIN_DA_VALUE)
    {
        ret |= CTLOUTPUT_LIMITED_LOW;
    }
    if(valueToWrite >= (s32)MAX_DA_VALUE)
    {
        ret |= CTLOUTPUT_LIMITED_HIGH;
    }

    return ret;
} //-----  end of sysio_WritePwm() ------


//------------------- PWM compensation with new tempcomp -----------------
TEMPCOMP_C_ENTRY(pwmcomp) =
{
    .AllowedValue =
    {
        [Xlow] = MIN_DA_VALUE,
        [Xhi] = MAX_DA_VALUE,
    },
    .fcode = FAULT_NONE, //TBD but looks about right
    .ErrLimit = ERRLIMIT_DEFAULT,
};


/** \brief Converts values in control output (CtlOut) domain to values in PWM domain
    \param[in] s32 ctlOut - a number in CtlOut domain to be converted
    \parma[out] s32 - a number in PWM domain after conversion.

NO LONGER:
    Numbers in PWM domain may exceed the max allowable size for PWM - it is
    responsibility of the caller to limit or clamp the result.
*/
/** TFS:5700 */
s32 sysio_CtlOut2PWMdomain(const s32 ctlOut)
{
    return pwmcomp_TempComp(ctlOut);
} //-----  end of sysio_CtlOut2PWMdomain() ------




/* This line marks the end of the source */
