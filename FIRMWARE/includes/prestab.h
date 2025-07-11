/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file prestab.c
    \brief Project-specific pressures tempcomp - header entries

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/prestab.h $
    $Date: 12/07/11 2:24p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: prestab.h $
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 2:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8204 - features for I/O channels again (typos)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:56p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/14/11   Time: 11:51p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 *
*/
#ifndef PRESTAB_H_
#define PRESTAB_H_
#include "tempcomp.h"
#include "projectdef.h"

#if FEATURE_PRESSURE_SENSOR_ATM == FEATURE_PRESSURE_SENSOR_ATM_SUPPORTED
TEMPCOMP_H_ENTRY(presatm);
#endif
#if FEATURE_PRESSURE_SENSOR_PILOT == FEATURE_PRESSURE_SENSOR_PILOT_SUPPORTED
TEMPCOMP_H_ENTRY(prespilot);
#endif
#if FEATURE_PRESSURE_SENSOR_1 == FEATURE_PRESSURE_SENSOR_1_SUPPORTED
TEMPCOMP_H_ENTRY(pres1);
#endif
#if FEATURE_PRESSURE_SENSOR_2 == FEATURE_PRESSURE_SENSOR_2_SUPPORTED
TEMPCOMP_H_ENTRY(pres2);
#endif
#if FEATURE_PRESSURE_SENSOR_3 == FEATURE_PRESSURE_SENSOR_3_SUPPORTED
TEMPCOMP_H_ENTRY(pres3);
#endif

#endif //PRESTAB_H_
/* This line marks the end of the source */
