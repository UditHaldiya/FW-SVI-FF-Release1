/*
Copyright 2008 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file bumpless.h
    \brief bumpless transfer process support API

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/bumpless.h $
    $Date: 11/17/10 2:20p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup Modeman
*/
/* (Optional) $History: bumpless.h $
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 11/17/10   Time: 2:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4760 - code review issues
 * TFS:4758 - Correct handling of open-loop control modes
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 10/15/10   Time: 2:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4350 - bumpless transfer wrapper
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 5:07p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Bumpless is back but now standalone
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:10p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/06/08    Time: 6:03p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Bumpless transfer got its own configuration extracted from
 * ConfigurationData_t (and HART commands hacked to that effect).
 * bumpless_Correct() fixes device mode if transfer was interrupted by a
 * reset.
 * Full testing delayed waiting for configurable SVI2Assistant
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 6:42p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Bumpless prototypes moved to their own bumpless.h
*/
#ifndef BUMPLESS_H_
#define BUMPLESS_H_
#include "nvram.h"
#include "process.h"

extern procresult_t mode_RunBumpless(s16 *procdetails);

typedef struct BumplessActive_t
{
    u16 Active;
    u16 CheckWord;
} BumplessActive_t;

typedef struct BumplessConf_t
{
    u8 Required;             //0 = optional
    u8 BumplessSpeed; 		//Bumpless transfer
    u16 CheckWord;
} BumplessConf_t;

SAFEGET(bumpless_GetConf, BumplessConf_t);
SAFESET(bumpless_SetConf, BumplessConf_t);

extern ErrorCode_t mode_SetNormalModeEx(void);

#ifdef OLD_NVRAM
extern void bumpless_SaveConf(void);
extern void bumpless_InitConf(InitType_t itype);
#endif //OLD_NVRAM

#endif //BUMPLESS_H_
/* This line marks the end of the source */
