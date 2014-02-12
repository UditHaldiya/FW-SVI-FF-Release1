/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file aosig.h
    \brief API of whatever is necessary to keep AO user trim synchronized with signal's.

    In DLT, AO1 user trim must match readback user trim; if they go out of sync,
    the newer trim is rolled back.
    In MNCB, there is no AO1.
    In other projects, AO1 and signal trims don't have to be synchronized.

    This is a MNCB flavor

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/inc_MNCB/aosig.h $
    $Date: 1/29/10 2:32p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: aosig.h $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 2:32p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * aosig_WriteSigUserTrim prototype moved from aosig.h to insignal.h
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/07/09    Time: 12:59p
 * Created in $/MNCB/Dev/FIRMWARE/inc_MNCB
 * First shot at synchronized user trim of AO1 and signal readback:
 * added trim storage modules for DLT and MNCB
*/
#ifndef AOSIG_H_
#define AOSIG_H_

#include "ao.h"
#include "insignal.h"


MN_INLINE ErrorCode_t aosig_WriteAOUserTrim(u8_least index, const AOCalibration_t *aocal)
{
    UNUSED_OK(aocal);
    return ram2nvramAtomic(NVRAMID_AOCalibration+index);
}

#endif //AOSIG_H_
/* This line marks the end of the source */
