/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_cmd_3.c
    \brief The functions used by HART command 3:

    CPU: Any

    OWNER: LS
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/hartfunc.c $
    $Date: 1/30/12 2:19p $
    $Revision: 218 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/

#include "mnwrap.h"
#include "projectdef.h"
//#include "pressmon.h"
#include "nvram.h"
#include "utility.h"
#include "devicemode.h"
#include "faultpublic.h"
#include "position.h"
#include "cycle.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hart.h"
#include "smoothing.h"

#include "hartdef.h"
#include "hartpriv.h"

#include "digitalsp.h"

#include "poscharact.h"

#include "fpconvert.h"
#include "configure.h"

/**
\brief Returns the signal, position, pressure, and if controller the setpoint and process variable

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_3_ReadDynamicVariables(const u8 *src, u8 *dst)
{
    s16 Position;
    pres_t Pressure1;
    pres_t Pressure2;
    pres_t MainPressure;
    pres_t PressureSupply;
    u8 presUnitsMain;
    u8 presUnits2;
    u8 presUnitsSupply;
    u8 presUnits;

    const BoardPressure_t* pAllPressures;
    bool_t bPressure1       = cnfg_GetOptionConfigFlag(PRESSURE_SENSOR_1);
    bool_t bPressure2       = cnfg_GetOptionConfigFlag(PRESSURE_SENSOR_2);
    bool_t bPressureSupply  = cnfg_GetOptionConfigFlag(PRESSURE_SUPPLY);

    //"3:Read All Var,<Sig,<PosUnits,<Pos,<PresUnits3,<Pres;"

    //use smoothed position
    //Position = control_GetPosition();
    //Position = smooth_GetSmoothedData(SELECTION_POSITION);
    Position = (pos_t)vpos_GetScaledPosition();
    u8 pos_units = fpconvert_IntToFloatBuffer(Position, UNITSID_POSITION_ENTRY, &dst[HC3_POS]);

    // Setpoint reading
    (void)fpconvert_IntToFloatBuffer(digsp_GetDigitalSetpoint(), UNITSID_PERCENT, &dst[HC3_SIGNAL]);

    //add the data to the response buffer
    dst[HC3_POS_UNITS] = pos_units;

    //process all pressures

    pAllPressures = cycle_GetPressureData();

    // This is to get the units!!!
    presUnits = fpconvert_IntToFloatBuffer(0, UNITSID_PRESSURE, &dst[HC3_TV]);
    presUnitsMain   = presUnits;
    presUnits2      = presUnits;
    presUnitsSupply = presUnits;

    if (bPressure1)
    {
        Pressure1 = pAllPressures->Pressures[PRESSURE_ACT1_INDEX];
        if (Pressure1 == PRESSURE_INVALID)
        {
            Pressure1 = 0;
        }
    }
    else
    {
        Pressure1     = 0;
        presUnitsMain = (u8)HART_NOT_USED;
    }

    if (bPressure2)
    {
        Pressure2 = pAllPressures->Pressures[PRESSURE_ACT2_INDEX];
        if (Pressure2 == PRESSURE_INVALID)
        {
            Pressure2 = 0;
        }
    }
    else
    {
        Pressure2  = 0;
        presUnits2 = (u8)HART_NOT_USED;
    }

    if (bPressureSupply)
    {
        PressureSupply = pAllPressures->Pressures[PRESSURE_SUPPLY_INDEX];
        if (PressureSupply == PRESSURE_INVALID)
        {
            PressureSupply = 0;
        }
    }
    else
    {
        PressureSupply  = 0;
        presUnitsSupply = (u8)HART_NOT_USED;
    }

    //main pressure
    MainPressure = Pressure1 - Pressure2;

    dst[HC3_PRES_UNITS] = presUnitsMain;
    (void)fpconvert_IntToFloatBuffer(MainPressure, UNITSID_PRESSURE, &dst[HC3_PRES]);

    // Supply pressure
    dst[HC3_TV_UNITS] = presUnitsSupply;
    (void)fpconvert_IntToFloatBuffer(PressureSupply, UNITSID_PRESSURE, &dst[HC3_TV]);

    //P2 pressure
    dst[HC3_QV_UNITS] = presUnits2;
    (void)fpconvert_IntToFloatBuffer(Pressure2, UNITSID_PRESSURE, &dst[HC3_QV]);

    UNUSED_OK(src);

    return HART_OK;
} // ----- end of hart_Command_3_ReadDynamicVariables() -----

/* This line marks the end of the source */
