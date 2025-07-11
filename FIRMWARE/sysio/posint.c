/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file posext.h
    \brief System-level data and services related to internal (Hall) position measurement

1. Sensor-specific raw data compensation
2. Sensor-specific linearization

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/posint.c $
    $Date: 1/24/12 1:17p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: posint.c $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 1/24/12    Time: 1:17p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8765 - 32-bit Hall sensor tempcomp
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:06p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8332 (Oops)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 7:12p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:19a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for position calculations
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/04/08   Time: 1:09p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * "Stops" are now separate entities with project-independent API
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 6:09p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * tempr_GetInstantTempr now takes sensor id as a parameter
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 8:16p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * Measurement techology-specific interfaces (channel functions) between
 * BIOS and position.c
*/
#include "mnwrap.h"
#include "posint.h"
#include "position.h"
#include "tempr.h"
#include "charact.h"

//#include "charact_table_pos.h"
#include "../services/charact_tables.h"
//#include "charact_def.h"

//hall sensor conversions
#define MAGNET_GAUSS 1000.0F
#define TABLE_AT_1000_GAUSS 12560.0F
#define STD_FROM_GAUSS(x) ((s16)((TABLE_AT_1000_GAUSS*(x))/MAGNET_GAUSS))   //converts gauss to standard counts
//#define GAUSS_AT_90_DEGREES 1000.0F
#define GAUSS_AT_70_DEGREES 940.0F

//------------------ required interfaces, AP/ESD version ---------------------
#define  LOW_POSITION_STOP_DEF 0
#define  HIGH_POSITION_STOP_DEF 10000

//Shared with posext
const PositionConf_t def_PositionConf =
{
    .rangeval = //necessarily bogus; maps -30 mV to 0% and 30 mV to 100%
    {
        [Xlow] = LOW_POSITION_STOP_DEF,
        [Xhi] = HIGH_POSITION_STOP_DEF,
    },
    .PositionRange =
    {
        [Xlow] = STANDARD_ZERO,
        [Xhi] = STANDARD_100,
    },
    .bATO = true,
    .filtercoef = FILTER_DEFAULT,
    CHECKFIELD_CLEAR()
};


TEMPCOMP_C_ENTRY32(posint) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_GAUSS(-GAUSS_AT_70_DEGREES),
        [Xhi] = STD_FROM_GAUSS(GAUSS_AT_70_DEGREES),
    },
    .fcode = FAULT_POSITION_SENSOR,
    .ErrLimit = ERRLIMIT_DEFAULT,
};

void posint_GetInstantTempCompParams(OffsetSpanS32_t *curveseg)
{
    MN_ENTER_CRITICAL();
        *curveseg = posint_TemprCorrRT.curveseg;
    MN_EXIT_CRITICAL();
}

#if 0 //let's keep it for reference for now
// .09%/degree C, but temperature is in .01 degrees c
#define MAG_CORRECTION_TEMP_OFFSET 2000  //temperature for no correction
#define MAG_TEMPERATURE_COEFFICIENT (0.000009F)
#define MAG_CORRECTION_SHIFT ((u8)24)
#define MAG_CORRECTION_SHIFT1 ((u8)8)
#define MAG_CORRECTION_SHIFT2 ((u8)(MAG_CORRECTION_SHIFT-MAG_CORRECTION_SHIFT1))
#define MAG_CORRECTION_SCALE ((s32)((MAG_TEMPERATURE_COEFFICIENT * (float32)(1u<<MAG_CORRECTION_SHIFT)) + 0.5F))

/** \brief corrects the temperature compensated position for magnet variation with temp

\deprecated NOTE: This business should be wrapped into temperature correction table
  \param[in] RawPosition - temperature correction position
  \return magnet corrected position
*/
static s32 comp_MagnetTemperatureCorrection(s32 RawPosition)
{
    s32 Correction;
    s32 Factor;

    Factor = intscale32(MAG_CORRECTION_SCALE,
             ((s32)tempr_GetInstantTempr(TEMPR_MAINBOARD)-MAG_CORRECTION_TEMP_OFFSET),
             0, MAG_CORRECTION_SHIFT1);

    Correction = intscale32(RawPosition, Factor, 0, MAG_CORRECTION_SHIFT2);

    return (RawPosition + Correction);
}
#endif

static HardPos_t m_RawHallPosition;

/** \brief A compatibility interface
\return tempcomped Hall position before limiting and linearization
*/
HardPos_t posint_GetPreLinearized(void)
{
    return m_RawHallPosition;
}

/** \brief A mandatory input channel function
\param adval - raw A/D read of internal (Hall) position sensor
*/
void posint_Update(ADRaw_t adval)
{
    tcorr_t RawHallPosition = posint_TempComp(adval);
    m_RawHallPosition = RawHallPosition;
    ErrorCode_t err;
    if (mn_abs(RawHallPosition) > MAX_HALL_INPUT_ALLOWED)
    {
        err = ERR_CAL_POSITION_OUT_OF_RANGE;
    }
    else
    {
        err = ERR_OK;
    }

    HardPos_t pos = charact_HallCharacterization(RawHallPosition);
    pos_UpdateState(pos, err);
}
/* This line marks the end of the source */
