/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_setpoint.c
    \brief HART interfaces for error limits configurations

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "fpconvert.h"
#include "bufutils.h"
#include "hartpriv.h"
#include "digitalsp.h"
#include "devicemode.h"
#include "ctllimits.h"

//---------------------------------------------------------------------------
// Verify that HART representattion of COntrol Modes matches internal representation
// HART representation is UCHAR (u8)

CONST_ASSERT((u8)CONTROL_IP_DIAGNOSTIC      == (u8)MODE_ipDiag);
CONST_ASSERT((u8)CONTROL_MANUAL_POS         == (u8)MODE_manual);
CONST_ASSERT((u8)CONTROL_IPOUT_LOW          == (u8)MODE_ipLow);
CONST_ASSERT((u8)CONTROL_IPOUT_LOW_FAIL     == (u8)MODE_ipLowFail);
CONST_ASSERT((u8)CONTROL_IPOUT_HIGH         == (u8)MODE_ipHigh);
CONST_ASSERT((u8)CONTROL_IPOUT_HIGH_FACTORY == (u8)MODE_ipHighFactory);
CONST_ASSERT((u8)CONTROL_IPOUT_HIGH_FAIL    == (u8)MODE_ipHighFail);
CONST_ASSERT((u8)CONTROL_OFF                == (u8)MODE_off);
CONST_ASSERT((u8)CONTROL_IPOUT_LOW_PROC     == (u8)MODE_ipLowProc);
CONST_ASSERT((u8)CONTROL_IPOUT_HIGH_PROC    == (u8)MODE_ipHighProc);

//---------------------------------------------------------------------------

s8_least hartcmd_ReadSetpoint(const u8 *src, u8 *dst)
{
    const Req_ReadSetpoint_t *s = (const void *)src;
    UNUSED_OK(s);
    Rsp_ReadSetpoint_t *d = (void *)dst;

    s32 sp;
    ctlmode_t ctlmode = mode_GetIntendedControlMode(&sp); //may be broken with the current treatment in control.c
    util_PutU8(d->ControlAlgorithmMode[0], (u8)ctlmode); //lint !e633 Sending out a strong type in a byte
    (void)fpconvert_IntToFloatBuffer(sp, UNITSID_POSITION, d->PositionSetpoint[0]);

    sp = digsp_GetDigitalSetpoint();
    (void)fpconvert_IntToFloatBuffer(sp, UNITSID_POSITION, d->DigitalSetpoint[0]);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteSetpoint(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WritePositionSetpoint(const u8 *src, u8 *dst)
{
    const Req_WritePositionSetpoint_t *s = (const void *)src;
    Rsp_WritePositionSetpoint_t *d = (void *)dst;
    pos_t sp = fpconvert_FloatBufferToInt16Clamped(s->PositionSetpoint[0], UNITSID_POSITION, positionsp_range);
    mode_SetControlMode(CONTROL_MANUAL_POS, sp);
    (void)fpconvert_IntToFloatBuffer(sp, UNITSID_POSITION, d->PositionSetpoint[0]);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_SetFullClosed(const u8 *src, u8 *dst)
{
    const Req_SetFullClosed_t *s = (const void *)src;
    Rsp_SetFullClosed_t *d = (void *)dst;
    UNUSED_OK(s);
    UNUSED_OK(d);

    ctlmode_t ctlmode;
    if(!pos_GetPositionConf(NULL)->bATO)
    {
        ctlmode = CONTROL_IPOUT_HIGH;
    }
    else
    {
        ctlmode = CONTROL_IPOUT_LOW;
    }

    mode_SetControlMode(ctlmode, 0); //sp don't care
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_SetFullOpen(const u8 *src, u8 *dst)
{
    const Req_SetFullOpen_t *s = (const void *)src;
    Rsp_SetFullOpen_t *d = (void *)dst;
    UNUSED_OK(s);
    UNUSED_OK(d);

    ctlmode_t ctlmode;
    if(pos_GetPositionConf(NULL)->bATO)
    {
        ctlmode = CONTROL_IPOUT_HIGH;
    }
    else
    {
        ctlmode = CONTROL_IPOUT_LOW;
    }

    mode_SetControlMode(ctlmode, 0); //sp don't care
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_DeEnergize(const u8 *src, u8 *dst)
{
    const Req_DeEnergize_t *s = (const void *)src;
    Rsp_DeEnergize_t *d = (void *)dst;
    UNUSED_OK(s);
    UNUSED_OK(d);
    mode_SetControlMode(CONTROL_IPOUT_LOW, 0); //sp don't care
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
