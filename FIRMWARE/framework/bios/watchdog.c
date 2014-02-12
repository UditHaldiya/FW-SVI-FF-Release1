/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup Watchdog  Internal watchdog and in-place resets
\brief Device driver for internal watchdog and in-place resets
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Watchdog Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_BIOS.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file watchdog.c
    \brief Device driver for internal watchdog - and in-place resets

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/framework/bios/watchdog.c $
    $Date: 7/28/11 12:13p $
    $Revision: 29 $
    $Author: Arkkhasin $

    \ingroup Watchdog
*/
/*
    $History: watchdog.c $
 *
 * *****************  Version 29  *****************
 * User: Arkkhasin    Date: 7/28/11    Time: 12:13p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Added History
*/
#include "config.h"
#include "mnwrap.h"
#include "timebase.h"
#include "watchdog.h"
#include "lpc2114io.h"
#include BIOS_DEF_H_
#include "bsp.h"
#include "process.h"
#include "pwm.h"

// #define MSEC_PER_SEC        1000u
#define WD_PRESCALE         4u      // prescaler in WD hardware
#define WD_PERIOD_3000      3000u
#define WD_COUNT_FROM_MS(ms) (u32)((cdouble_t)(ms) * ((cdouble_t)PCLKFREQ / WD_SCALE))
#define WD_PERIOD_10        10u
// #define WD_SCALE             ((cdouble_t)WD_PRESCALE * (cdouble_t)MSEC_PER_SEC)
#define WD_SCALE             ((cdouble_t)WD_PRESCALE * (cdouble_t)ONE_SECOND)
#define WD_SIGNATURE        0xdeadbeefU

#define INTERNAL_WD_RESET OPTION_ON

static volatile MN_NO_INIT u32 wdExpected;      // must be volatile

#define WD_PERIOD_150       160u     // in millseconds


/*
    Watchdog testing:

        Compile the RELEASE version of the firmware and download to target.
        Must use a unit with the external watchdog enabled.

    External

        Enable Factory commands and use HART command 130, address 0, type 137,
        Target should reset immediastely and should NOT set the watchdog fault.

    Internal

        Enable Factory commands and use HART command 130, address != 0, type 137
        Target should reset immediately and SHOULD set the watchdog fault.
*/

/** \brief Initializes the internal watchdog timer
*/
void wdog_WatchdogInit(void)
{
#if INTERNAL_WD_RESET == OPTION_ON
    if ((WDOG->WDMOD & WDTOF) != 0)
    {
        if (wdExpected != WD_SIGNATURE)
        {
            WDOG->WDMOD = 0;
            process_TrapWatchdog();
        }
    }
    wdExpected  = 0;
    WDOG->WDTC  = WD_COUNT_FROM_MS(WD_PERIOD_150);
    WDOG->WDMOD = WDEN | WDRESET | WDTOF;
#else
    bsp_InitWatchdogInterrupt(bsp_WatchdogInterrupt);
    WDOG->WDTC  = WD_COUNT_FROM_MS(WD_PERIOD_150);
    WDOG->WDMOD = WDEN;
#endif
}

/** \brief Extend the time for the internal watchdog and wait for the
    external watchdog to timeout.
*/
void wdog_WaitExternalWatchdog(void)
{
    WDOG->WDTC   = WD_COUNT_FROM_MS(WD_PERIOD_3000);
    WDOG->WDMOD  = WDRESET | WDEN;
    // feed sequence must not be interrupted (Any I/O access to peripheral address will cause trouble)
    mn_disable();
    WDOG->WDFEED = 0xaa;
    WDOG->WDFEED = 0x55;
    for (;;)
    {
    }
}

/** \brief Provides a hardware reset via the external/internal watchdog timer
*/
void wdog_HardwareReset(void)
{
    MN_ENTER_CRITICAL();
        WDOG->WDTC   = WD_COUNT_FROM_MS(1000*INPLACE_RESET_LENGTH);
        WDOG->WDMOD  = WDEN | WDRESET;
#if INTERNAL_WD_RESET  == OPTION_ON
        // Must be two of these because a reset is coming. wdExpected must be volatile
        // See the topic "On-chip Static RAM" in the LPC21x4 users manual
        wdExpected   = WD_SIGNATURE;
        wdExpected   = WD_SIGNATURE;
#endif
        WDOG->WDFEED = 0xaa;
        WDOG->WDFEED = 0x55;
        while (WDOG->WDTC != 0u)          // wait here for reset
        {
        }
    MN_EXIT_CRITICAL();
}

/** \brief The watchdog retrigger function - triggers the external and internal watchdog timers
*/
void wdog_WatchdogTrigger(void)
{
    GPIO0->IOSET = GPIO0_WDI; /* pulse is always longer than minimum pulse width of 60ns */\
    GPIO0->IOCLR = GPIO0_WDI;
#if USE_INTERNAL_WATCHDOG == OPTION_ON
    // feed sequence must not be interrupted (Any I/O access to peripheral address will cause trouble)
    MN_ENTER_CRITICAL();
        WDOG->WDFEED = 0xaa;
        WDOG->WDFEED = 0x55;
    MN_EXIT_CRITICAL();
#endif
}

/* This line marks the end of the source */
