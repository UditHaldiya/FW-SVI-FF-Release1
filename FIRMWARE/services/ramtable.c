/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ramtable.c
    \brief The (possibly, project-dependent) table of RAM areas accessors

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/services/ramtable.c $
    $Date: 5/21/07 11:51a $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup rammap
*/
/* (Optional) $History: ramtable.c $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:51a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * New tombstone header
*/

#include "mnwrap.h"
#include "selftest.h"
#include "ramtable.h"
#include "ramdatamap.h"

ramaccess_t * const ramtable[NUM_RAMIDS] =
{
    [RAMID_TESTSTAT & RAMID_MASK] = selftest_GetStatMap,
};
/* This line marks the end of the source */

