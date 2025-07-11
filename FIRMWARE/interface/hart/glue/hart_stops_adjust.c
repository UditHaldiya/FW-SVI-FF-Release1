/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_stops_adjust.c
    \brief HART glue layer for reading and setting parameters determining open
           and closed stops adjustment.
           Closed stops adjustment is for future use only - presently not implemented

    CPU: Any

    OWNER:
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_stops_adjust.c $
    $Date: 12/08/11 6:37p $
    $Revision: 6 $
    $Author: Justin Shriver $

    \ingroup HARTapp
*/

/* $History: hart_stops_adjust.c $
 *
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 12/08/11   Time: 6:37p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * -- AP -- LINT - removed unused header.
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:18p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8347 - Restore AP build
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 1:20p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 5/02/11    Time: 1:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5739 -- removed appropriate unused functions.
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 1/14/11    Time: 10:57a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5323 - Create configurabel OS Adj overrun and modify HART commands
 * for read/set OS Adj to XML-based.
*/

#include "mnwrap.h"
#include "position.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "fpconvert.h"


//this is a command shell for subcommands -- does nothing
s8_least hartcmd_WriteStopAdjust(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_WriteStopAdjust() ------


/**
    \brief Write Open Stop Adjustment - subcommand 0
           Note:
           This command is allowed in OOS and Failsafe modes
*/
s8_least hartcmd_SetOpenStopAdjust(const u8 *src, u8 *dst)
{
    const Req_SetOpenStopAdjust_t *s = (const void *)src;
    Rsp_SetOpenStopAdjust_t *d = (void *)dst;

    s8_least ret;
    //get the OS Adj value from the wire
    pos_t OpenStopAdjustment = fpconvert_FloatBufferToInt16Clamped(s->adj_OpenStopAdj[0], UNITSID_PERCENT, OpenStopAdj_range);

    //write it to the FW structure
    if(pos_SetStopAdjustment(STANDARD_ZERO, OpenStopAdjustment) != ERR_OK)
    {
        ret = HART_INVALID_DATA;
    }
    else
    {
        (void)fpconvert_IntToFloatBuffer(pos_GetPositionConf(NULL)->PositionRange[Xhi], UNITSID_PERCENT, d->adj_OpenStopAdj[0]);
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    return ret;
} //-----  end of hartcmd_SetOpenStopAdjust() ------




//this is a command shell for subcommands -- does nothing
s8_least hartcmd_ReadStopAdjust(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_WriteStopAdjust() ------


/**
    \brief Read Open Stop Adjustment - subcommand 0
           Note:
           This command is allowed in all modes
*/
s8_least hartcmd_ReadOpenStopAdjust(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadOpenStopAdjust_t *d = (void *)dst;
    (void)fpconvert_IntToFloatBuffer(pos_GetPositionConf(NULL)->PositionRange[Xhi], UNITSID_PERCENT, d->adj_OpenStopAdj[0]);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_ReadStopAdjust() ------

/* This line marks the end of the source */
