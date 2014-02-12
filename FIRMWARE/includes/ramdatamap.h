/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ramdatamap.h
    \brief RAM map API for accessing raw RAM by ids

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/includes/ramdatamap.h $
    $Date: 5/21/07 11:51a $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup rammap
*/
/* (Optional) $History: ramdatamap.h $
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:51a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New tombstone header
*/
#ifndef RAMDATAMAP_H_
#define RAMDATAMAP_H_

extern bool_t ram_GetMapEntry(u32 id, u16 *offset, u16 *length);
#define RAMID_MASK 0x7fffU //!< RAM data ids (may) have bit 15 set
#define RAMID_FLAG 0x8000U //!< indicator or RAM (as opposed to NVMEM) id

//Project-independent ids
#define RAMID_TESTSTAT (0U | RAMID_FLAG) //!< self-test statistics
#define NUM_RAMIDS 1U //since RAM ids are unique, NUM_RAMIDS is project-independent

#endif //RAMDATAMAP_H_
/* This line marks the end of the source */
