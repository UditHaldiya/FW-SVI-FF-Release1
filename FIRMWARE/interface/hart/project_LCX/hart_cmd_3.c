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
    \brief The functions used by HART command 3

    CPU: Any

    OWNER: LS
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/hartfunc.c $
    $Date: 1/30/12 2:19p $
    $Revision: 218 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/

#include "projectdef.h"
#include "diagnosticsUniversal.h"
#include "pressmon.h"
#include "oswrap.h"
#include "mnassert.h"
#include "mncbtypes.h"
#include "nvram.h"
#include "utility.h"
#include "conversion.h"
#include "devicemode.h"
#include "faultpublic.h"
#include "configure.h"
#include "cycle.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hart.h"
#include "smoothing.h"
#include "mncbdefs.h"
#include "sysio.h"

#include "hartdef.h"
#include "hartpriv.h"

#include "poscharact.h"

#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#include "signalsp.h"
#endif

#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#include "uihw_test.h"
#endif

#include "dohwif.h" //for CMD 200 rework

#include "fpconvert.h"

#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#include "insignal.h"
#endif

#include "hartapputils.h"

/**
\brief Returns the signal, position, pressure, and if controller the setpoint and process variable

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_3_ReadDynamicVariables(const u8 *src, u8 *dst)
{
    s16 Position;
    s16 RawSignal;
    float32 fPressure1;
    float32 fPressure2;
    float32 fMainPressure;
    float32 fPressureSupply;
    u8 presUnitsMain;
    u8 presUnits2;
    u8 presUnitsSupply;
#if 0
    const ConfigurationData_t*  pConfigurationData;
    const BoardPressure_t* pAllPressures;
    bool_t bPressure1 = cnfg_GetOptionConfigFlag(PRESSURE_SENSOR_1);
    bool_t bPressure2 = cnfg_GetOptionConfigFlag(PRESSURE_SENSOR_2);
    bool_t bPressureSupply = cnfg_GetOptionConfigFlag(PRESSURE_SUPPLY);
#endif
    //"3:Read All Var,<Sig,<PosUnits,<Pos,<PresUnits3,<Pres;"

    //use smoothed position
    //Position = control_GetPosition();
    //Position = smooth_GetSmoothedData(SELECTION_POSITION);
    Position = (pos_t)vpos_GetSmoothedScaledPosition();
    u8 pos_units = fpconvert_IntToFloatBuffer(Position, UNITSID_POSITION_ENTRY, &dst[HC3_POS]);

    //use smoothed signal
    //get and convert signal
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
    RawSignal = sig_GetSmoothedSignal();
#else
    RawSignal = 0; //RED_FLAG fixme
#endif

    (void)fpconvert_IntToFloatBuffer(RawSignal, UNITSID_SIGNAL_ENTRY, &dst[HC3_SIGNAL]);

    //add the data to the response buffer
    dst[HC3_POS_UNITS] = pos_units;

    //process all pressures

#if 0
    pAllPressures = cycle_GetPressureData();
    pConfigurationData =  cnfg_GetConfigurationData();
    if ( bPressure1 )
    {
        fPressure1 = cnfg_PressureToFPressure(pAllPressures->Pressures[PRESSURE_ACT1_INDEX]);
        presUnitsMain = pConfigurationData->PresUnits;
    }
    else
#endif
    {
        fPressure1 = 0.0F;
        presUnitsMain = (u8)HART_NOT_USED;
    }
#if 0
    if ( bPressure2 )
    {
        fPressure2 = cnfg_PressureToFPressure(pAllPressures->Pressures[PRESSURE_ACT2_INDEX]);
        presUnits2 = pConfigurationData->PresUnits;
    }
    else
#endif
    {
        fPressure2 = 0.0F;
        presUnits2 = (u8)HART_NOT_USED;
    }
#if 0
    if ( bPressureSupply )
    {
        fPressureSupply = cnfg_PressureToFPressure(pAllPressures->Pressures[PRESSURE_SUPPLY_INDEX]);
        presUnitsSupply = pConfigurationData->PresUnits;
    }
    else
#endif
    {
        fPressureSupply = 0.0F;
        presUnitsSupply = (u8)HART_NOT_USED;
    }

    fMainPressure = fPressure1 - fPressure2;

    //main pressure
    dst[HC3_PRES_UNITS] = presUnitsMain;
    util_PutFloat(&dst[HC3_PRES], fMainPressure);

    //supply pressure
    dst[HC3_TV_UNITS] = presUnitsSupply;
    util_PutFloat(&dst[HC3_TV], fPressureSupply);

    //P2 pressure
    dst[HC3_QV_UNITS] = presUnits2;
    util_PutFloat(&dst[HC3_QV], fPressure2);

    UNUSED_OK(src);

    return HART_OK;
} // ----- end of hart_Command_3_ReadDynamicVariables() -----

/* This line marks the end of the source */
