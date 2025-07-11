/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_writectlselector.c
    \brief Part of Glue layer of position control HART support

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_writectlselector.c $
    $Date: 1/15/12 2:23p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_writectlselector.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/15/12    Time: 2:23p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8680 -write air action and pid slot
*/
#include "mnwrap.h"
#include "errcodes.h"
#include "selftune.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"

/** \brief Writes the current index of PID parameters set
*/
s8_least hartcmd_WriteControlParametersSelector(const u8 *src, u8 *dst)
{
    Rsp_WriteControlParametersSelector_t *d = (void *)dst;
    UNUSED_OK(d->PIDDataSelector); //rely on HART framework to fill
    const Req_WriteControlParametersSelector_t *s = (const void *)src;
    CtlSetSlot_t ctlset;
    (void)posctl_GetCtlset(&ctlset);
    ctlset.slot = util_GetU8(s->PIDDataSelector[0]);
    ErrorCode_t err = posctl_SetCtlset(&ctlset);
    return err2hart(err);
}

/* This line marks the end of the source */
