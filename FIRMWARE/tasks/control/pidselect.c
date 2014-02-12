/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pidselect.c
    \brief Function that returns auto-tunable PID paramters set as the current set

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/control/pidselect.c $
    $Date: 1/20/12 10:54a $
    $Revision: 6 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* $History: pidselect.c $
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:54a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * Header resolution FBO TFS:8748. No code change
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/11/11    Time: 1:37a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:5841 Missing fram mutex sandwich; Don't save RAM to FRAM  on init
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/06/10   Time: 6:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Still TFS:4956 (forgot to save to NVMEM for OLD_NVRAM)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/06/10   Time: 2:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:4956 - app level part (UI remains) LCX is broken until it is
 * completed.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 3/30/10    Time: 2:45p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * The correct implementation.
 * Function returns PID Parameter index.
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
#include "dimensions.h"
#include "nvram.h"
#include "crc.h"
#ifdef OLD_NVRAM
#include "oswrap.h"
#endif

static const CtlSetSlot_t ctlset_default =
{
    .slot = CTLPARAMSET_AUTOTUNE,
    .CheckWord = CRC_SEED
};

static CtlSetSlot_t ctlset;

const void *TypeUnsafe_posctl_GetCtlset(void *dst)
{
    return STRUCT_TESTGET(&ctlset, dst);
}

ErrorCode_t TypeUnsafe_posctl_SetCtlset(const void *src)
{
    ErrorCode_t err;
    const CtlSetSlot_t *p = src;
    if(p == NULL)
    {
        p = &ctlset_default; //default init
    }
    if(p->slot >= NUM_POSCTL_PARAMSETS)
    {
        err = ERR_INVALID_PARAMETER;
    }
    else
    {
        Struct_Copy(CtlSetSlot_t, &ctlset, p);
        err = ram2nvramAtomic(NVRAMID_CtlSetSlot);
#ifdef OLD_NVRAM
        if(oswrap_IsOSRunning())
        {
            posctl_Save();
        }
#endif
    }
    return err;
}


/** \brief Returns the current PID parameters index.
    \return u8 - PID Parameter index of auto-tunable set
*/
u8 posctl_GetCtlSetIndex(void)
{
    return posctl_GetCtlset(NULL)->slot;
}


#ifdef OLD_NVRAM
//NVRAM Shims
void posctl_Init(InitType_t itype)
{
    CtlSetSlot_t t;
    CtlSetSlot_t *p;
    if(itype==INIT_FROM_NVRAM)
    {
        nvram2ram(&t, NVRAMID_CtlSetSlot);
        p = &t;
    }
    else
    {
        p = NULL;
    }
    (void)posctl_SetCtlset(p);
}

void posctl_Save(void)
{
    Struct_Test(CtlSetSlot_t, &ctlset);
    MN_FRAM_ACQUIRE();
        ram2nvram(&ctlset, NVRAMID_CtlSetSlot);
    MN_FRAM_RELEASE();
}

#endif


/* This line marks the end of the source */
