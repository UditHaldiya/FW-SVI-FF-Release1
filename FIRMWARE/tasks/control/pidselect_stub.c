/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pidselect_stub.c
    \brief A stub returning auto-tunable PID paramters set as the current set

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control/pidselect_stub.c $
    $Date: 3/26/10 6:22p $
    $Revision: 3 $
    $Author: Anatoly Podpaly $

    \ingroup control
*/
/* $History: pidselect_stub.c $
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 3/26/10    Time: 6:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Changed the name of the function.
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 3/19/10    Time: 10:36a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Added a call to UI function to return an actual PID slot index.
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 1:01a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * The minimum stub for PID params set selector
*/
#include "mnwrap.h"
#include "selftune.h"
#include "uipublic.h"
/** \brief A stub to return the current PID parameters index.
Must be replaced with real implementation, e.g. with
- selector set via LCD UI or HART, or
- by reading LCX rotary switch via BIOS interface, or...
\return CTLPARAMSET_AUTOTUNE (index of auto-tunable set)
*/
u8 posctl_GetCtlSetIndex(void)
{
    u8          retval;

    retval = UI_Get_PIDIndex();

    return retval;
}

/* This line marks the end of the source */
