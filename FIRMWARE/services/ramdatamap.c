/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup rammap RAM Data Map module
\brief A small module facilitating accessing raw RAM by ids

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Raw RAM Access Module.doc"> Design document </a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/ramdatamapHTML.html"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file ramdatamap.c
    \brief RAM map API implementation for accessing raw RAM by ids

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/services/ramdatamap.c $
    $Date: 5/21/07 11:51a $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup rammap
*/
/* (Optional) $History: ramdatamap.c $
 * 
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:51a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * New tombstone header
*/
#include "mnwrap.h"
#include "ramtable.h"
#include "ramdatamap.h"

/** \brief A public interface to retrieve RAM map
\param id - project-independent RAM id
\param[out] offset - a pointer to RAM offset holder
\param[out] length - a pointer to RAM entry length holder
\return true iff there is an entry.
*/
bool_t ram_GetMapEntry(u32 id, u16 *offset, u16 *length)
{
    u16 off, len;
    id &= RAMID_MASK;
    if(id >= NUM_RAMIDS)
    {
        return false;
    }
    if(ramtable[id] == NULL)
    {
        return false;
    }
    ramtable[id](&off, &len);
    if(offset != NULL)
    {
        *offset = off;
    }
    if(length != NULL)
    {
        *length = len;
    }
    return true;
}
/* This line marks the end of the source */
