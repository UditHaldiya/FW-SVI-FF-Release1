/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file pwm.c
    \brief BIOS routines for PWM (I to P) output

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP/pwm.c $
    $Date: 1/16/12 3:27p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $

    \ingroup BIOS
*/

/*
    $History: pwm.c $
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 1/16/12    Time: 3:27p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * This file is project specific - moved into the project specific area.
 * 
 * *****************  Version 32  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 3:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- moved gpio definition.
 * 
 * *****************  Version 31  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 7:17p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- TFS:7899; 7898 - Use GPIO module.
 * 
 * *****************  Version 29  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 3:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * ELiminated LINT warning regarding file errcode.h. Scope of the bug
 * (4259)
 * 
 * *****************  Version 28  *****************
 * User: Arkkhasin    Date: 10/06/10   Time: 9:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4128 Dismantling io.{c,h}
 * +Standard tombstone header
*/

#include "mnwrap.h"
#include "lpc2114io.h"
#include "pwm.h"
#include "pbypass.h"
#include "oswrap.h"
#include "watchdog.h"
#include "bios_def.h"
#include "mncbtypes.h"
#include "configure.h"

/* (Optional) $History: pwm.c $
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 1/16/12    Time: 3:27p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * This file is project specific - moved into the project specific area.
 * 
 * *****************  Version 32  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 3:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- moved gpio definition.
 * 
 * *****************  Version 31  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 7:17p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- TFS:7899; 7898 - Use GPIO module.
 * 
 * *****************  Version 29  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 3:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * ELiminated LINT warning regarding file errcode.h. Scope of the bug
 * (4259)
 * 
 * *****************  Version 28  *****************
 * User: Arkkhasin    Date: 10/06/10   Time: 9:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4128 Dismantling io.{c,h}
 * +Standard tombstone header
 *
 * *****************  Version 27  *****************
 * User: Sergey Kruss Date: 9/01/10    Time: 10:31a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4015 and Code Review2.3.4
 * Moved PWM scaling to sysio.c
 *
 * *****************  Version 26  *****************
 * User: Sergey Kruss Date: 8/18/10    Time: 10:34a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Experimental modification for Pilot Pressure adjustment
 *
 * *****************  Version 25  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 11:39a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * 3262 - Moved the Enabling #defines to bios_def.h. COrrected header.
 *
*/

/* PMW timer counter is reset to 0 if match0 register reaches PWM_FREQUENCY */
/* PWM freq = 1.8khz -> (2/1.84e6 * 512(9bit)) (0..511 = 512 steps) */
#define PWM_FREQUENCY ((u32)511)

static MN_NO_INIT u16 wsSignature, wsValue;
static u16  lastVal;
static bool_t isWarmstart = false;

#define PWM_SIGNATURE (0x5a5a)

//----------------------------------------------------------------------------------------------------------------------

/** \brief Enable or disable the I/P output based on the passed parameter. 

    \param on - a boolean indicatiing whether the I/P is to be enabled (false) or disabled (true)
*/
static void pwm_SetEnableIP(bool_t on)
{
    if (on)
    {
        GPIO1->IOSET = GPIO1_EN_IP;
    }
    else
    {
        GPIO1->IOCLR = GPIO1_EN_IP;
    }
}

//----------------------------------------------------------------------------------------------------------------------

/** \brief Initialize PWM counter, channel 4,5 of PWM outputs and module
    specific variables. PWM output is set to zero.

    \param none
*/
void bios_InitPwm(void)
{
    PWM->PWMPR  = (u32)0;                   /* prescaler is set to 0 */
    PWM->PWMPC  = (u32)0;                   /* reset prescaler and timer counter */
    PWM->PWMTC  = (u32)0;
    PWM->PWMMR0 = PWM_FREQUENCY;
    PWM->PWMMR4 = (u32)0 ;                  /* PWM = 0 */
    PWM->PWMMR5 = (u32)0x1FF ;              /* PWM=0, but we have to invert PWM value */
    PWM->PWMMR6 = PWM_CHANNEL_6_FREQUENCY;
    PWM->PWMMCR = MR0RES ;                  /* Reset PWMTC if match0 matches PWMTC (freq. generation) */
    PWM->PWMPCR = PWM_CHANNELS_CFG_ENABLE;  /* PWM channel4,5 are single edge. and enabled */
    PWM->PWMLER = PWM_CHANNELS_RELOAD;      /* reload match channels */
    PWM->PWMTCR = (CE   | PWMEN);           /* enable counter and PWM */

    if (wsSignature == PWM_SIGNATURE)
    {
        bios_WritePwm(wsValue);
        bios_SetBypassSwitch(false);
        isWarmstart = true;
        wsSignature = 0;
    }
}

/** \brief return the current (most recent) value written to PWM

*/
u16 pwm_GetValue(void)
{
    return lastVal;
}

/** \brief Report warmstart state

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
        wsValue     = lastVal;
        bios_WritePwm((u16)(lastVal + 700));
        wdog_HardwareReset();
    MN_EXIT_CRITICAL();
}

/** \brief output the specified 16 bit value to the dual PWM

    \param[in] u16 value - the PWM value 0..65535
*/
void bios_WritePwm(u16 value)
{
    bool_t  EnableIP;
    lastVal = value;

    EnableIP = (value > (u16)MIN_DA_VALUE);
    pwm_SetEnableIP(EnableIP);

    /* send 9 MSB bits to match channel 4 */
    PWM->PWMMR4 = (value >> 7);
    /* invert PWM, multiply by 4 (expand from 7 to 9bit),send 9 LSB bits to match channel 5 */
    PWM->PWMMR5 = PWM_FREQUENCY - (u32)((value << 2) & 0x1ff);
    /* reload channel 4,5 match info */
    PWM->PWMLER = (EM4L | EM5L) ;
}

/* End of the source */
