/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file low_supply_pressure_public.h
    \brief Public interfaces of the Control Module

    CPU: Any

    $Revision: 1 $
    OWNER: JS
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/low_supply_pressure.h $
    $Date: 3/21/11 7:38p $
    $Revision: 1 $
    $Author: Justin Shriver $

    \ingroup monitors
*/
/* $History: low_supply_pressure.h $
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 3/21/11    Time: 7:38p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5914
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 3/11/11    Time: 5:15p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5165 Air loss detection
 *
 * *****************  Version 68  *****************
 * User: Anatoly Podpaly Date: 2/17/11    Time: 3:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5607 -- moved Low Power handling code to SYSIO layer.
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 2:29a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4922 - ctltask.c moved and compiles conditionally
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 10/22/10   Time: 2:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4206 fix - Step 2: (a, also TFS:2514) checksums in rate limits
 * stuff which moved to ctllimits.c (b) removed /some/ compiled-out code
 * (c, FOB TFS:2616) rate limits can be reset to any setpoint
 *
 *
 * *****************  Version 65  *****************
 * User: Arkkhasin    Date: 10/21/10   Time: 5:25p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4206 fix - Step 1: Removed unused (and confusing) mode
 * CONTROL_MANUAL_SIG.
 * Also, standard tomstone header.
*/
#ifndef LOW_SUPPLY_PRESSURE_PUBLIC_H_
#define LOW_SUPPLY_PRESSURE_PUBLIC_H_


/** Public functions
*/
#include "mnwrap.h"
#include "mncbtypes.h"
//#include "strong_types.h"
#include "pneumatics.h"


bool_t mon_checkLowSupplyFromIPandI(const PneumaticParams_t  * const pneumatic_parameters);

#endif //low_supply_pressure_public.h
/* This line marks the end of the source */
