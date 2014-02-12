/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file pressmon_sys.c
    \brief Interface to system needs

     CPU: Any

    $Revision: 6 $

*/

#include "mnwrap.h"
#include "errcodes.h"
#include "pneumatics.h"
//lint -e766
//#define IN_ActuatorOutputMonitors
#include "pressmon_sys.h"
#include "pressmon_sub.h"

//xlint -e960
//#undef IN_ActuatorOutputMonitors
//xlint +e960




bool_t isSingleActing(void)
{
    return pneu_IsSingleActing();
}

bool_t isLowCost(void)
{
    return((cnfg_GetOptionConfigDashVersion()==LOW_COST_VERSION));
}


