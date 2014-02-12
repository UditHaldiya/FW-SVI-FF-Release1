/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file timebase.h
    \brief All time-related definitons

     CPU: Any

    OWNER: EP
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
#ifndef TIMEBASE_G_
#define TIMEBASE_G_

#define ASSERT_LIFESPAN     (TIME_VALUE(20000u)) //! Time to live for an assert record (now 20 seconds)

#define XTALFREQ    1843200u    //  cpu crystal frequency



// possible dividers for the peripheral bus clock
#define VPBDIV1		1u
#define VPBDIV2		2u
#define VPBDIV4		4u
#define VPRATE		VPBDIV2         // running rate - may be changed to
                                    //  VPBDIV1 during startup
#define PCLKFREQ (XTALFREQ / VPRATE)

#define BAUDRATE    9600u
#define BAUDRATEDIVISOR (PCLKFREQ / (BAUDRATE * 16u))

#define ONE_SECOND      1000u
#define TICK_MS         (ONE_SECOND / LCL_TICKS_PER_SEC)
#define CTRL_TASK_DIVIDER 3u        // number of timer ticks per control task cycle
#define DISP_TASK_DIVIDER 7u        // number of control task cycles per disp task cycle
#define AO_INTERVAL_CT 7u           // number of control task cycles per analog output cycle
#define TIMER_TICK_INTERVAL
#define COMPUTE_COEFF_INTERVAL  1000u // in milliseconds
#define COMPUTE_COEFF_DIVIDER\
    (COMPUTE_COEFF_INTERVAL / ((CTRL_TASK_DIVIDER * DISP_TASK_DIVIDER) * TICK_MS))


#define LCL_TICKS_PER_SEC 200u
#define TB_TICKS_PER_SEC ((LCL_TICKS_PER_SEC / CTRL_TASK_DIVIDER) + 1U)

#define TB_MS_PER_TICK     (1000u / LCL_TICKS_PER_SEC)

#ifndef _MSC_VER
//#   define TIME_VALUE(t) ((u16)((t) / TB_MS_PER_TICK)) //real target
#   define TIME_VALUE(t) ((u16)((t) / TB_MS_PER_TICK)) //real target
#else
#   define TIME_VALUE(t) ((u16)((t)* CLOCKS_PER_SEC/1000u)) //PC build; requires <time.h>
#endif

#define T0_050   TIME_VALUE(    50u)
#define T0_100   TIME_VALUE(   100u)
#define T0_150   TIME_VALUE(   150u)
#define T0_250   TIME_VALUE(   250u)
#define T0_500   TIME_VALUE(   500u)
#define T0_750   TIME_VALUE(   750u)
#define T1_000   TIME_VALUE(  1000u)
#define T1_500   TIME_VALUE(  1500u)
#define T2_000   TIME_VALUE(  2000u)
#define T2_500   TIME_VALUE(  2500u)
#define T3_000   TIME_VALUE(  3000u)
#define T4_000   TIME_VALUE(  4000u)
#define T5_000   TIME_VALUE(  5000u)
#define T20_000  TIME_VALUE( 20000u)
#define T100_000 TIME_VALUE(100000u)
#define T120_000 TIME_VALUE(120000u)

//By design, output calc occurs when all A/D channels are read, which occurs every control cycle
#define CTRL_CALCULATION_INTERVAL_MS CTRL_TASK_PERIOD //! period of output calc

typedef s16 ctltick_t; //!< count time in control cycles

#if 0 //for compile-time verification
#define CTRL_TASK_PERIOD 15 //! nominal period of control task in ms
CONST_ASSERT(TB_MS_PER_TICK*CTRL_TASK_DIVIDER == CTRL_TASK_PERIOD);
#else
#define CTRL_TASK_PERIOD (TB_MS_PER_TICK*CTRL_TASK_DIVIDER) //! nominal period of control task in ms
#endif

#define CYCLE_TASK_DIVIDER 7u        // number of control task cycles per disp task cycle

#endif //TIMEBASE_G_

/* This line marks the end of the source */
