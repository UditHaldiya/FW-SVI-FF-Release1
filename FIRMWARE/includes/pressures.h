/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pressures.h
    \brief Pressures-related API

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/pressures.h $
    $Date: 11/14/11 11:36p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: pressures.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/14/11   Time: 11:36p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 2/10/10    Time: 7:01p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * Light linting
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 2:35p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * Pressure handling is split into individual channels
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 11/16/09   Time: 2:42p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * pressures presentation model moved from param.h to pressures.h
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 5/13/09    Time: 1:20a
 * Updated in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * Lint-inspired corrections and lawyering
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:11p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:00p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * dsampler_SamplePosPres declaration moved from dsampler.h to pressures.h
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:54p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Adapted to new tempcomp interface
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 12:37p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 * Type-safe accessors versions use SAFE{S,G}ET[X] macros
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 3:06p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * pres_GetPressureCalData is unused; commented out
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/15/08    Time: 10:44p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Pressures API consolidated here
*/
#ifndef PRESSURES_H_
#define PRESSURES_H_
#include "pressuredef.h"
#include "errcodes.h"
#include "ad.h" //for ADRaw_t; refactor

#define MAXNUM_PRESSURE_SENSORS ((u8)4) //AK:TODO: temporariliy here
typedef struct PressureCalData_t
{
    pres_t nLowPressure[MAXNUM_PRESSURE_SENSORS];
    u16 CheckWord;
} PressureCalData_t;


//AK:TODO: Make definitions more robust
#define PRESSURE_ACT1_INDEX     0u
#define PRESSURE_ACT2_INDEX     1u
#define PRESSURE_SUPPLY_INDEX   2u
#define PRESSURE_PILOT_INDEX    3u
#define PRESSURE_ATM_INDEX      4u
#define PRESSURE_MAIN_INDEX     5u //!< An artificial abstraction of SA/DA differences
#define NUMBER_OF_PRESSURE_SENSORS 5u

typedef struct BoardPressure_t
{
    pres_t Pressures[NUMBER_OF_PRESSURE_SENSORS+1];
} BoardPressure_t;

extern const BoardPressure_t* pres_GetPressureData(void);
extern const BoardPressure_t *pres_GetRawPressureData(void);
extern       pres_t pres_GetMainPressure(void);


/** Empirical estimate for noise level in MNCB pressure sensors.
It's used as an initial guess.
Changed from 0.1 to 0.15 for scale change from max pres 120 to 150
*/
#define NOISE_BAND_PRES_STABLE  STD_FROM_PSI(0.15)

extern void pres_PressureComp(void);
extern bool_t       cal_CalibratePressure(void);

/** A (somewhat arbitrary?) threshold on max acceptable zero deviation for
user calibration of zero of a pressure sensor
*/
#define MAX_PRESSURE_OFFSET               STD_FROM_PSI(3.2)

SAFESET(pres_SetPressureCalData, PressureCalData_t);
SAFEGET(pres_GetPressureCalData, PressureCalData_t);

extern pres_least_t pres_CorrectForTemperature(s8 adchannel, ADRaw_t adval);

#endif //PRESSURES_H_
/* This line marks the end of the source */
