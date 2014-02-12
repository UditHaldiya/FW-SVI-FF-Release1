/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ifman.h
    \brief API for Manager/coordinator of various interfaces to the device

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes/ifman.h $
    $Date: 7/26/11 4:01p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup interface
*/
/* $History: ifman.h $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:01p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes
 * With errcodes.h
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/07/11    Time: 4:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * Lint annotations FBO TFS:6936
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/11    Time: 2:37p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * Exclusive configuration channel support FBO TFS:6455
*/
#ifndef IFMAN_H_
#define IFMAN_H_

#include "errcodes.h"
#include "nvram.h"

typedef enum iflock_t
{
    iflock_none = 0,
    iflock_temp = 1,
    iflock_perm = 2,
    iflock_special = 253 //lint -esym(769,iflock_t::iflock_special) deined by protocol but may remain unused
} iflock_t; //!< internal lock types; may or may not coincide with HART numbers

typedef enum iflock_owner_t
{
    //iflock_owner_none,
    iflock_owner_ui,
    iflock_owner_hart_primary,
    iflock_owner_hart_secondary
} iflock_owner_t; //!< enumeration of known interface channels

typedef struct InterfaceAccess_t
{
    iflock_t iflock;
    iflock_owner_t owner;
    u16 CheckWord;
} InterfaceAccess_t; //!< (Persistent) data structure for exclusive interface locks

LINT_PURE(ifman_GetEffectiveLock)
/** \brief Returns exclusive lock status for an interface channel
\param owner - one of predefined interface channels
\return effective lock(-out) level
*/
extern iflock_t ifman_GetEffectiveLock(iflock_owner_t owner);

LINT_PURE(ifman_IsAccessEnabled)
/** \brief Checks whether write access is enabled by lock
\param owner - one of predefined interface channels
\return true iff not locked out
*/
MN_INLINE bool_t ifman_IsAccessEnabled(iflock_owner_t owner)
{
    return ifman_GetEffectiveLock(owner) == iflock_none;
}

SAFESET(ifman_SetLock, InterfaceAccess_t);
extern const void *TypeUnsafe_ifman_GetLock(void *dst);

#ifdef OLD_NVRAM
extern void ifman_SaveAccessLock(void);
extern void ifman_InitLockData(InitType_t itype);
#endif //OLD_NVRAM

#endif //IFMAN_H_
/* This line marks the end of the source */
