/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ifman.c
    \brief Manager/coordinator of various interfaces to the device

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/ifman.c $
    $Date: 7/26/11 4:37p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup interface
*/
/* $History: ifman.c $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:37p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface
 * Temporarily,lock doesn't store data in NVRAM until NVRAM is repaired
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 6/08/11    Time: 12:58p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface
 * More robust ifman_InitLockData
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/11    Time: 2:37p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface
 * Exclusive configuration channel support FBO TFS:6455
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "ifman.h"
#include "oswrap.h"
#include "mnassert.h"
#include "hart.h"
#include "faultpublic.h"

static InterfaceAccess_t InterfaceAccess;
static const InterfaceAccess_t InterfaceAccess_Default =
{
    .iflock = iflock_none,
    .owner = iflock_owner_ui,
    .CheckWord = 0, //don't care
};

iflock_t ifman_GetEffectiveLock(iflock_owner_t owner)
{
    iflock_t lock;
    if(hart_GetHartData()->hart_version == HART_REV_5)
    {
        lock = iflock_none;
    }
    else
    {
        if(InterfaceAccess.owner == owner)
        {
            lock = iflock_none;
        }
        else
        {
            lock = InterfaceAccess.iflock;
        }
    }
    return lock;
}



ErrorCode_t TypeUnsafe_ifman_SetLock(const void *src)
{
    const InterfaceAccess_t *ia = src;
    if(ia == NULL)
    {
        ia = &InterfaceAccess_Default;
    }

    switch(ia->iflock)
    {
        case iflock_none:
        case iflock_temp:
        case iflock_perm:
            break;
        default:
            return ERR_INVALID_PARAMETER;
    }
    switch(ia->owner)
    {
        case iflock_owner_ui:
        case iflock_owner_hart_primary:
        case iflock_owner_hart_secondary:
            break;
        default:
            return ERR_INVALID_PARAMETER;
    }
    Struct_Copy(InterfaceAccess_t, &InterfaceAccess, ia);

    ErrorCode_t err;
    if(ia->iflock == iflock_temp)
    {
        //Don't write to NVRAM
        err = ERR_OK;
    }
    else
    {
#ifdef OLD_NVRAM
        if(oswrap_IsOSRunning())
        {
            ifman_SaveAccessLock();
        }
        err = ERR_OK;
#else
        err = ram2nvramAtomic(NVRAMID_InterfaceAccess);
#endif
    }
    return err;
}

const void *TypeUnsafe_ifman_GetLock(void *dst)
{
    return STRUCT_GET(&InterfaceAccess, dst);
}

#ifdef OLD_NVRAM
void ifman_SaveAccessLock(void)
{
    MN_FRAM_ACQUIRE();
#ifdef NVRAMID_InterfaceAccess
        ram2nvram(&InterfaceAccess, NVRAMID_InterfaceAccess);
#endif
    MN_FRAM_RELEASE();
}


/** \brief A standard initializer for old NVRAM
*/
void ifman_InitLockData(InitType_t itype)
{
    MN_ASSERT(!oswrap_IsOSRunning()); //and no FRAM mutex
    const InterfaceAccess_t *pia;
#ifdef NVRAMID_InterfaceAccess
    InterfaceAccess_t ia;
    if(itype == INIT_FROM_NVRAM)
    {
        nvram2ram(&ia, NVRAMID_InterfaceAccess);
        pia = &ia;
    }
    else
#endif
    {   /* default init */
        pia = NULL;
    }
    ErrorCode_t err = ifman_SetLock(pia);
    if(err != ERR_OK)
    {
        error_SetFault(FAULT_NVM_CHECKSUM);
    }
}

#endif //OLD_NVRAM
/* This line marks the end of the source */
