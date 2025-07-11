/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_engunitsif.c
    \brief HART glue layer for reading internal scaling coefficients

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_engunitsif.c $
    $Date: 1/23/12 4:03p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_engunitsif.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 4:03p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8762 - Publish internal conversions (again)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 11:55a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8762 - Publish internal conversions
*/
#include "mnwrap.h"
#include "hartfunc.h"
#include "hartpriv.h"
#include "hartdef.h"
#include "bufutils.h"
#include "fpconvert.h"


/**
\brief Read FP coefficients (see xml or html definition for description)
*/
s8_least hartcmd_ReadConversionCoefficients(const u8 *src, u8 *dst)
{
    Rsp_ReadConversionCoefficients_t *d = (void *)dst;
    const Req_ReadConversionCoefficients_t *s = (const void *)src;
    u8 unitsid = util_GetU8(s->HFP_Dimension[0]);
    if(unitsid >= HartUnitsTab_size)
    {
        return HART_INVALID_SELECTION;
    }
    UnitsId_t uid = HartUnitsTab[unitsid];
    if((u8)uid == 0U)
    {
        return HART_INVALID_SELECTION;
    }

    const UnitsConv_t *p = fpconvert_GetConvertParam(uid);

    util_PutU8(d->HFP_UnitsCode[0], p->ExtrnId);
    util_PutS32(d->HFP_i2f_slope[0], p->uiscale);
    util_PutS32(d->HFP_Xintercept[0], p->Xintercept);
    util_PutS32(d->HFP_Yintercept[0], p->Yintercept);
    util_PutU8(d->HFP_i2f_shift[0], p->uiright_shift);
    util_PutU8(d->HFP_order[0], p->uidecpt);
    util_PutU32(d->HFP_f2i_slope[0], p->f2i_scale);
    util_PutU8(d->HFP_f2i_shift[0], p->f2i_shift);

    UNUSED_OK(d->HFP_Dimension); //rely on HART framework to fill data

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
