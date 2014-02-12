/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_hwuisimple.c
    \brief HART glue layer for MNCB-style data smoothing **coefficients**

    Uses old API; see devel trunk for implementation for new API

    CPU: Any

    OWNER: AK (but no real support except bugs)
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_mncbsmoothing.c $
    $Date: 11/04/11 5:01p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_mncbsmoothing.c $
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 5:01p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/15/10    Time: 12:09a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * 130/138-cum-100,101,102  in xml (TFS:4067)
 * Added 129 read counterparts (TFS:4120)
*/

#include "mnwrap.h"
#include "oswrap.h"
#include "mncbtypes.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "smoothing.h"

s8_least hartcmd_WritePositionSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_WritePositionSmoothingCoefficient_t *s = (const void *)src;
    FilterCoefficients_t FilterCoefficients;
    MN_ENTER_CRITICAL();
        FilterCoefficients = *smooth_GetFilter(NULL);
    MN_EXIT_CRITICAL();
    FilterCoefficients.PositionShiftConstant = util_GetU8(s->SmoothCoefPosition[0]);
    ErrorCode_t err = smooth_SetFilter(&FilterCoefficients);
    return err2hart(err);
}

s8_least hartcmd_ReadPositionSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadPositionSmoothingCoefficient_t *d = (void *)dst;
    util_PutU8(d->SmoothCoefPosition[0], smooth_GetFilter(NULL)->PositionShiftConstant);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteSignalSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_WriteSignalSmoothingCoefficient_t *s = (const void *)src;
    FilterCoefficients_t FilterCoefficients;
    MN_ENTER_CRITICAL();
        FilterCoefficients = *smooth_GetFilter(NULL);
    MN_EXIT_CRITICAL();
    FilterCoefficients.SignalShiftConstant = util_GetU8(s->SmoothCoefSignal[0]);
    ErrorCode_t err = smooth_SetFilter(&FilterCoefficients);
    return err2hart(err);
}

s8_least hartcmd_ReadSignalSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadSignalSmoothingCoefficient_t *d = (void *)dst;
    util_PutU8(d->SmoothCoefSignal[0], smooth_GetFilter(NULL)->SignalShiftConstant);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


s8_least hartcmd_WriteTemperatureSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_WriteTemperatureSmoothingCoefficient_t *s = (const void *)src;
    FilterCoefficients_t FilterCoefficients;
    MN_ENTER_CRITICAL();
        FilterCoefficients = *smooth_GetFilter(NULL);
    MN_EXIT_CRITICAL();
    FilterCoefficients.TemperatureShiftConstant = util_GetU8(s->SmoothCoefTempr[0]);
    ErrorCode_t err = smooth_SetFilter(&FilterCoefficients);
    return err2hart(err);
}

s8_least hartcmd_ReadTemperatureSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadTemperatureSmoothingCoefficient_t *d = (void *)dst;
    util_PutU8(d->SmoothCoefTempr[0], smooth_GetFilter(NULL)->TemperatureShiftConstant);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
