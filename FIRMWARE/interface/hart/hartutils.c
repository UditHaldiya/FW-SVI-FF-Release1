/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup mnhartcmd HART Command Module
\brief Dispatcher of HART commands

*/
/**
    \file hartutils.c
    \brief Misc. HART helpers

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart/hartutils.c $
    $Date: 9/02/11 2:21p $
    $Revision: 12 $
    $Author: Arkkhasin $

    \ingroup mnhartcmd
*/
/* $History: hartutils.c $
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 9/02/11    Time: 2:21p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * TFS:7480 Post-integration Lint cleanup
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 8/31/11    Time: 6:42p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * TFS:7480 Lint
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 8/10/11    Time: 1:45p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * HART 6 integration from AP
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 8/10/11    Time: 1:15p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * HART 6 integration from AP
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:26p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * Adaptation to ESD. For now, device variables removed - and don't belong
 * here
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 7/19/11    Time: 1:30p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * Re-sync to TFS:C20251
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 7/15/11    Time: 2:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * Sync to TFS:C20251 (FBO TFS:7036)
*/
#include "mnwrap.h"
#include "errcodes.h"
#include "hartfunc.h"
#include "hartcmd.h" //for err2hart
#include "faultpublic.h"
#include "devicemode.h"

/** \brief Prepares response's status byte 2

    \param[in] masterFlag - true = primary, false = secondary
    \return the status byte
*/
u8 hart_makeResponseFlags(bool_t masterFlag)
{
    u8 rsp2 = 0;

    const u8 *enable_masks;

    if(nvram_GetConfigurationChangedFlag(masterFlag))
    {
        rsp2 |= HART_CONFIG_CHANGED;
    }

    if(error_IsAnyFaultWithAttributes(FATTRIB_FAILSAFE)) // *not* failsafe mode!
    {
        rsp2 |= HART_DEVICE_FAILED;
    }

    //Check how the faults are doing
    enable_masks = error_GetFaultCodeMasks(NULL)->hideInStatusBit;
    if(error_IsAnyFaultExt(enable_masks))
    {
        rsp2 |= HART_ADDL_STATUS;
    }
    return hart_StatusPlugin(rsp2);
}

#if 0 //doesn't belong to all versions and thus this file
/** \brief Prepares extended device status, for cmd 0, 9, 48 of
   Hart6
   \param[in]
   \param[out]
   \return the extended device status byte
*/
u8 hart_ExtendedDeviceStatus(void)
{
    u8  ExtDevStatus = 0;

    if (hart_DeviceVariableAlert())
    {
        ExtDevStatus |= 0x02U;
    }
    return ExtDevStatus;
}
#endif

/** \brief convert error codes to hart codes
*   \param[in]  error code
*   \param[out]
*   \return   hart code
*/
s8_least err2hart(ErrorCode_t err)
{
    s8_least ret;
    switch (err)
    {
    //success
    case ERR_OK:
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
        break;

    case ERR_NOT_SUPPORTED:
    case ERR_INVALID_PARAMETER:
        ret = HART_INVALID_SELECTION;
        break;

        //mode checks
    case ERR_MODE_ILLEGAL_SUBMODE:
        ret = HART_WRONG_MODE;
        break;
    case ERR_HART_RV_LOW_TOO_HIGH:
         ret = HART_LOWERRANGE_TOOHIGH;
        break;
    case ERR_HART_RV_LOW_TOO_LOW:
        ret = HART_LOWERRANGE_TOOLOW;
        break;
    case ERR_HART_RV_HIGH_TOO_HIGH:
        ret = HART_UPPERRANGE_TOOHIGH;
        break;
    case ERR_HART_RV_HIGH_TOO_LOW:
        ret = HART_UPPERRANGE_TOOLOW;
        break;
    case ERR_HART_RV_SPAN_LOW:
        ret = HART_INVALID_SPAN;
        break;
    case  ERR_HART_RV_BOTH_OOR:
        ret = HART_RV_BOTH_OOR;
        break;
    case ERR_MODE_CANNOT_CHANGE:
        ret = HART_WRONG_MODE;
        break;
    case ERR_UPPERLIM:
        ret =  HART_PASSED_PARAMETER_TOO_LARGE;
        break;
    case ERR_LOWERLIM:
        ret = HART_PASSED_PARAMETER_TOO_SMALL;
        break;

    default:
        ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
        break;
    } //lint !e788 Not all enum values a switch with a default
    return ret;
}


/* This line marks the end of the source */
