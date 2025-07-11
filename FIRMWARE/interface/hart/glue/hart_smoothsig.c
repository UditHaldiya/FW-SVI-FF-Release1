/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup HARTapp
\brief HART application layer component

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Hart Module.doc"> Design document </a><br>
Unit Test Credit claim is found <a href="../../../Doc/TestDocs/UnitTests.doc"> here </a><br>
\endhtmlonly
*/
/**
    \file hart_posmon.c
    \brief Glue functions to support factory-defined smoothing of common input variables

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_smoothsig.c $
    $Date: 11/15/11 9:53p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_smoothsig.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 9:53p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/30/11    Time: 1:39p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Reverted to old API for old fixed-length commands
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/07/09    Time: 12:50p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Signal conf separated from trim; added trim storage modules for DLT and
 * MNCB
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/16/09    Time: 5:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Resurrected old Larry's "smooth filters" settings
*/
#include "mnwrap.h"
#include "hartcmd.h"
#include "hartfunc.h"
#include "hart.h"
#include "bufutils.h"
#include "hartdef.h"
#include "insignal.h"


s8_least hartcmd_ReadSignalSmoothingCoefficient(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadSignalSmoothingCoefficient_t *d = (void *)dst;
    util_PutU8(d->SmoothCoefSignal[0], sig_GetSignalConf(NULL)->filtercoef.Shift);
    return HART_OK;
}
s8_least hartcmd_WriteSignalSmoothingCoefficient(const u8 *src, u8 *dst)
{

    UNUSED_OK(dst);
    SignalConf_t sc;
    const Req_WriteSignalSmoothingCoefficient_t *s = (const void *)src;
    (void)sig_GetSignalConf(&sc);
    sc.filtercoef.Shift = util_GetU8(s->SmoothCoefSignal[0]);
    ErrorCode_t err = sig_SetSignalConf(&sc);
    s8_least ret;
    if(err == ERR_OK)
    {
        ret = HART_OK;
    }
    else
    {
        ret = HART_INVALID_DATA;
    }
    return ret;
}
/* This line marks the end of the source */
