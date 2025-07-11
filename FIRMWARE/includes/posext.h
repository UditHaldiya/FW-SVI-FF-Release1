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
    \brief API for System-level data and services related to extrnal position measurement

1. Sensor-specific raw data compensation
2. Sensor-specific linearization

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/posext.h $
    $Date: 11/15/11 10:25a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: posext.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:25a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 8:17p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Measurement techology-specific interfaces (channel functions) between
 * BIOS and position.c
*/
#ifndef POSEXT_H_
#define POSEXT_H_
#include "tempcomp.h"

extern void posext_Update(ADRaw_t adval);
TEMPCOMP_H_ENTRY(posext);

MN_DECLARE_API_FUNC(posext_GetInstantTempCompParams)
extern void posext_GetInstantTempCompParams(OffsetSpanS32_t *curveseg);

#endif //POSEXT_H_
/* This line marks the end of the source */
