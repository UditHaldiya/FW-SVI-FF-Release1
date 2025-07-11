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
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue/hart_ctllimits.c $
    $Date: 2/25/10 1:00a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_ctllimits.c $
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
#include "mnwrap.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "control.h"
#include "ctllimits.h"
#include "fpconvert.h"
#include "bufutils.h"
//#include "hartpriv.h"

/**
\brief Read position control limits

Notes:
    Only valid in OOS mode
    AK: should change the format of the command to simplify and modularize interface
*/
s8_least hartcmd_WritePositionControlLimits(const u8 *src, u8 *dst)
{
    const Req_WritePositionControlLimits_t *s1 = (const void *)src;
    const ControlLimitsConf_t *s = (const void *)s1->ControlLimitsConf[0];

    Rsp_WritePositionControlLimits_t *d = (void *)dst;
    UNUSED_OK(d->ControlLimitsConf); //rely on read command called below to fill

    CtlLimits_t lims;
    (void)control_GetLimits(&lims); //will replace here

    s8_least x;
    for(x = Xlow; x < Xends; x++)
    {
        lims.EnableSetpointLimit[x] = util_GetU8(s->PositionLimitsEnabled[x]);
        lims.EnableSetpointRateLimit[x] = util_GetU8(s->PositionRateLimitsEnabled[x]);
        lims.SetpointLimit[x] = fpconvert_FloatBufferToInt16Clamped(s->PositionLimits[x], UNITSID_POSITION, spposlimits_range[x]);
        //lims.SetpointLimit[x] = convert_FPositionToStdPosition(util_GetFloat(s->PositionLimits[x]));
        lims.SetpointRateLimit[x] =
          fpconvert_FloatBufferToInt16Clamped(s->PositionRateLimits[0], UNITSID_POSITION, spratelim_range); //only one PositionRateLimit
        //lims.SetpointRateLimit[x] = convert_FPositionToStdPosition(util_GetFloat(s->PositionRateLimits[0]));
    }

    lims.Protected = util_GetU8(s->PositionLimitsProtected[0]);
    lims.EnableTightShutoff[Xlow] = util_GetU8(s->TightShutoffEnabled[0]);
    lims.TightShutoff[Xlow] = fpconvert_FloatBufferToInt16Clamped(s->TightShutOff[0], UNITSID_POSITION, tso_limits);
    //TFS:9578 - Take Full Open/Close out of control
    lims.EnableTightShutoff[Xhi] = util_GetU8(s->TightCutoffEnabled[0]);
    lims.TightShutoff[Xhi] = fpconvert_FloatBufferToInt16Clamped(s->TightCutOff[0], UNITSID_POSITION, tso_limits);

    s8_least ret = err2hart(control_SetLimits(&lims));
    if ( (lims.TightShutoff[Xhi] - lims.TightShutoff[Xlow]) < FIVE_PCT_819 )
    {
        ret = HART_INVALID_SPAN;
    }

    if(ret == HART_NO_COMMAND_SPECIFIC_ERRORS)
    {
        ret = hartcmd_ReadPositionControlLimits(src, dst);
    }
    return ret;
}

/**
\brief Write control limits

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadPositionControlLimits(const u8 *src, u8 *dst)
{
    Rsp_ReadPositionControlLimits_t *d1 = (void *)dst;
    ControlLimitsConf_t *d = (void *)d1->ControlLimitsConf[0];

    UNUSED_OK(src);
    CtlLimits_t lims;
    (void)control_GetLimits(&lims);

    s8_least x;
    for(x = Xlow; x < Xends; x++)
    {
        util_PutU8(d->PositionLimitsEnabled[x], lims.EnableSetpointLimit[x]);
        util_PutU8(d->PositionRateLimitsEnabled[x], lims.EnableSetpointRateLimit[x]);
        //util_PutFloat(d->PositionLimits[x], convert_StdPositionToFPosition(lims.SetpointLimit[x]));
        (void)fpconvert_IntToFloatBuffer(lims.SetpointLimit[x], UNITSID_POSITION, d->PositionLimits[x]);
    }
    //util_PutFloat(d->PositionRateLimits[0], convert_StdPositionToFPosition(lims.SetpointRateLimit[Xlow]));
    (void)fpconvert_IntToFloatBuffer(lims.SetpointRateLimit[Xlow], UNITSID_POSITION, d->PositionRateLimits[0]);

    util_PutU8(d->PositionLimitsProtected[0], lims.Protected);
    util_PutU8(d->TightShutoffEnabled[0], lims.EnableTightShutoff[Xlow]);
    util_PutU8(d->TightCutoffEnabled[0], lims.EnableTightShutoff[Xhi]);
    //util_PutFloat(d->TightShutOff[0], convert_StdPositionToFPosition(lims.TightShutoff[Xlow]));
    (void)fpconvert_IntToFloatBuffer(lims.TightShutoff[Xlow], UNITSID_POSITION, d->TightShutOff[0]);
    (void)fpconvert_IntToFloatBuffer(lims.TightShutoff[Xhi], UNITSID_POSITION, d->TightCutOff[0]);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
