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

    $Archive: /MNCB/Dev/FIRMWARE/mn_instrum/profiler/critsect.c $
    $Date: 9/01/09 11:31a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup OSWrap
*/
/* $History: critsect.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/01/09    Time: 11:31a
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * Profiler instrumentation (tasks and critical sections)
*/
#include "mnwrap.h"
#include "mn_coinstrum.h"

CritSectStat_t CritSectStat; //critical sections profiler

/* This line marks the end of the source */
