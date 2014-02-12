/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file timedef.h
    \brief Description of time data types

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/timedef.h $
    $Date: 1/06/12 4:38p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: timedef.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 4:38p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Still TFS:8370 (Oops)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 12:12a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8370 - time units
*/
#ifndef TIMEDEF_H_
#define TIMEDEF_H_
#include "timebase.h"

//------- Time (generic and might not be applicable to all time needs --------------
#define UNITSID_TIME_a 0
#define UNITSID_TIME_b (600 * TICKS_IN_ONE_SECOND)

//MNSEC - Masoneilan time in control cycles
//proprietary units for position speed (%/s)
#define H_MNSEC 249 //240-249 are proprietary units
//internal representation is not arbitrary: it maps to control cycles
#define UNITSID_MNTIME_a 0
#define UNITSID_MNTIME_b ((s32)(UNITS_MNSEC_B*((cdouble_t)ONE_SECOND/(cdouble_t)CTRL_TASK_PERIOD)))
#define UNITS_MNSEC_A 0.0
#define UNITS_MNSEC_B 326.7
#define UNITS_MNSEC_N 1
//test CONST_ASSERT(UNITSID_MNTIME_b==21780);
#define MNTIME_TO_CTLCYCLES(f) ((ctltick_t)(((f)-UNITS_MNSEC_A)*(UNITSID_MNTIME_b-UNITSID_MNTIME_a)/(UNITS_MNSEC_B-UNITS_MNSEC_A)))
//test CONST_ASSERT(MNTIME_TO_CTLCYCLES(UNITS_MNSEC_B)==21780);

//#define MILLISEC_PER_TIME_1COUNT 20
#define TICKS_IN_ONE_SECOND (200)

#define UNITS_SEC_A 0.0
#define UNITS_SEC_B 600.0 //20 ms resolution
#define UNITS_SEC_N 3 //4
#define H_SEC 51 //from HART 5 spec (SPEC-183.PDF)

//------- FILTER_TIME_CONSTANT used for filters --------------
#if 0 //FUTURE
//HART INTERFACE
#define H_FILTER_TIME_CONSTANT_SEC 51 //from HART 5 spec (SPEC-183.PDF)
//PRIMATIVE
//#define UNITSID_FILTER_TIME_CONSTANT_min 0
//#define UNITSID_FILTER_TIME_CONSTANT_max 120
//ENGINEERING UNITS FOR SEC WHICH IS A UNIT OF FILTER_TIME_CONSTANT
//Decimal places for SECONDS
#define UNITS_FILTER_TIME_CONSTANT_SEC_N 4
//ENGINEERING RANGE
//#define UNITS_FILTER_TIME_CONSTANT_SEC_A 0.0
//#define UNITS_FILTER_TIME_CONSTANT_SEC_B 600.0
//#define UNITS_FILTER_TIME_CONSTANT_SEC_A (UNITSID_FILTER_TIME_CONSTANT_min * 1.0)
//#define UNITS_FILTER_TIME_CONSTANT_SEC_B (UNITSID_FILTER_TIME_CONSTANT_max * 1.0)
#define UNITS_FILTER_TIME_CONSTANT_SEC_A 0.0
#define UNITS_FILTER_TIME_CONSTANT_SEC_B 120.0
//INTERNAL REPRESENTATION FOR ABOVE
//#define UNITSID_FILTER_TIME_CONSTANT_a (UNITSID_FILTER_TIME_CONSTANT_min * SLOW_FILTER_TIME_CONSTANT_SCALE)
//#define UNITSID_FILTER_TIME_CONSTANT_b (UNITSID_FILTER_TIME_CONSTANT_max * SLOW_FILTER_TIME_CONSTANT_SCALE)
#define UNITSID_FILTER_TIME_CONSTANT_a 0
#define UNITSID_FILTER_TIME_CONSTANT_b 491520

#define H_ELAPSED_TIME_SEC 51 //from HART 5 spec (SPEC-183.PDF)
#define UNITS_ELAPSED_TIME_SEC_N 2
//ENGINEERING RANGE
#define UNITS_ELAPSED_TIME_SEC_A 0.0
#define UNITS_ELAPSED_TIME_SEC_B 120.0
#define UNITSID_ELAPSED_TIME_a 0
#define UNITSID_ELAPSED_TIME_b 61440
#endif //FUTURE

#define H_SHEDTIME_SEC 51 //from HART 5 spec (SPEC-183.PDF)
#define UNITS_SHEDTIME_SEC_N 3 //4
#define UNITSID_SHEDTIME_a 0
#define UNITSID_SHEDTIME_b ((s32)(UNITS_SHEDTIME_SEC_B*(cdouble_t)LCL_TICKS_PER_SEC))
#define UNITS_SHEDTIME_SEC_A 0.0
#define UNITS_SHEDTIME_SEC_B (2.0E6) // about 23 days; close to max

#define TICKS_FROM_SHEDTIME_SEC(a) ((s32)(((((a) - UNITS_SHEDTIME_SEC_A) * (UNITSID_SHEDTIME_b - UNITSID_SHEDTIME_a)) / (UNITS_SHEDTIME_SEC_B - UNITS_SHEDTIME_SEC_A)) + UNITSID_SHEDTIME_a))

#if 0
//TBD whether we need minutes and hours
typedef s16 sec_t; //
typedef s16_least sec_fast_t; //
#define STD_FROM_SEC(a) ((s32)(((((a) - UNITS_SEC_A) * (UNITSID_TIME_b - UNITSID_TIME_a)) / (UNITS_SEC_B - UNITS_SEC_A)) + UNITSID_TIME_a))
#endif

#endif //TIMEDEF_H_
/* This line marks the end of the source */
