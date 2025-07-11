/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ramtable.h
    \brief A private header to expose project-dependent table to the project-
            independent API function

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/services/ramtable.h $
    $Date: 5/21/07 11:51a $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup rammap
*/
/* (Optional) $History: ramtable.h $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:51a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * New tombstone header
*/
#ifndef RAMTABLE_H_
#define RAMTABLE_H_

typedef void ramaccess_t(u16 *offset, u16 *length);
extern ramaccess_t * const ramtable[];

#endif //RAMTABLE_H_
/* This line marks the end of the source */
