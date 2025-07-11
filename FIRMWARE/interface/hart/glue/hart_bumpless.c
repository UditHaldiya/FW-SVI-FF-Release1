/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_ctllimis.c
    \brief HART glue layer for bumpless transfer

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue/hart_bumpless.c $
    $Date: 3/30/10 5:05p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_bumpless.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 5:05p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Bumpless is back but now standalone
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "bumpless.h"

/**
\brief Write bumpless transfer config
*/
s8_least hartcmd_WriteBumplessTransferConf(const u8 *src, u8 *dst)
{
    const Req_WriteBumplessTransferConf_t *s1 = (const void *)src;
    const BumplessTransferConf_t *s = (const void *)s1->BumplessTransferConf[0];
    UNUSED_OK(dst);

    BumplessConf_t t;

    t.Required = util_GetU8(s->BumplessRequired[0]);
    t.BumplessSpeed = util_GetU8(s->BumplessSpeed[0]);
    ErrorCode_t err = bumpless_SetConf(&t);

    return err2hart(err); //output buffer filled automatically
}
/**
\brief Read bumpless transfer config

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadBumplessTransferConf(const u8 *src, u8 *dst)
{
    Rsp_ReadBumplessTransferConf_t *d1 = (void *)dst;
    BumplessTransferConf_t *d = (void *)d1;

    UNUSED_OK(src);
    BumplessConf_t t;
    (void)bumpless_GetConf(&t);

    util_PutU8(d->BumplessRequired[0], t.Required);
    util_PutU8(d->BumplessSpeed[0], t.BumplessSpeed);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
