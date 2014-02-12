/*
Copyright 2005-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \addtogroup proj Project definitions (ESD)
    \brief A collection of project-defining compile-time constants

*/
/**
    \file mncbdefs.c
    \brief Global project-specific defines

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/mncbdefs.c $
    $Date: 7/14/11 5:27p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup proj
*/
/* (Optional) $History: mncbdefs.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/14/11    Time: 5:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface
 * TFS:7028 HART 6 is 4.1.1
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/21/11    Time: 2:01p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface
 * TFS:6693,6720 UI device identification depending on HART personality
 * (Based on trunk ver. 20)
 *
 * *****************  Version 20  *****************
 * User: Arkkhasin    Date: 5/28/10    Time: 12:24a
 * Updated in $/MNCB/Dev/FIRMWARE/configure
 * Partial taming of headers toward modularization. No code change
 *
*/
#include "mnwrap.h"
#include "mncbdefs.h"
#include "nvram.h"
#include "hart.h"


//lint -esym(765, VerString)  ident is used by bsp_a.s79
//lint -esym(714, VerString)  ident is used by bsp_a.s79
//lint -esym(754, VerInfo_t::*)  members used by image massage tool

//required interface; doc in mncbdefs.h
const s16 VerNumber[] =
{
    [0] = SW_REV + (10*(TXSPEC_REV + (10*(MNCB_HW_REV)))),
};

const VerInfo_t VerString[] =
{
    [HART_REV_5] =
    {
        .signature = 4, //a unique identifier of versioned VerString for Ernie's hex massage
        .trans_spec_revision = TXSPEC_REV,
        .sw_revision = SW_REV,
        .hw_revision = MNCB_HW_REV,
        .fw_type = FIRMWARE_TYPE,
        .date_str = MNCB_DEFAULT_DATE_STRING, /*lint !e784 No nul terminator character*/
        .num_nvm_volumes = 1,
        .nvram_version = { MNCB_VERSION_NUMBER },
        .ManufacturerDeviceCode = MNCB_MANUFACTURER_DEVICE_CODE,
    },
    [HART_REV_6] = //identical for now
    {
        .signature = 4, //a unique identifier of versioned VerString for Ernie's hex massage
        .trans_spec_revision = TXSPEC_REV,
        .sw_revision = SW_REV,
        .hw_revision = MNCB_HW_REV,
        .fw_type = FIRMWARE_TYPE,
        .date_str = MNCB_DEFAULT_DATE_STRING, /*lint !e784 No nul terminator character*/
        .num_nvm_volumes = 1,
        .nvram_version = { MNCB_VERSION_NUMBER },
        .ManufacturerDeviceCode = MNCB_MANUFACTURER_DEVICE_CODE,
    },
};

/* End of source */
