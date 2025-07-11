/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_ctllimis.c
    \brief HART glue layer for control (position setpoint) limits

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_pneuparams.c $
    $Date: 1/23/12 2:27p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_pneuparams.c $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 2:27p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Captured but compiled out (incorrectly scaled) FP penumatic params FBO
 * TFS:8762
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 10/08/10   Time: 1:12p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:3984 (Spelling of variables)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/12/10    Time: 11:41a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * HART commands to read/write pneumatic params
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 1:00a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Commands to Read/Write PID parameters moved from hartfunc.c to
 * hart_posctl.c and acquired PID set selector
 * Autotune command re-implemented in XML and moved from hartfunc.c to
 * hart_posctl.c
 * Additional commands implemented in XML
 * Non-existent PID parameters removed from PID read/write commands
 * Added reading PID set selector
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 8:33p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Interfaces extracted from hartfunc.c to correspond to modules and their
 * storage
*/
#include <limits.h>
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "pneumatics.h"
#include "bufutils.h"
#include "fpconvert.h"

/**
\brief Write pneumatic relay paramters
*/
s8_least hartcmd_WritePneumaticParameters(const u8 *src, u8 *dst)
{
    const Req_WritePneumaticParameters_t *s1 = (const void *)src;
    const PneumaticParameters_t *s = (const void *)s1->PneumaticParameters[0];
    Rsp_WritePneumaticParameters_t *d = (void *)dst;
    UNUSED_OK(d->PneumaticParameters); //rely on HART framework to fill

    PneumaticParams_t t;

    t.boostCoeff.boost[Xlow] = util_GetU8(s->BoostThreshold[0]);
    t.boostCoeff.boost[Xhi] = util_GetU8(s->BoostThreshold[1]);
    t.boostCoeff.BoostOffset = (s8)util_GetU8(s->BoostOffset[0]);
    t.presLimitsPilot.presLimit[Xlow] = util_GetS16(s->PilotPressureSaturationLimits[0]);
    t.presLimitsPilot.presLimit[Xhi] = util_GetS16(s->PilotPressureSaturationLimits[1]);
    t.SupplyLossThreshold_Pilot = util_GetS16(s->LowSupplyPressurePilotThreshold[0]);
    t.SupplyLossThreshold_Supply = util_GetS16(s->LowSupplyPressureSupplyThreshold[0]);
    t.SingleActing = util_GetU8(s->SingleActingFlag[0]);
    CONST_ASSERT(NELEM(t.SensorMap) == NELEM(s->PressureSensorMap));
    for(u8_least i=0; i<NELEM(t.SensorMap); i++)
    {
        t.SensorMap[i] = (s8)util_GetU8(s->PressureSensorMap[i]);
    }

    ErrorCode_t err = pneu_SetParams(&t);

    return err2hart(err); //output buffer filled automatically
}


/**
\brief Read pneumatic relay paramters

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadPneumaticParameters(const u8 *src, u8 *dst)
{
    Rsp_ReadPneumaticParameters_t *d1 = (void *)dst;
    PneumaticParameters_t *d = (void *)d1->PneumaticParameters[0];

    UNUSED_OK(src);
    PneumaticParams_t t;
    (void)pneu_GetParams(&t);

    util_PutU8(d->BoostThreshold[0], t.boostCoeff.boost[Xlow]);
    util_PutU8(d->BoostThreshold[1], t.boostCoeff.boost[Xhi]);
    util_PutU8(d->BoostOffset[0], (u8)t.boostCoeff.BoostOffset);
    util_PutS16(d->PilotPressureSaturationLimits[0], t.presLimitsPilot.presLimit[Xlow]);
    util_PutS16(d->PilotPressureSaturationLimits[1], t.presLimitsPilot.presLimit[Xhi]);
    util_PutS16(d->LowSupplyPressurePilotThreshold[0], t.SupplyLossThreshold_Pilot);
    util_PutS16(d->LowSupplyPressureSupplyThreshold[0], t.SupplyLossThreshold_Supply);
    util_PutU8(d->SingleActingFlag[0], t.SingleActing);
    CONST_ASSERT(NELEM(t.SensorMap) == NELEM(d->PressureSensorMap));
    for(u8_least i=0; i<NELEM(t.SensorMap); i++)
    {
        util_PutU8(d->PressureSensorMap[i], (u8)t.SensorMap[i]);
    }

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

#if 0 //capture devel but not unleash. (Yet?)
/**
\brief Read pneumatic relay paramters

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadPneumaticParametersFP(const u8 *src, u8 *dst)
{
    Rsp_ReadPneumaticParametersFP_t *d1 = (void *)dst;
    FPPneumaticParameters_t *d = (void *)d1;

    UNUSED_OK(src);
    PneumaticParams_t t;
    (void)pneu_GetParams(&t);

    fpconvert_IntToFloatBuffer(t.boostCoeff.boost[Xlow], UNITSID_CTLOUT_ENTRY, d->FPBoostThreshold[0]);
    fpconvert_IntToFloatBuffer(t.boostCoeff.boost[Xhi], UNITSID_CTLOUT_ENTRY, d->FPBoostThreshold[1]);
    u8 units = fpconvert_IntToFloatBuffer(t.boostCoeff.BoostOffset, UNITSID_CTLOUT_ENTRY, d->FPBoostOffset[0]);
    util_PutU8(d->ControlOutputUnits[0], units);

    fpconvert_IntToFloatBuffer(t.presLimitsPilot.presLimit[Xlow], UNITSID_PRESSURE_ENTRY, d->FPPilotPressureSaturationLimits[0]);
    fpconvert_IntToFloatBuffer(t.presLimitsPilot.presLimit[Xhi], UNITSID_PRESSURE_ENTRY, d->FPPilotPressureSaturationLimits[1]);
    fpconvert_IntToFloatBuffer(t.SupplyLossThreshold_Pilot, UNITSID_PRESSURE_ENTRY, d->FPLowSupplyPressurePilotThreshold[0]);
    units = fpconvert_IntToFloatBuffer(t.SupplyLossThreshold_Supply, UNITSID_PRESSURE_ENTRY, d->FPLowSupplyPressureSupplyThreshold[0]);
    util_PutU8(d->PressureUnits[0], units);

    util_PutU8(d->SingleActingFlag[0], t.SingleActing);
    CONST_ASSERT(NELEM(t.SensorMap) == NELEM(d->PressureSensorMap));
    for(u8_least i=0; i<NELEM(t.SensorMap); i++)
    {
        util_PutU8(d->PressureSensorMap[i], (u8)t.SensorMap[i]);
    }

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

static const s16 byterange[Xends] = {0, 255};
static const s16 s16range[Xends] = {SHRT_MIN, SHRT_MAX};
/**
\brief Write pneumatic relay paramters
*/
s8_least hartcmd_WritePneumaticParametersinFP(const u8 *src, u8 *dst)
{
    const Req_WritePneumaticParametersinFP_t *s1 = (const void *)src;
    const FPPneumaticParameters_t *s = (const void *)s1->FPPneumaticParameters[0];

    PneumaticParams_t t;
    s16 var;

    if(util_GetU8(s->PressureUnits[0]) != fpconvert_GetUnits(UNITSID_PRESSURE_ENTRY))
    {
        return HART_INVALID_SELECTION;
    }
    if(util_GetU8(s->ControlOutputUnits[0]) != fpconvert_GetUnits(UNITSID_CTLOUT_ENTRY))
    {
        return HART_INVALID_SELECTION;
    }

    var = fpconvert_FloatBufferToInt16Clamped(s->FPBoostThreshold[0], UNITSID_CTLOUT_ENTRY, byterange);
    if((var < byterange[Xlow]) || (var > byterange[Xhi]))
    {
        return HART_INVALID_SELECTION;
    }
    t.boostCoeff.boost[Xlow] = (u8)var;

    var = fpconvert_FloatBufferToInt16Clamped(s->FPBoostThreshold[1], UNITSID_CTLOUT_ENTRY, byterange);
    if((var < byterange[Xlow]) || (var > byterange[Xhi]))
    {
        return HART_INVALID_SELECTION;
    }
    t.boostCoeff.boost[Xhi] = (u8)var;

    var = fpconvert_FloatBufferToInt16Clamped(s->FPBoostOffset[0], UNITSID_CTLOUT_ENTRY, byterange);
    if((var < byterange[Xlow]) || (var > byterange[Xhi]))
    {
        return HART_INVALID_SELECTION;
    }
    t.boostCoeff.BoostOffset = (u8)var;

    var = fpconvert_FloatBufferToInt16Clamped(s->FPPilotPressureSaturationLimits[0], UNITSID_PRESSURE_ENTRY, s16range);
    t.presLimitsPilot.presLimit[Xlow] = var;
    var = fpconvert_FloatBufferToInt16Clamped(s->FPPilotPressureSaturationLimits[1], UNITSID_PRESSURE_ENTRY, s16range);
    t.presLimitsPilot.presLimit[Xhi] = var;


    var = fpconvert_FloatBufferToInt16Clamped(s->FPLowSupplyPressurePilotThreshold[0], UNITSID_PRESSURE_ENTRY, s16range);
    t.SupplyLossThreshold_Pilot = var;
    var = fpconvert_FloatBufferToInt16Clamped(s->FPLowSupplyPressureSupplyThreshold[0], UNITSID_PRESSURE_ENTRY, s16range);
    t.SupplyLossThreshold_Supply = var;

    t.SingleActing = util_GetU8(s->SingleActingFlag[0]);
    CONST_ASSERT(NELEM(t.SensorMap) == NELEM(s->PressureSensorMap));
    for(u8_least i=0; i<NELEM(t.SensorMap); i++)
    {
        t.SensorMap[i] = (s8)util_GetU8(s->PressureSensorMap[i]);
    }

    ErrorCode_t err = pneu_SetParams(&t);
    if(err != ERR_OK)
    {
        return err2hart(err); //output buffer filled automatically
    }
    return hartcmd_ReadPneumaticParametersFP(src, dst);

}
#endif //0

/* This line marks the end of the source */
