/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/

/**
    \file osadjust.h
    \brief Declares functions used by several other modules. Focus: OS adjustment.

    CPU: Any

    OWNER: Sergey

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/osadjust.h $
    $Date: 12/09/11 1:52p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup Process
*/
/* $History: osadjust.h $
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 1:52p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8370 - header untangling - no code change
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 1:15p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 2  *****************
 * User: Sergey Kruss Date: 1/05/11    Time: 2:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5241-osadj_RunOpenStopAdjustment() should return type void.
 * TFS:4293-Initial SP should be set to current position instead of Normal
 * Mode SP
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 10/26/10   Time: 2:24p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4294  - OSadj moved from calib.c to osadjust.c
 * TFS:4288 - OS Adj is called as a "normal" process via proc table.
 * TFS:4287 - OS Adj must fail if cannot disable limits.
*/

#ifndef OSADJUST_H_
#define OSADJUST_H_

#include "position.h"

#define OS_ADJ_MIN_OVERRUN (-10.0F) //-10% overrun creates 100% max travel
#define OS_ADJ_MAX_OVERRUN 80.0F    //80% overrun creates 190% max travel

#define OPEN_STOP_ADJ_MIN_INT_EXT (OPEN_STOP_ADJ_MIN_INT/2)
#define OPEN_STOP_ADJ_MAX_INT_EXT (OPEN_STOP_ADJ_MAX_INT*2)
#define OS_ADJ_MIN_INT_OVERRUN  ((s32)((((cdouble_t)OS_ADJ_MIN_OVERRUN*(cdouble_t)STANDARD_RANGE)/100.0)-0.5))
#define OS_ADJ_MAX_INT_OVERRUN  ((s32)((((cdouble_t)OS_ADJ_MAX_OVERRUN*(cdouble_t)STANDARD_RANGE)/100.0)+0.5))


typedef struct OSAdjConf_t
{
    percent_t OSAdjOverrun;
    u16 CheckWord;
} OSAdjConf_t;

extern const OSAdjConf_t *osadj_GetConf(OSAdjConf_t *dst);
extern ErrorCode_t osadj_SetConf(const OSAdjConf_t *src);
UNTYPEACC(osadj_SetConf, osadj_GetConf);

extern  void osadj_RunOpenStopAdjustment(void);  /** TFS:5241 */
/** TFS:4293 -- OS Adj SP is initiated to current position (instead of Normal Mode SP)  */

#endif

/* end of the source */

