/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_errlimits.c
    \brief HART interfaces for error limits configurations

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/interface/hart/glue/hart_errlimits.c $
    $Date: 3/30/11 1:39p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History $
*/
#include "mnwrap.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "fpconvert.h"
#include "bufutils.h"
#include "hartpriv.h"
#include "errlimits.h"

/**
\brief Save the error limits data

Notes:
    Only valid in OOS mode
    AK: should change the format of the command to simplify and modularize interface
*/
s8_least hartcmd_WriteErrorLimits(const u8 *src, u8 *dst)
{
    const Req_WriteErrorLimits_t *s1 = (const void *)src;
    const PositionErrorLimits_t *s = (const void *)s1->PositionErrorLimits[0];
    ErrorLimits_t ErrorLimits;
    (void)pos_GetErrorLimits(&ErrorLimits);

    //get the data
    ErrorLimits.PositionErrorBand = fpconvert_FloatBufferToInt16Clamped(s->PositionErrorBand[0], UNITSID_POSITION, errband_range);
    ErrorLimits.bPosErr1Enable = util_GetU8(s->PositionError1Enable[0]);
    ErrorLimits.PositionTime1 = fpconvert_FloatBufferToInt16Clamped(s->PositionTime1[0], UNITSID_MNTIME, errtime_range);
    ErrorLimits.NearPosition = fpconvert_FloatBufferToInt16Clamped(s->NearPosition[0], UNITSID_POSITION, nearpos_range);

    s8_least ret = err2hart(pos_SetErrorLimits(&ErrorLimits));

    if(ret == HART_NO_COMMAND_SPECIFIC_ERRORS)
    {
        Rsp_WriteErrorLimits_t *d = (void *)dst;
        UNUSED_OK(d->PositionErrorLimits); //rely on the read function to fill
        ret = hartcmd_ReadErrorLimits(src, dst);
    }
    return ret;
}

/**
\brief Returns the error limit value and timeouts

Notes:
This command is allowed in all modes, even when write busy or process busy is set
AK: TODO: Change the command format to match data
*/
s8_least hartcmd_ReadErrorLimits(const u8 *src, u8 *dst)
{
    Rsp_ReadErrorLimits_t *d1 = (void *)dst;
    PositionErrorLimits_t *d = (void *)d1->PositionErrorLimits[0];
    ErrorLimits_t ErrorLimits;

    UNUSED_OK(src);
    (void)pos_GetErrorLimits(&ErrorLimits);

    (void)fpconvert_IntToFloatBuffer(ErrorLimits.PositionErrorBand, UNITSID_POSITION, d->PositionErrorBand[0]);
    (void)fpconvert_IntToFloatBuffer(ErrorLimits.NearPosition, UNITSID_POSITION, d->NearPosition[0]);
    util_PutU8(d->PositionError1Enable[0], ErrorLimits.bPosErr1Enable);
    (void)fpconvert_IntToFloatBuffer(ErrorLimits.PositionTime1, UNITSID_MNTIME, d->PositionTime1[0]);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
