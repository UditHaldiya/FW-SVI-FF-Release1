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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_hwuisimple.c $
    $Date: 1/05/12 3:13p $
    $Revision: 7 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_hwuisimple.c $
 * 
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:13p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8577 - header refactoring
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 1/20/11    Time: 5:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5440 - eliminate HART command to set and read UI LOCK settings.
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 12/06/10   Time: 3:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4959: UI_Get_ImmediateATO and UI_Get_ImmediatePIDIndex moved from
 * hart_posctl.c to hart_hwuisimple.c. Subcommands 218, 219 of cmd 170
 * became LCX-specific.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 11/18/10   Time: 1:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * Bug 4759 - Added missing DO information to HART cmd 129,150.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 2:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * Bug 4188 - implemented UI Lock - HART interface.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/14/10    Time: 11:00p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * 129/161-cum-150 in xml (TFS:4067)
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "key.h"
#include "mncbtypes.h"
#include "quad.h"
#include "uisimple.h"
#include "uilock.h"
#include "dohwif.h"

//------------------------------------------------------------------

/**
\brief Read SN data
*/
s8_least hartcmd_ReadRawUIHWData(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadRawUIHWData_t *d = (void *)dst;

    u8  PB_Edges;
    u8  PB_Status;
    s16 RE_Steps;
    bool_t  TempBool;

    // Place LED states into the response data buffer
    util_PutU8(d->LEDState[0], (u8)LED_GetDisplayState());

    // Place BIOS Jumper reading
    util_PutU8(d->RotarySwitchState[0], bios_ReadJumpers());

    // Place BIOS PushButtons reading
    PB_Edges = bios_ReadPushButtonStatus(&PB_Status);
    util_PutU8(d->PushbuttonsEdges[0], PB_Edges);
    util_PutU8(d->PushbuttonsState[0], PB_Status);

    RE_Steps = quad_ReadAndClearValue();
    util_PutS16(d->QuadEncoderStep[0], RE_Steps);

    // Read DO information
    TempBool = bios_DOCardPresent();
    util_PutU8(d->DOCardPresence[0], bool2int(TempBool));

    TempBool = bios_GetDigitalOutput(DO_1);
    util_PutU8(d->DO1HardwareState[0], bool2int(TempBool));

    TempBool = bios_GetDigitalOutput(DO_2);
    util_PutU8(d->DO2HardwareState[0], bool2int(TempBool));

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

//------------------------------------------------------------------
// UI Lock HART commands

#if UI_LOCK_FEATURE == UI_LOCK_FEATURE_ON
/**
 \brief Read UI Access control data (170,2)
 */
s8_least hartcmd_ReadUIAccessControl(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);

    Rsp_ReadUIAccessControl_t *d1 = (void *)dst;
    UIAccessControl_t *d = (UIAccessControl_t *)d1->UIAccessControl[0];

    UI_LockData_t   LockData;
    UI_GetLockData(&LockData);

    util_PutU8(d->UILockLevel[0], (u8)(LockData.LockState));
    util_PutU8(d->UIPasswordEnabledFlag[0], bool2int(LockData.PasswordEnabled));
    util_PutU16(d->UIpassword[0], LockData.Password);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
    \brief Write UI Access Control data (171,2)
 */
s8_least hartcmd_WriteUIAccessControl(const u8 *src, u8 *dst)
{
    ErrorCode_t err;

    const Req_WriteUIAccessControl_t *s1 = (const void *)src;
    const UIAccessControl_t *s = (const void *)s1->UIAccessControl[0];
    Req_WriteUIAccessControl_t *d1 = (void *)dst;
    UIAccessControl_t *d = (void *)d1->UIAccessControl[0];

    UI_LockData_t   LockData;
    UI_GetLockData(&LockData);

    bool_t flag = int2bool(util_GetU8(s->UIPasswordEnabledFlag[0]));
    LockData.PasswordEnabled = flag;
    util_PutU8(d->UIPasswordEnabledFlag[0], bool2int(flag));

    LockData.Password  = util_GetU16(s->UIpassword[0]);
    LockData.LockState = util_GetU8(s->UILockLevel[0]);

    err = UI_SetLockData(&LockData);

    // //We rely on the response field filled automatically
    // CONST_ASSERT(MN_OFFSETOF(Req_WriteUIAccessControl_t,UIAccessControl)==MN_OFFSETOF(Rsp_WriteUIAccessControl_t,UIAccessControl));

    return err2hart(err);
}

#endif

/** \brief Returns index of the current Immediate PID parameters set
*/
s8_least hartcmd_ReadControlImmediateParametersSelector(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadControlImmediateParametersSelector_t *d = (void *)dst;
    util_PutU8(d->PIDDataSelector[0], UI_Get_ImmediatePIDIndex());
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/** \brief Returns index of the current Immediate ATO Selector
*/
s8_least hartcmd_ReadImmediateATOSelector(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadImmediateATOSelector_t *d = (void *)dst;
    util_PutU8(d->ATOSelector[0], (u8)UI_Get_ImmediateATO());
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


/* This line marks the end of the source */
//------------------------------------------------------------------

