/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file critsect.c
    \brief MN code instrumentation. Critical sections profilng

    CPU: Any (no instrumentation)

    OWNER: AK

    $Archive: /MNCB/Dev/FIRMWARE/includes/oswrap.h $
    $Date: 3/29/09 12:55a $
    $Revision: 42 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: oswrap.h $
*/
#include "mnwrap.h"
#include "mn_coinstrum.h"

CritSectStat_t CritSectStat; //critical sections profiler

/* This line marks the end of the source */
