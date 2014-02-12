/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_writeato.c
    \brief HART glue layer for writing air action configuration

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_writeato.c $
    $Date: 1/15/12 2:23p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_writeato.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/15/12    Time: 2:23p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8680 -write air action and pid slot
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "position.h"

/**
\brief Write air action flag
*/
s8_least hartcmd_WriteAirAction(const u8 *src, u8 *dst)
{
    const Req_WriteAirAction_t *s = (const void *)src;
    Rsp_WriteAirAction_t *d = (void *)dst;

    PositionConf_t conf;
    (void)pos_GetPositionConf(&conf);
    u8 AirAction = util_GetU8(s->AirAction[0]);
    conf.bATO = int2bool(AirAction);
    util_PutU8(d->AirAction[0], bool2int(conf.bATO));

    ErrorCode_t err = pos_SetPositionConf(&conf);
    return err2hart(err);
}

/* This line marks the end of the source */
