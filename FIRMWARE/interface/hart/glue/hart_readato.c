/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_readato.c
    \brief HART glue layer for reading air action configuration

NOTE: Write is conspicously absent here: not all projects allow write over HART

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_readato.c $
    $Date: 1/15/12 2:28p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_readato.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/15/12    Time: 2:28p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Cleanup FBO TFS:8680
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/28/10    Time: 6:13p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Added hart_readato.c to read Air Action flag
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "position.h"

/**
\brief Read air action flag
*/
s8_least hartcmd_ReadAirAction(const u8 *src, u8 *dst)
{
    Rsp_ReadAirAction_t *d = (void *)dst;

    UNUSED_OK(src);

    util_PutU8(d->AirAction[0], (u8)((pos_GetPositionConf(NULL)->bATO)?HAirToOpen:HAirToClose));
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
