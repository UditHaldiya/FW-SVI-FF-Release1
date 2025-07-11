/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file aotab.c
    \brief Configuration tables of AO channels

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/sysio/aotab.c $
    $Date: 3/23/09 3:29p $
    $Revision: 12 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* (Optional) $History: aotab.c $
 * 
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 3/23/09    Time: 3:29p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Corrected output range checking and added OUTPUT_SATURATED fault
 * definition
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 3/10/09    Time: 3:28p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * R/W output direction and ranges reimplemented and extracted
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 3/07/09    Time: 12:53p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * First shot at synchronized user trim of AO1 and signal readback:
 * Added sync counter (tag) to trim data
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:41a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * A fix for output signal ranges.
 * Perhaps temporary: same limits for normal output as for fixed output,
 * which may turn off the sensor. TBD.
 *
 * *****************  Version 8  *****************
 * User: Sergey Kruss Date: 12/14/08   Time: 11:19a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Cleaned out commented code only.
 *
 * *****************  Version 7  *****************
 * User: Sergey Kruss Date: 11/04/08   Time: 9:50a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * 1) Removed the 8mA lower limit of Cmd_40
 * 2) Fixed the display of Feedback current by Cmd_221 (went from UINT4 to
 * INT)
 * 3) Removed redundant calibration of fixed AO
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 10/17/08   Time: 6:42p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Removed AO-specific scaling. Now all AO are represented like signal.
 * Opens a way to optimize the AO table, too.
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 10/07/08   Time: 1:40a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Denis's limits brought from "Var Dictionary"
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 9/22/08    Time: 4:51p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added a member pointer to read last raw output
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 8/07/08    Time: 10:30p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * FIxed a scaling bug clamping the signal output to 4 mA
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 2:07p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Light linting
 * AO_1 ranges corrected
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 9:21p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * Added multiple AO channels support with table-driven configuration
 * AO interfaces became ao_ (from sysio_) and take AO id as a paramter
*/
#include "mnwrap.h"
#include "ao.h"
#include "da.h"
#include "tempcomp.h"
#if AO_NUM_CHANNELS > 1
#include PWM_H_
#endif


//conversions for range checking
#define STD_FROM_COUNTS(x) (x)  //just for consistency - no conversion

#define AO_MAX_DIFFERENCE   (AO_FP_LIMIT_HIGH / 5) //! Limit in mA how far cal value can walk away from nominal





TEMPCOMP_C_ENTRY(ao0) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_COUNTS(-100),
        [Xhi] = STD_FROM_COUNTS(5000),
    },
    .fcode = FAULT_NONE,
    .ErrLimit = ERRLIMIT_DEFAULT,
};

#if AO_NUM_CHANNELS > 1
TEMPCOMP_C_ENTRY(ao1) =
{
    .AllowedValue = //If not sure what you are doing, don't change this!
    {
        [Xlow] = STD_FROM_COUNTS(-100),
        [Xhi] = STD_FROM_COUNTS(MAX_DA_VALUE+1),
    },
    .fcode = FAULT_NONE,
    .ErrLimit = ERRLIMIT_DEFAULT,
};
#endif

/* We need to compute the desired loop current to map [LRV, URV] in "soft stops" domain
to the loop current domain, using user calibrations.
Then we need to back-calculate the PWM value that produces the loop current we computed.
Finally, the PWM value is tempcomp'ed and output
*/

/* Let's use the same approach as in osubrange.c to AO user calibration.
If we output x after (=without) user cal and see X on the wire, we know that to output X
we need to pretend (by user calibration) that we are writing x. I.e. assuming linear inter-/extrapolation, if
a ~= A and b ~= B were observed,
x = a + (X-A)*(b-a)/(B-A)
where X is what we intend to see on the wire and x is our “user-calibrated” value which promises to produce X.
Again, the computation could be refactored to the form x = m*X + b but we don't do it to avoid an extra
integer rounding error
*/
#define AO_USERCAL_SCALE_SHIFT MIN(30, ((30 - INTLOG(AO_STD_RANGE)) + INTLOG(AO_USERCAL_MIN_SPAN)))
#define AO_USERCAL_MIN_SPAN STD_FROM_MA(AO_CAL_DIF)



const aotab_t aotab[AO_NUM_CHANNELS] =
{
    [0] = //secondary (retransmit) output
    {
        .writef = bios_WriteDa,
        .readf = bios_GetLastDa,
        .tcorr = ao0_TempComp,
        .output_hw_range =
        {
            [Xlow] = AO_LIMIT_LOW,
            [Xhi] = AO_LIMIT_HIGH,
        },
        .aoNormal_range =
        {
            [Xlow] = STD_FROM_MA(3.8),
            [Xhi] = STD_FROM_MA(20.5),
        },
        .aoFixed_range =
        {
            [Xlow] = MAX(STD_FROM_MA(AO_FP_LIMIT_LOW), AO_SIGNAL_LOW_LIMIT),
            [Xhi] = STD_FROM_MA(AO_FP_LIMIT_HIGH),
        },
        .maxCalDifference = STD_FROM_MA(AO_MAX_DIFFERENCE),
        .CalMinRange_Raw = AO_USERCAL_MIN_SPAN,
        .aoUsercal_scaleShift = AO_USERCAL_SCALE_SHIFT,
        .ao_CalLimits =
        {
            [Xlow] =
            {
                [Xlow] = STD_FROM_MA(AO_CAL_LOW_LOW),
                [Xhi] = STD_FROM_MA(AO_CAL_LOW_HIGH),
            },
            [Xhi] =
            {
                [Xlow] = STD_FROM_MA(AO_CAL_HIGH_LOW),
                [Xhi] = STD_FROM_MA(AO_CAL_HIGH_HIGH),
            },
        },
    },
#if AO_NUM_CHANNELS > 1
    [1] = //primary (loop current) output
    {
        .writef = bios_WritePwm,
        .readf = pwm_GetValue,
        .tcorr = ao1_TempComp,
        .output_hw_range =
        {
            [Xlow] = MIN_DA_VALUE,
            [Xhi] = MAX_DA_VALUE,
        },
        .aoNormal_range = //normal operating range - Denis's limits
        {
            [Xlow] = STD_FROM_MA(3.8),
            [Xhi] = STD_FROM_MA(20.5),
        },
        .aoFixed_range =
        {
            [Xlow] = STD_FROM_MA(3.6), //Denis's limits
            [Xhi] = STD_FROM_MA(23.0),
        },
        .maxCalDifference = STD_FROM_MA(AO_MAX_DIFFERENCE),
        .CalMinRange_Raw = AO_USERCAL_MIN_SPAN,
        .aoUsercal_scaleShift = AO_USERCAL_SCALE_SHIFT,
        .ao_CalLimits =
        {
            [Xlow] =
            {
                [Xlow] = STD_FROM_MA(AO_CAL_LOW_LOW),
                [Xhi] = STD_FROM_MA(AO_CAL_LOW_HIGH),
            },
            [Xhi] =
            {
                [Xlow] = STD_FROM_MA(AO_CAL_HIGH_LOW),
                [Xhi] = STD_FROM_MA(AO_CAL_HIGH_HIGH),
            },
        },
    },
#endif
};

/* This line marks the end of the source */
