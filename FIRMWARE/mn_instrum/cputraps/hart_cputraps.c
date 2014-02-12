/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_activate.c
    \brief HART glue layer for control (position setpoint) limits

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/cputraps/hart_cputraps.c $
    $Date: 6/10/11 2:26p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_cputraps.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/10/11    Time: 2:26p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/mn_instrum/cputraps
 * TFS:6584 Instrumentation for CPU traps
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/18/11    Time: 4:16p
 * Updated in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * Added capability to add extra time load to a task
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/27/11    Time: 10:15p
 * Created in $/MNCB/Dev/FIRMWARE/mn_instrum/profiler
 * HART support of profiler instrumentation FBO TFS:5505 Step 3
*/
#include "mnwrap.h"
#include "oswrap.h"
#include "mn_instrum.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"

/**
\brief Read activation data
*/
s8_least hartcmd_ReadAWord(const u8 *src, u8 *dst)
{
    const Req_ReadAWord_t *s = (const void *)src;
    Rsp_ReadAWord_t *d = (void *)dst;

    u32 addr = util_GetU32(s->RawAddress[0]);
    u32 val = *HARDWARE(u32 *, addr);
    util_PutU32(d->DataWord[0], val);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

typedef void func_t(void);

s8_least hartcmd_Goto(const u8 *src, u8 *dst)
{
    const Req_ReadAWord_t *s = (const void *)src;
    UNUSED_OK(dst);

    u32 addr = util_GetU32(s->RawAddress[0]);
    HARDWARE(func_t *, addr)();
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteAWord(const u8 *src, u8 *dst)
{
    const Req_WriteAWord_t *s = (const void *)src;
    UNUSED_OK(dst);

    u32 addr = util_GetU32(s->RawAddress[0]);
    u32 val;
    val = util_GetU32(s->DataWord[0]);
    *HARDWARE(u32 *, addr) = val;
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
