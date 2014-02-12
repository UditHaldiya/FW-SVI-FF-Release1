/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_dosw.c
    \brief HART glue layer for DO switches

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_dosw.c $
    $Date: 10/07/10 5:14p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/

/*
    $History: hart_dosw.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 10/07/10   Time: 5:14p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * HART switches separation FOB TFS:4036
*/

#include "mnwrap.h"
#include "hartfunc.h"
#include "configure.h"
#include "doswitch.h"
#include "hartdef.h"
#include "bufutils.h"

/**
\brief Returns the current switch status (open or closed)

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadDOswitches(const u8 *src, u8 *dst)
{
    SwitchReading_t  SwitchReading;
    UNUSED_OK(src);

    Rsp_ReadDOswitches_t *d = (void *)dst;

    dosw_GetSwitchReading(&SwitchReading);
    CONST_ASSERT(NELEM(d->DOSwitchState) == DO_NUM_CHANNELS);

    SwitchConfiguration_t SwitchConfiguration;
    (void)cnfg_GetFillSwitchConfiguration(&SwitchConfiguration);
    //only allowed when switches are enabled
    if(cnfg_GetOptionConfigFlag(DO_OPTION))
    {

        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            u8 sw = SwitchReading.bSwitch[i];
            util_PutU8(d->DOSwitchState[i], sw);
            sw ^= SwitchConfiguration.swconf[i].SwitchDirection;
            util_PutU8(d->DOSwitchLogicalState[i], sw);
        }
    }
    else
    {
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            util_PutU8(d->DOSwitchState[i], 0);
            util_PutU8(d->DOSwitchLogicalState[i], 0);
        }
    }

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


s8_least hartcmd_WriteDOswitches(const u8 *src, u8 *dst)
{
    const Req_WriteDOswitches_t *s = (const void *)src;
    if(!cnfg_GetOptionConfigFlag(DO_OPTION))
    {
        return COMMAND_NOT_IMPLEMENTED;
    }
    DO_Passthrough_t Passthrough;
    for(u8 i=0; i<DO_NUM_CHANNELS; i++)
    {
        Passthrough.state[i] = bool2int(int2bool(util_GetU8(s->DOSwitchLogicalState[i])));
    }
    dosw_SetPassthroughValues(&Passthrough);
    UNUSED_OK(dst);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;

}

/**
\brief
*/
s8_least hartcmd_WriteOutputSwitchesConfiguration(const u8 *src, u8 *dst)
{
    s8_least ret;
    Rsp_WriteOutputSwitchesConfiguration_t *d = (void *)dst;
    UNUSED_OK(d->DOSWConf); //rely on HART infrastructure to fill
    const Req_WriteOutputSwitchesConfiguration_t *s1 = (const void *)src;
    const DOSWConf_t *s = (const void *)s1->DOSWConf[0];

    SwitchConfiguration_t SwitchConfiguration;
    ///get the data from the hart buffer
    (void)cnfg_GetFillSwitchConfiguration(&SwitchConfiguration);
    if(cnfg_GetOptionConfigFlag(DO_OPTION))
    {
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            SwitchConfiguration.swconf[i].SwitchDirection = util_GetU8(s[i].DONormalState[0]);
            SwitchConfiguration.swconf[i].SwitchType = util_GetU8(s[i].DOFunction[0]);
        }
        ErrorCode_t err = cnfg_SetSwitchConfiguration(&SwitchConfiguration);
        ret = err2hart(err);
    }
    else
    {
        ret = COMMAND_NOT_IMPLEMENTED;
    }

    return ret;
}

/**
\brief Returns the configuration of the switches and their current position

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadOutputSwitchesConfiguration(const u8 *src, u8 *dst)
{
    SwitchConfiguration_t SwitchConfiguration;
    UNUSED_OK(src);
    Rsp_ReadOutputSwitchesConfiguration_t *d1 = (void *)dst;
    DOSWConf_t *d = (void *)d1->DOSWConf[0];

    (void)cnfg_GetFillSwitchConfiguration(&SwitchConfiguration);

    // valid only if switch option present
    if(cnfg_GetOptionConfigFlag(DO_OPTION))
    {
        ///put the data into the hart buffer
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            util_PutU8(d[i].DONormalState[0], SwitchConfiguration.swconf[i].SwitchDirection);
            util_PutU8(d[i].DOFunction[0], SwitchConfiguration.swconf[i].SwitchType);
        }
    }
    else
    {
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            util_PutU8(d[i].DONormalState[0], 0);
            util_PutU8(d[i].DOFunction[0], 0);
        }
    }

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


/* This line marks the end of the source */
