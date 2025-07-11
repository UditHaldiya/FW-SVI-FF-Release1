/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_ao.c
    \brief HART interfaces analog output(s)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_ao.c $
    $Date: 1/12/12 12:41p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History $
*/
#include "mnwrap.h"
#include "ao.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "configure.h"
#include "fpconvert.h"
#include "bufutils.h"
#include "hartpriv.h"

/** \brief A common implementation of AO freeze interface.
NOTE: Does NOT check for valid aochannel (a constant at the time of call.
\param in - "freeze" value in a floating point buffer
\param[out] out - actually accepted "freeze" value in a floating point buffer
\param uid - units id for the channel
\param aochannel - one of the predefined channel ids
\return HART_OK or a HART error code
*/
s8_least hc_WriteFixedAO(const fpbuf_t in, fpbuf_t out, UnitsId_t uid, u8 aochannel)
{
    ErrorCode_t err;

    if(ieee754_IsZero(float2ieee(util_GetFloat(in))))
    {
        ao_Freeze(aochannel, false);
        err = ERR_OK;
    }
    else
    {
        ao_least_t AOValue = fpconvert_FloatBufferToInt16Clamped(in, uid, aotab[aochannel].aoFixed_range);
        //fill the output buffer incase and with the value was accepted
        fpconvert_IntToFloatBuffer(AOValue, uid, out);
        err = ao_SetAOValueFixed(aochannel, AOValue);
    }

    return err2hart(err);
}


/**
\brief Sets position retransmit output to a constant

Notes:
This command is allowed in all modes
    If it had a common practice number, would not be allowed if stictly conforming to HART since these
    are meant for transmitters where fixed output is the PV
*/
s8_least hartcmd_WriteFixedOptionalOutput(const u8 *src, u8 *dst)
{
    //only allowed when position retransmit is enabled
    if(!cnfg_GetOptionConfigFlag(POSITION_RETRANSMIT_OPTION))
    {
        /* Legitimate early return: Pre-requisites check */
        return COMMAND_NOT_IMPLEMENTED;
    }

    //Set the value for retransmit as requested.
    const Req_WriteFixedOptionalOutput_t *s = (const void *)src;
    Rsp_WriteFixedOptionalOutput_t *d = (void *)dst;

    return hc_WriteFixedAO(s->OptOutput[0], d->OptOutput[0], UNITSID_SIGNAL, AO_CHANNEL_POS_RETRANSMIT);
}

s8_least hartcmd_DirectAOControl(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_DirectAOControl_t *s = (const void *)src;
    Rsp_DirectAOControl_t *d = (void *)dst;
    u8_least index = util_GetU8(s->AOchannelnumber[0]);
    bool_t enable = int2bool(util_GetU8(s->Enableflag[0]));
    u16 value = util_GetU16(s->DirectControlValue[0]);

    ErrorCode_t err = ao_SetDirectControl(index, enable, value);
    util_PutU16(d->DirectControlValue[0], aotab[index].readf());
    return err2hart(err);
}

s8_least hartcmd_CalibrateAO0Retransmit(const u8 *src, u8 *dst)
{
    s8_least ret;
    const Req_CalibrateAO0Retransmit_t *s = (const void *)src;
    Rsp_CalibrateAO0Retransmit_t *d = (void *)dst;

    //only allowed when position retransmit is enabled
    if(!cnfg_GetOptionConfigFlag(POSITION_RETRANSMIT_OPTION))
    {
        /* Legitimate early return: Pre-requisites check */
        return COMMAND_NOT_IMPLEMENTED;
    }

    if(!ao_IsFixedAO(AO_CHANNEL_POS_RETRANSMIT))
    {
        return HART_NOT_PROPER_OUTPUT_MODE;
    }

    //We rely on the response field filled automatically
    CONST_ASSERT(MN_OFFSETOF(Rsp_CalibrateAO0Retransmit_t,CalType)==MN_OFFSETOF(Req_CalibrateAO0Retransmit_t,CalType));

    u8 Type = util_GetU8(s->CalType[0]);
    ao_t CalValue = fpconvert_FloatBufferToInt16Clamped(s->CalValue[0], UNITSID_SIGNAL, aotab[AO_CHANNEL_POS_RETRANSMIT].ao_CalLimits[Type]);
    fpconvert_IntToFloatBuffer(CalValue, UNITSID_SIGNAL, d->CalValue[0]);

    if(ao_Calibrate(AO_CHANNEL_POS_RETRANSMIT, Type, CalValue) == ERR_OK)
    {
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    else
    {
        //debug: ret = HART_OK;
        // - one of the two
        ret = HART_CAL_FAILED;
        //ret = HART_INVALID_DATA;
    }

    return ret;
}

s8_least hartcmd_ReadAOUserTrim(const u8 *src, u8 *dst)
{
    const Req_ReadAOUserTrim_t *s = (const void *)src;
    Rsp_ReadAOUserTrim_t *d = (void *)dst;
    AOCalibration_t aocal;
    u8 channel = util_GetU8(s->AOChannelSelector[0]);
    if(ao_GetAOCalibration(channel, &aocal) == NULL)
    {
        return HART_INVALID_SELECTION;
    }
    util_PutU8(d->AOChannelSelector[0], channel);
    UserTrimRecord_t *p = (void *)d->UserTrimRecord[0];
    for(u8_least i=Xlow; i<Xends; i++)
    {
        util_PutS16(p->UserRangeValues[i], aocal.AOCal[i]);
        util_PutS16(p->DeviceRangeValues[i], aocal.nAOCalAD[i]);
    }
    util_PutU32(p->UserTrimTag[0], aocal.SeqNumber);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ResetAO0RetransmitCalibrationsToFactory(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    (void)ao_SetAOCalibration(AO_CHANNEL_POS_RETRANSMIT, NULL);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
/* This line marks the end of the source */
