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
    \brief System-level data and services related to extrnal position measurement

1. Sensor-specific raw data compensation
2. Sensor-specific linearization

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/posext.c $
    $Date: 11/15/11 10:19a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: posext.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:19a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for position calculations
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 8:16p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * Measurement techology-specific interfaces (channel functions) between
 * BIOS and position.c
*/
#include "mnwrap.h"
#include "posext.h"
#include "position.h"

//remote pot conversions
#define REMOTE_POT_HIGH 25000.0F
#define MAX_POT_ROTATION 120.0F  //in degrees
#define MAX_POT_OHMS 1000.0F     //pot resistance in ohms
//STD_FROM_OHMS assumes that 0 is in the center of the pot (MAX_POT_OHMS/2.0F) and the scale is symetric on both sides
#define STD_FROM_OHMS(x) ((s16)(((REMOTE_POT_HIGH*(x))/(MAX_POT_OHMS/2.0F)) - REMOTE_POT_HIGH)) //convert remote pot ohms to counts
#define STD_FROM_DEGREES_REMOTE_POT(x) ((s16)(STD_FROM_OHMS(MAX_POT_OHMS*((x)/MAX_POT_ROTATION))))


TEMPCOMP_C_ENTRY(posext) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_DEGREES_REMOTE_POT(2.5F),
        [Xhi] = STD_FROM_DEGREES_REMOTE_POT(MAX_POT_ROTATION-2.5F),
    },
    .fcode = FAULT_POSITION_SENSOR,
    .ErrLimit = ERRLIMIT_DEFAULT,
};

void posext_GetInstantTempCompParams(OffsetSpanS32_t *curveseg)
{
    MN_ENTER_CRITICAL();
        *curveseg = posext_TemprCorrRT.curveseg;
    MN_EXIT_CRITICAL();
}


/** \brief A mandatory input channel function
\param adval - raw A/D read of internal (Hall) position sensor
*/
void posext_Update(ADRaw_t adval)
{
    HardPos_t pos = posext_TempComp(adval);
    pos_UpdateState(pos, ERR_OK); //no error detection here
}

/* This line marks the end of the source */
