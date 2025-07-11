/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file
    \brief Contains functions used by several other modules

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

#ifndef CYCLE_H_
#define CYCLE_H_

#include "oswrap.h"

// ---------------- Begin deprecated -----------------
#include "tempr.h"
MN_INLINE const BoardTemperature_t* cycle_GetTemperatureData(void)
{
    return tempr_GetTemperature(TEMPR_MAINBOARD, NULL);
}

#include "pressures.h"
MN_INLINE s16 cycle_GetSupplyPressure(void)           // internal copy of supply pressure - maintained even in -2
{
    return pres_GetRawPressureData()->Pressures[PRESSURE_SUPPLY_INDEX];
}

// ---------------- End deprecated -----------------

//extern void cycle_FactoryResetTemperatureData(void);
#define cycle_GetPressureData() (pres_GetPressureData()) //deprecated
//#define cycle_GetRawPressureData() (pres_GetRawPressureData()) //deprecated
//#define cycle_GetRawMainPressure() (pres_GetRawMainPressure()) //deprecated

extern void periodicjob_RefVoltage(void);

extern const taskjob_t taskjob_cycle;

#define cycle_Mopup tempr_Mopup //deprecated
#endif


/* end of the source */

