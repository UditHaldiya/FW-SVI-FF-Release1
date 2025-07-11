/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_hwuisimple.c
    \brief HART glue layer for LED-based (LCX-style) UI hardware layer

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_digipot.c $
    $Date: 10/20/10 6:33p $
    $Revision: 2 $
    $Author: Anatoly Podpaly $

    \ingroup HARTapp
*/
/* $History: hart_digipot.c $
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/20/10   Time: 6:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * Bug 4223 - added command to read COnfiog register. Corrected other
 * commands to use correct error codes and pass parameters.
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/14/10    Time: 11:33p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * 129/17x in xml (TFS:4067)
*/
#include "mnwrap.h"
#include "hartdef.h"
#include "errcodes.h"
#include "bufutils.h"
#include "hartfunc.h"
#include "digital_pot.h"
 
s8_least hartcmd_WriteOutputDigitalPot(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);

    const Req_WriteOutputDigitalPot_t *s = (const void *)src;
    s8_least                ret;
    ErrorCode_t             DR_Error;
    DR_WiperStorage_t       DR_Storage;
    u16                     WiperPos;

    // Parse command
    u8 Type = util_GetU8(s->DigiPotMemoryDest[0]);
    WiperPos = util_GetU16(s->DigiPotData[0]);                   // Get position

    DR_Storage = (Type == 0) ? DR_WIPER_VOLATILE_STORAGE : DR_WIPER_NONVOLATILE_STORAGE;
    DR_Error = DigitalResistor_WriteWiper(DR_Storage, WiperPos);
    if (DR_Error == ERR_OK)
    {   // Everything is OK
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    else if (DR_Error == ERR_BIOS_PARAM)
    {   // Position out of range
        ret = HART_PASSED_PARAMETER_TOO_LARGE;
    }
    else
    {   // Something is ioncorrect
        ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
    }
    return ret;
}

s8_least hartcmd_ReadOutputDigitalPot(const u8 *src, u8 *dst)
{
    const Req_ReadOutputDigitalPot_t *s = (const void *)src;
    Rsp_ReadOutputDigitalPot_t *d = (void *)dst;

    ErrorCode_t             DR_Error;
    DR_WiperStorage_t       DR_Storage;
    u16                     WiperPos;

    u8 Type = util_GetU8(s->DigiPotMemoryDest[0]);
    DR_Storage = (Type == 0) ? DR_WIPER_VOLATILE_STORAGE : DR_WIPER_NONVOLATILE_STORAGE;
    DR_Error = DigitalResistor_ReadWiper(DR_Storage, &WiperPos);
    util_PutU16(d->DigiPotData[0], WiperPos);
    util_PutU8(d->DigiPotError[0],  (u8)DR_Error);       // Place Error into the buffer
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_EnableOutputDigitalPotWiperLock(const u8 *src, u8 *dst)
{
    s8_least                ret;
    ErrorCode_t             DR_Error;
    UNUSED_OK(src);
    UNUSED_OK(dst);

    DR_Error = DigitalResistor_EnableWiperLock();
    ret = (DR_Error == ERR_OK) ? HART_NO_COMMAND_SPECIFIC_ERRORS :
                                 TRANSMITTER_SPECIFIC_COMMAND_ERROR;
    return ret;
}

s8_least hartcmd_DisableOutputDigitalPotWiperLock(const u8 *src, u8 *dst)
{
    s8_least                ret;
    ErrorCode_t             DR_Error;
    UNUSED_OK(src);
    UNUSED_OK(dst);

    DR_Error = DigitalResistor_DisableWiperLock();
    ret = (DR_Error == ERR_OK) ? HART_NO_COMMAND_SPECIFIC_ERRORS :
                                 TRANSMITTER_SPECIFIC_COMMAND_ERROR;
    return ret;
}

s8_least hartcmd_ReadOutputDigitalPotStatus(const u8 *src, u8 *dst)
{
    ErrorCode_t             DR_Error;
    u16                     StatusRegister;

    UNUSED_OK(src);
    Rsp_ReadOutputDigitalPotStatus_t *d = (void *)dst;

    DR_Error = DigitalResistor_ReadStatus(&StatusRegister);
    util_PutU16(d->DigiPotStatus[0], StatusRegister);
    util_PutU8(d->DigiPotError[0],  (u8)DR_Error);       // Place Error into the buffer
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ReadOutputDigitalPotConfig(const u8 *src, u8 *dst)
{
    ErrorCode_t             DR_Error;
    u16                     ConfigRegister;

    UNUSED_OK(src);
    Rsp_ReadOutputDigitalPotConfig_t *d = (void *)dst;

    DR_Error = DigitalResistor_ReadConfig(&ConfigRegister);
    util_PutU16(d->DigiPotConfig[0], ConfigRegister);
    util_PutU8(d->DigiPotError[0],  (u8)DR_Error);       // Place Error into the buffer
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
