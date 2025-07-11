/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file errlimits.c (origin: configure1.c:70)
    \brief Position control error limits

    CPU: Any

    OWNER: LS
    $Archive: /MNCB/Dev/FIRMWARE/posctl/errlimits.c $
    $Date: 3/29/09 4:12p $
    $Revision: 75 $
    $Author: Arkkhasin $

    \ingroup Configure
*/
/* (Optional) $History: errlimits.c $
 *
 * *****************  Version 75  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 74  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 6:26p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Removed obsolete conversion.{c,h}
 *
 * *****************  Version 73  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:44a
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 72  *****************
 * User: Arkkhasin    Date: 4/09/08    Time: 11:57p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Partial rearrangement of source files by modules
 *
 * *****************  Version 70  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 2:58p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 * Module initilaization table types - ModuleDataItem_t and ModuleData_t
 * removed
 * ram2nvram[Atomic] do not take a pointer to the buffer anymore, but now
 * return an error code
 *
 * *****************  Version 69  *****************
 * User: Arkkhasin    Date: 3/24/08    Time: 5:26p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * Fifth step toward uniform support of engineering units:
 * Ranges of all variables are exposed
 *
 * *****************  Version 68  *****************
 * User: Arkkhasin    Date: 3/17/08    Time: 5:11p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * Second step toward uniform support of engineering units:
 * Valid range limits for any parameter are owned by a corresponding
 * module and exposed as an array of 2 elements (low and high limits)
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 5:03p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * Removed unused header(s)
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 2/26/08    Time: 4:18p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * removed disabling I/P controller Bypass Switch (which was a bug).
 * May require a tweak in in-place reset PWM pre-emphasis
 *
 * *****************  Version 65  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 2:25a
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * SwitchConfiguration is gone - nothing to configure anymore
 * Stuck/Linkage and Supply configurations now belong to the corresponding
 * monitors
 *
 *
 * *****************  Version 64  *****************
 * User: Arkkhasin    Date: 1/25/08    Time: 9:44a
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * Lint
 *
 * *****************  Version 63  *****************
 * User: Arkkhasin    Date: 1/23/08    Time: 5:51p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * Removed old DO configuration in favor of the new doswitch.c
 *
 * *****************  Version 62  *****************
 * User: Arkkhasin    Date: 1/10/08    Time: 3:01p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * All AO-related things except its use for position retransmit is moved
 * to ao{.c,.h}
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 1/04/08    Time: 11:47a
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * Partial (and light) linting
 *
 * *****************  Version 60  *****************
 * User: Arkkhasin    Date: 1/03/08    Time: 6:56p
 * Updated in $/MNCB/Dev/FIRMWARE/utilities
 * A massive but interim check-in:
 * - Initialization is handled with a common routine
 * - ATO/ATC moved to PositionStop to improve modularity
*/
#include "mnwrap.h"
#include "errcodes.h"
#include "timedef.h"
#include "nvram.h"
#include "param.h"
#include "errlimits.h"
#include "faultpublic.h"

/** member data */
static ErrorLimits_t           m_ErrorLimits;

//defaults
static const ErrorLimits_t def_ErrorLimits =
{
    .NearPosition = INT_PERCENT_OF_RANGE(DEFAULT_fNearPosition),
    .PositionErrorBand = INT_PERCENT_OF_RANGE(DEFAULT_fPositionErrorBand),
    .PositionTime1 = MNTIME_TO_CTLCYCLES(DEFAULT_fPositionTime1),
    .bPosErr1Enable = DEFAULT_ERR_DISABLE,
    CHECKFIELD_CLEAR()
};


/** \brief returns the error limits

  returns the error limits
  \return const ErrorLimits_t*
*/
const ErrorLimits_t *pos_GetErrorLimits(ErrorLimits_t *dst)
{
    return STRUCT_TESTGET(&m_ErrorLimits, dst);
}

const pos_t errband_range[Xends] =
{
    [Xlow] = POSBAND_LOW_INT, //lower limit
    [Xhi] = POSBAND_HIGH_INT, //upper limit
};

const pos_t nearpos_range[Xends] =
{
    [Xlow] = NEARPOS_LOW_INT, //lower limit
    [Xhi] = NEARPOS_HIGH_INT, //upper limit
};

const ctltick_t errtime_range[Xends] =
{
    [Xlow] = MNTIME_TO_CTLCYCLES(POSTIME1_LOW), //lower limit
    [Xhi] = MNTIME_TO_CTLCYCLES(POSTIME1_HIGH), //upper limit
};

/** \brief Save the error limits and compute integer versions

  Save the error limits and compute integer versions
  \param[in] pErrorLimits - the data to save
  \return ERR_OK if no error, PARAM_OUT_OF_RANGE if invalid data
*/
ErrorCode_t  pos_SetErrorLimits(const ErrorLimits_t *src)
{
    s16_least errband, nearpos;
    ctltick_t time1;

    const ErrorLimits_t* pErrorLimits = src;

    if(pErrorLimits == NULL)
    {
        pErrorLimits = &def_ErrorLimits;
    }

    errband = pErrorLimits->PositionErrorBand;
    nearpos = pErrorLimits->NearPosition;

    /** validate the input  */
    if( (errband < errband_range[Xlow]) || (errband > errband_range[Xhi]) ||
        (nearpos < nearpos_range[Xlow]) || (nearpos > nearpos_range[Xhi]))
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    time1 = pErrorLimits->PositionTime1;

    if( (time1 <= errtime_range[Xlow]) ||
        (time1 > errtime_range[Xhi]) )
    {
        //**LS 7/11/06
        //pErrorLimits->fPositionTime1 = DEFAULT_fPositionTime1;
        return ERR_INVALID_PARAMETER;
    }

    //Put data in place
    Struct_Copy(ErrorLimits_t, &m_ErrorLimits, pErrorLimits);
    /* write the data to fram */
    return ram2nvramAtomic(NVRAMID_ErrorLimits);
}

/* This line marks the end of the source */
