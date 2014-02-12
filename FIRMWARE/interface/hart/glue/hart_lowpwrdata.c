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
    \brief HART glue layer for low power data

    Probably needs a split between I/P limiting and fault setting data
    See devel trunk for separated data structures

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_lowpwrdata.c $
    $Date: 8/25/11 6:23p $
    $Revision: 4 $
    $Author: Anatoly Podpaly $

    \ingroup HARTapp
*/
/* $History: hart_lowpwrdata.c $
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 8/25/11    Time: 6:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:7264 - two HART commands were modifying separate parts of one data
 * structure, one was executing without the reading the whole structure
 * first; thus "cross contaminating" the results. Fixed : RMW with Reading
 * the whole data structure.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 2/17/11    Time: 3:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5607 -- moved Low Power handling code to SYSIO layer.
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 9/18/10    Time: 1:11a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * Updated HART var names. Hopefully, final for TFS:4067
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/14/10    Time: 11:50p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 *  129/140-cum-140,250 in xml (TFS:4067)
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "sysiolopwr.h"

/** \brief Write I/P limiting power coefficients
*/
s8_least hartcmd_WriteLowPowerIPlimitingFactoryCoef(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    LowPowerData_t lpdata;
    //Parse the input buffer
    const Req_WriteLowPowerIPlimitingFactoryCoef_t *s1 = (const void *)src;
    const HIPLimitingFactoryCoef_t *s = (const void *)s1->HIPLimitingFactoryCoef[0];

    (void)sysio_GetLowPwrData(&lpdata);

    lpdata.uiLo     =      util_GetU16(s->LPFCIPLimitingThreshold[0]);
    lpdata.nPowerCoef    = util_GetS16(s->LPFCPowerCoef[0]);
    lpdata.nPressureCoef = util_GetS16(s->LPFCPressureCoef[0]);

    ErrorCode_t err = sysio_SetLowPowerData(&lpdata);
    return err2hart(err);
}

/** \brief Write power monitoring threshold coefficients
*/
s8_least hartcmd_WriteLowPowerThresholdsFactoryCoef(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    LowPowerData_t lpdata;
    //Parse the input buffer
    const Req_WriteLowPowerThresholdsFactoryCoef_t *s1 = (const void *)src;
    const HLowPowerThresholdsFactoryCoef_t *s = (const void *)s1->HLowPowerThresholdsFactoryCoef[0];

    (void)sysio_GetLowPwrData(&lpdata);

    lpdata.uiHysteresis  = util_GetU16(s->LPFCHysteresis[0]);
    lpdata.uiMarginal   =  util_GetU16(s->LPFCMarginalPowerThreshold[0]);
    lpdata.uiLoLo   =      util_GetU16(s->LPFCLowPowerThreshold[0]);

    ErrorCode_t err = sysio_SetLowPowerData(&lpdata);
    return err2hart(err);
}

//-------------------------------------------------------------

/** \brief Read I/P limiting power coefficients
*/
s8_least hartcmd_ReadLowPowerIPlimitingFactoryCoef(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    //Parse the input buffer
    Rsp_ReadLowPowerIPlimitingFactoryCoef_t *d1 = (void *)dst;
    HIPLimitingFactoryCoef_t *d = (void *)d1->HIPLimitingFactoryCoef[0];

    const LowPowerData_t *lpdata = sysio_GetLowPwrData(NULL);

    //Stuff the input buffer
    util_PutU16(d->LPFCIPLimitingThreshold[0], lpdata->uiLo);
    util_PutS16(d->LPFCPowerCoef[0], lpdata->nPowerCoef);
    util_PutS16(d->LPFCPressureCoef[0], lpdata->nPressureCoef);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/** \brief Read power monitoring threshold coefficients
*/
s8_least hartcmd_ReadLowPowerThresholdsFactoryCoef(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadLowPowerThresholdsFactoryCoef_t *d1 = (void *)dst;
    HLowPowerThresholdsFactoryCoef_t *d = (void *)d1->HLowPowerThresholdsFactoryCoef[0];

    const LowPowerData_t *lpdata = sysio_GetLowPwrData(NULL);

    //Stuff the input buffer
    util_PutU16(d->LPFCHysteresis[0], lpdata->uiHysteresis);
    util_PutU16(d->LPFCLowPowerThreshold[0], lpdata->uiLoLo);
    util_PutU16(d->LPFCMarginalPowerThreshold[0], lpdata->uiMarginal);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
