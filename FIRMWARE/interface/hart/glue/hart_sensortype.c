/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_sensortype.c
    \brief HART glue layer for (position) sensor option

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/

#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "adtbl.h"

s8_least hartcmd_ReadSensorType(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadSensorType_t *d = (void *)dst;
    util_PutU8(d->PositionSensorType[0], bool2int(ad_GetSensorType(NULL)->IsRemotePositionSensor));
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteSensorType(const u8 *src, u8 *dst)
{
    Rsp_WriteSensorType_t *d = (void *)dst;
    UNUSED_OK(d->PositionSensorType); //rely on HART framework to fill
    const Req_WriteSensorType_t *s = (const void *)src;

    const SensorType_t st =
    {
        .IsRemotePositionSensor = int2bool(util_GetU8(s->PositionSensorType[0])),
        .CheckWord = 0, //don't care
    };
    ErrorCode_t err =  ad_SetSensorType(&st);

    return err2hart(err);
}
/* This line marks the end of the source */
