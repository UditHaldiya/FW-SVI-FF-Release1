/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file logfpriv.h
    \brief Private interfaces of the Log File module.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/nvram/logfpriv.h $
    $Date: 4/15/09 11:16p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup logfile
*/
/* $History: logfpriv.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 11:16p
 * Created in $/MNCB/Dev/FIRMWARE/nvram
 * Log file private definitions extracted from nvmempriv.h
*/
#ifndef LOGFPRIV_H_
#define LOGFPRIV_H_
//extent definitions (to access them extents by indexing with the extent type: 0BASE, 1FIRSTDIFFERENCE, etc)
typedef struct extdef_t
{
    u16 base_addr; //!< base NVMEM address of the first extent of this type
    u16 length; //!< length of an extent of this type
    u8 count; //!< number of extents of this type reserved in NVMEM
} extdef_t;
extern const extdef_t extdef[];

typedef u8 logdef_t; //! a type combining number of versions and # of extent types required PER VERSION
#define NumVersions(def) ((def) & 0x0fU) //! number of versions to keep
#define NumExtents(def) ((def)>>4) //! number of extents in a log
#define LogDef(nvers, nexts) (((nexts)<<4)|NumVersions(nvers))
extern const logdef_t logdef[];

typedef struct logfEvent_t
{
    faultcode_t newdata; //!< Event to set on successful close
    faultcode_t dataerr; //!< Event to set on bad save attempt
} logfEvent_t;

extern const logfEvent_t logf_EventTable[]; //! Project-provided table of faults per file id to set clear on newest write/read
#endif //LOGFPRIV_H_
/* This line marks the end of the source */
