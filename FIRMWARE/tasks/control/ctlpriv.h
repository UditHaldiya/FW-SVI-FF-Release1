/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ctlpriv.h
    \brief Private interfaces of the Control Module

    CPU: Any

    OWNER: DZ
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
#ifndef CTLPRIV_H_
#define CTLPRIV_H_



#include "param.h" //for standard range
#include "timebase.h" //for XTAL frequency

/* For scales calculation */
//#define BIAS_SHIFT_LIMIT 2500u
#define BIAS_DEFAULT    15000u
//#define BIAS_STABLE_GAP   50u
/* #define BIAS_SHIFT   300u */
//#define BIAS_AVE_TIME_CONSANT 4u
//#define BIAS_AVE_MASK 0x0F
#define BIAS_CHANGE_HI  3000
#define BIAS_CHANGE_LO  -1000
#define BIAS_MARGIN_HI  3000u
#define BIAS_MARGIN_LO  2000u
//Removed #define BIAS_ADJUST_TIGHT 600

#define AIR_LOLO_3   3
//#define ACT_LIMIT_6  6

#define NOT_STOP_0   0
#define CLOSED_1     1
#define OPEN_2       2

#define POS_SHIFT   40u
/* BIAS limits : I/P */
#define ERR_HIGH_P 8000
#define I_LOW_ZONE 149
#define I_HIGH_ZONE 199
#define I_LIMIT_AT_STOP 100
#define I_LIMIT_NOT_STOP 500

//#define P_LOW 90u
#define P_MID_CON 300u

#define MAX_FOR_16BIT 65535u
#define ERR_S 10
#define T_BASE  25             /* temperature reference 25 degree C */
#define T_LOW  -20             /* temperature low -20 degree C */
#define T_HIGH  70             /* temperature high 70 degree C */

#define P_CALC_SCALE 250
#define D_CALC_SCALE 25

#define I_CALC_COEF  67        /* integral calc coef */
#define I_CALC_LOW  10        /* integral calc low frequency */
// #define I_CALC_HIGH  40        /* integral calc high frequency */
//#define I_CALC_FREQ_COEF1  2000        /* integral calc frequency coef */
//#define I_CALC_FREQ_COEF2  400        /* integral calc frequency coef */
#define I_CALC_MID  24        /* integral calc frequency coef */

#define D_CALC_COEF  120

//#define POS_BOOST_1  200
//#define POS_BOOST_2  150
// #define POS_BOOST_3  120
// #define POS_BOOST_4  30

#define ERR_BOOST_1  120
#define ERR_BOOST_2  80

#define NOISE_RATIO  5u          /* noise ratio */
#define START_COUNT_MAX  65000
#define STAY_COUNT_MAX  65000
//#define STAY_COUNT_LOW  800

#define ERR_COUNT_MAX  65000
#define BOOST_COUNT_LOW  300
#define BOOST_COUNT_HIGH  500

/* #define POWER_CNT_LIMIT 10 */
// #define SIGNAL_CNT_PER_MA 2621.4F  /* 0.1 volts/ma and 2.5 volts for 65535 counts= 2621.4  */
// #define SIGNAL_CNT_PER_MA 2594.3F  /* corrected for actual amplifier gain  */
// #define POWER_3PT9 (u16)(3.9F*SIGNAL_CNT_PER_MA)   /* 10117 raw counts */
// #define POWER_3PT2 (u16)(3.2F*SIGNAL_CNT_PER_MA)   /* 8301 raw counts */
// #define POWER_3PT6 (u16)(3.6F*SIGNAL_CNT_PER_MA)   /* 9437 raw counts */
// #define POWER_3PT7 (u16)(3.7F*SIGNAL_CNT_PER_MA)    /* 9699   */
// #define POWER_3PT64 (u16)(3.64F*SIGNAL_CNT_PER_MA)    /* 9541   */
//#define CEIL_SLOPE_I (u16)(MAX_FOR_16BIT/(POWER_3PT9-POWER_3PT2))   /* 36  */

// #define CEIL_I_COEF 15     // 2376 x I_mA=2376 x 1.6 x I_std/2^16
// #define CEIL_P_COEF 294     // 1.15 x P_raw=18842/2^14 x P_raw

// #define GAP 82          /* about 0.5% of full valve travel(16384 count) for tight shutoff */
//#define TIGHT_TRIG_POS ((s16)(((float32)STANDARD_RANGE * 0.05F) + 0.5F))

/* Counters for CONTINUOUS DIAGNOSTIC */
#define TIME_INCREMENT  1u             /* 1 increment per second */


#define ONE_HOUR_3600 3600

typedef enum Diagnos_t   /* For continuous diagnostic */
{
    OPEN,
    CLOSED,
    NEAR
} Diagnos_t;

#endif //CTLPRIV_H_
/* This line marks the end of the source */
