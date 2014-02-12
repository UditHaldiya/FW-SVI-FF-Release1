/**
Copyright 2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file devicemode.h
    \brief friendly interfaces between device mode manager and fault manager

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/devfltpriv.h $
    $Date: 5/16/11 6:34p $
    $Revision: 8 $
    $Author: Arkkhasin $

    \ingroup DeviceMode
*/
/* $History: devfltpriv.h $
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 5/16/11    Time: 6:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:6336: dummy faults necessary for the build moved away from the
 * fault matrix to dummyfaults.h
 *
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 11/24/10   Time: 1:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Added API to set target mode FOB TFS:4845
 * Also, tombstone header updated
*/
#ifndef DEVFLTPRIV_H_
#define DEVFLTPRIV_H_

#include "devicemode.h"
#include "errcodes.h"

//extern void mode_ClearLowPower(void);

#ifdef OLD_DEVMODE
#define mode_PreInit()
#else
extern void mode_PreInit(void);
#endif

MN_DECLARE_API_FUNC(mode_SetTargetMode)
extern ErrorCode_t mode_SetTargetMode(devmode_t devmode);

/** \brief Checks for inconsistent device mode and replaces if bad
*/
extern devmode_t mode_RepairStartupMode(devmode_t mode);

typedef struct ctlmodelist_t
{
    devmode_t mode; //!< device mode to which the list is applicable
    devsubmode_t submode;  //!< device submode to which (after masking operation) the list is applicable
    devsubmode_t submode_mask;  //!< mask for actual device submode to match the .submode
    u8 count; //!< number of entries in the list
    const ctlmode_t *list; //!< the list of compatible modes
} ctlmodelist_t; //!< the type to interface project-dependent compatible modes

extern const ctlmodelist_t ctlmodelist[]; //!< a (possibly hierarchical) list of compatible control modes

extern bool_t mode_IsPersistent(devmode_t mode);

#endif //DEVFLTPRIV_H_
/* This line marks the end of the source */
