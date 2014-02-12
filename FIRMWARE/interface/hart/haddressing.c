/*
Copyright 2004-2005 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file haddressing.c
    \brief HART protocol - test for acceptable addressing

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart/haddressing.c $
    $Date: 7/08/11 6:10p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup HARTDLL
*/
/* $History: haddressing.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 7/08/11    Time: 6:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6718 Lint
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 6/30/11    Time: 1:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6880,6883 - (internal) HART revisions enumerated from highest down.
 * TFS:6876,6402 - A mechanism to "undefine" a HART 5 command in HART 6
 * Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/27/11    Time: 2:49p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * Standalone addressing checker for HART STX frames
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hart.h"

//NULL-terminated list of supported dispatchers
//Made possible by requirement 6880
const hartdipfunc_t * const hartdisptab[] =
{
    [HART_REV_5] = hart_Dispatch,
    [HART_REV_6] = hart6_Dispatch,
    [HART_REVS_NUMBER] = NULL
};

const hartburstdispfunc_t * const hartburstdisptab[] =
{
    [HART_REV_5] = hart_BurstDispatch,
    [HART_REV_6] = hart6_BurstDispatch,
    [HART_REVS_NUMBER] = NULL
};


// 0-terminated lists of allowed commands (offset by 1)
static const u8 hart_address_not4us_commands[] = {0};
static const u8 hart_address_broadcast_commands[] = {11+1, 21+1, 73+1, 75+1, 0};
static const u8 hart_address_short_commands[] = {0+1, 0};

static const u8 * const HartAllowedAddressing[] =
{
    [hart_address_long] = hart_address_not4us_commands, //don't care
    [hart_address_broadcast] = hart_address_broadcast_commands,
    [hart_address_not4us] = hart_address_not4us_commands,
    [hart_address_short] = hart_address_short_commands,
};

//Description in header
bool_t hart_TestStxAddressing(u8_least cmd, hart_address_t hart_address_type)
{
    if(hart_address_type == hart_address_long)
    {
        return true;
    }

    if((size_t)hart_address_type >= NELEM(HartAllowedAddressing))
    {
        return false;
    }
    const u8 *p = HartAllowedAddressing[hart_address_type];
    for(; *p != 0; p++)
    {
        if(*p == (cmd+1))
        {
            return true;
        }
    }
    return false;
}

