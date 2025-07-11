/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file tcomptab.c
    \brief Temperature correction index tables for MNCB (AP+ESD)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/tcomptab.c $
    $Date: 12/07/11 1:01p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: tcomptab.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 1:01p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 7:12p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 11:03a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 7:23p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Reduced number of channels in LCX tempcomp
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 10:12p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * MNCB builds repaired
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 6:03p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Renaming to support ao.c changes to ver.6
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/04/08    Time: 8:56p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Position-related headers refactored
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:44p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * Tempreature correction object(s) for the new temperature correction
 * scheme
*/

#include "mnwrap.h"
#include "projectdef.h"
#include "ipcurr.h"
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#include "insignal.h"
#endif
#if FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
#include "inpv.h"
#endif
#include "tempr.h"
#include "tempr_mncb.h"
#include "posint.h"
#if FEATURE_REMOTE_POSITION_SENSOR == FEATURE_REMOTE_POSITION_SENSOR_AVAILABLE
#   include "posext.h"
#endif
#if FEATURE_AO == FEATURE_AO_SUPPORTED
#   include "ao.h"
#endif
#if FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
#   include "pressures.h"
#   include "prestab.h"
#endif
#include "pwmcomp.h"

#include "nvmemids.h"

static const TCompAccess_t BoardTempCompAccess[] =
{
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
    TCOMP_ACCESS_ENTRY(sig),
#endif
#if FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
    TCOMP_ACCESS_ENTRY(inpv),
#endif
    TCOMP_ACCESS_ENTRY(tempr), //yes, board temperature itself is temperature-compensated
#if FEATURE_PRESSURE_SENSOR_ATM == FEATURE_PRESSURE_SENSOR_ATM_SUPPORTED
    TCOMP_ACCESS_ENTRY(presatm),
#endif
#if FEATURE_PRESSURE_SENSOR_3 == FEATURE_PRESSURE_SENSOR_3_SUPPORTED
    TCOMP_ACCESS_ENTRY(pres3),
#endif
#if FEATURE_PRESSURE_SENSOR_2 == FEATURE_PRESSURE_SENSOR_2_SUPPORTED
    TCOMP_ACCESS_ENTRY(pres2),
#endif
#if FEATURE_PRESSURE_SENSOR_1 == FEATURE_PRESSURE_SENSOR_1_SUPPORTED
    TCOMP_ACCESS_ENTRY(pres1),
#endif
#if FEATURE_PRESSURE_SENSOR_PILOT == FEATURE_PRESSURE_SENSOR_PILOT_SUPPORTED
    TCOMP_ACCESS_ENTRY(prespilot),
#endif
#if FEATURE_REMOTE_POSITION_SENSOR == FEATURE_REMOTE_POSITION_SENSOR_AVAILABLE
    TCOMP_ACCESS_ENTRY(posext),
#endif
    TCOMP_ACCESS_ENTRY32(posint),
    TCOMP_ACCESS_ENTRY(ipcurr),
#if FEATURE_AO == FEATURE_AO_SUPPORTED
    TCOMP_ACCESS_ENTRY(ao0), //secondary (retransmit) AO
#endif
    TCOMP_ACCESS_ENTRY(pwmcomp),
};

static u8 BoardTempIndex = NUMBER_OF_CALIB_POINTS/2U;

static const TempComp_t BoardTempComp =
{
    .GetRawTemp = tempr_GetSmoothedRawTemperature,
    .tindex = &BoardTempIndex,
    .numchannels = NELEM(BoardTempCompAccess),
    .ptsId = NVRAMID_TemperatureCalibrationPoints,
    .TAccess = BoardTempCompAccess,
};

const TempComp_t *const TempComp[] =
{
    &BoardTempComp,
    NULL
};
/* This line marks the end of the source */
