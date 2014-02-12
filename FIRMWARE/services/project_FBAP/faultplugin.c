/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file faultplugin.c
    \brief Project-specific handler of fault behaviour attributes

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/project_LCX/faultplugin.c $
    $Date: 9/02/10 2:12p $
    $Revision: 4 $
    $Author: Anatoly Podpaly $

    \ingroup Faultman
*/
/* (Optional) $History: faultplugin.c $
 * 
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 9/02/10    Time: 2:12p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/project_LCX
 * Bug 4063: Device was not switching to Low Power Mode.
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 5/25/10    Time: 2:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/project_LCX
 * Lint (moved prototype)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/09/10    Time: 12:37a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/project_LCX
 * Lint (mostly unused header)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 12:28p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/project_LCX
 * 4.
 * Get services\project_LCX from the trunk
 *     Remove powermncb.c services\project_LCX\conf.mak
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/16/09   Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Light linting
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 5:35p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Failsafe mode set using correct API - mode_SetMode()
 * Removed unused header(s)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 10:06p
 * Updated in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/23/08    Time: 6:34p
 * Created in $/MNCB/Dev/FIRMWARE/services/project_SVI2AP
 * Project-specific handler of fault behaviour attributes
*/
#include "mnwrap.h"
#include "faultpublic.h"
#include "devicemode.h"

/** \brief AP-style support for fault behavior attributes
\param properties - a bitmap of the fault behavior attributes
*/
void error_ProjectPlugin(attribmask_t properties)
{
    /* Other properties must be attended to either below or in a polling
       routine (somewhere e.g., in the cycle task) or both.
       Currently, only mode-changing properties are looked at here.
    */
    if((properties & FATTRIB_FAILSAFE) != FATTRIB_NONE)
    {
        ErrorCode_t err;
#ifdef OLD_DEVMODE
        err = mode_SetOOSMode(SUBMODE_OOS_FAILSAFE);
#else
        err = mode_SetMode(MODE_FAILSAFE);
#endif //OLD_DEVMODE
        UNUSED_OK(err); //cannot fail
    }

    // Check for the LOW POWER
    if((properties & FATTRIB_LOWPWR) != FATTRIB_NONE)
    {
        ErrorCode_t err;
#ifdef OLD_DEVMODE
        err = mode_SetOOSMode(SUBMODE_OOS_LOWPOWER);
#else
        err = mode_SetMode(MODE_LOWPOWER);
#endif //OLD_DEVMODE
        UNUSED_OK(err); //cannot fail
    }
}
/* This line marks the end of the source */

