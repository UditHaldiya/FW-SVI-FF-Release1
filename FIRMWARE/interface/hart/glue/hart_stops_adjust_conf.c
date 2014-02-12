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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_stops_adjust_conf.c $
    $Date: 12/05/11 4:45p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/

/* $History: hart_stops_adjust_conf.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:45p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8347
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
#include "mncbtypes.h"
#include "configure.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "conversion.h"
#include "osadjust.h"


/**
    \brief Write Open Stop Adjustment Overrun - subcommand 2
           Note:
           This command is allowed in OOS and Failsafe modes
*/
s8_least hartcmd_SetOSAdjustOverrun(const u8 *src, u8 *dst)
{
    const Req_SetOSAdjustOverrun_t *s = (const void *)src;
    //Rsp_SetOSAdjustOverrun_t *d = (void *)dst;

    s8_least HARTerr;
    ErrorCode_t err;
    float32 fOSAdjOverrun;
    //const PositionStop_t* pPositionStop;

    //get the OS Adj value from the wire
    fOSAdjOverrun = util_GetFloat(s->adj_OSOverrun[0]);
    //write it to the FW structure
    OSAdjConf_t conf;
    (void)osadj_GetConf(&conf);
    conf.OSAdjOverrun = convert_FPositionToStdPosition(fOSAdjOverrun);
    err = osadj_SetConf(&conf);
    if ( err != ERR_OK )
    {
        HARTerr = HART_INVALID_SELECTION;
    }
    else
    {
        //pPositionStop = cnfg_GetPositionStop();
        //util_PutFloat(d->adj_OSOverrun[0], pPositionStop->fOSAdjOverrun);
        HARTerr = hartcmd_ReadOSAdjustOverrun(src, dst);
    }

    return HARTerr;
} //-----  end of hartcmd_SetOSAdjustOverrun() ------



/**
    \brief Read Open Stop Adjustment Overrun - subcommand 2
           Note:
           This command is allowed in all modes
*/
s8_least hartcmd_ReadOSAdjustOverrun(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadOSAdjustOverrun_t *d = (void *)dst;
#if 0
    const PositionStop_t* pPositionStop;

    pPositionStop = cnfg_GetPositionStop();
    util_PutFloat(d->adj_OSOverrun[0], pPositionStop->fOSAdjOverrun);
#else
    util_PutFloat(d->adj_OSOverrun[0], convert_StdPositionToFPosition(osadj_GetConf(NULL)->OSAdjOverrun));
#endif
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_ReadOSAdjustOverrun() ------


/* This line marks the end of the source */
