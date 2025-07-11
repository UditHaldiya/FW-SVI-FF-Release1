/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_sernumchip.c
    \brief HART glue layer for SN chip

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_sernumchip.c $
    $Date: 9/15/10 12:17a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_sernumchip.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/15/10    Time: 12:17a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * 129/172 in xml (TFS:4067)
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "sernumchip.h"

/**
\brief Read SN data
*/
s8_least hartcmd_ReadRawSNData(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadRawSNData_t *d = (void *)dst;
    SerialNumberError_t SNReadError;
    u64                 SerialNumber;

    // Read the data
    SNReadError = bios_ReadSerialization(&SerialNumber);

    util_PutU8Array(d->RawSNData[0], sizeof(SerialNumber), (u8 *)&SerialNumber);
    util_PutU8(d->RawSNReadCode[0], (u8)SNReadError);     // Place Error into the buffer

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
