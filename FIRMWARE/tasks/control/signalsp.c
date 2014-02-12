/*
Copyright 2008 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file signalsp.c
    \brief Manages conversion of input signal to position setpoint w.r.t.
            split range and characterization.
            Replaces the short-lived convsig2sp.c

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/control/signalsp.c $
    $Date: 1/30/12 2:53p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup posctl
*/
/* $History: signalsp.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:53p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * Dead code removal (no code change) FBO TFS:8782
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:55a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 8/31/09    Time: 5:35p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Went back to delivering signal setpoint in adjusted position domain -
 * because that's where control limits are.
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 6:24p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * POS_LIMIT macro removed in favor of CLAMP
 * Removed obsolete conversion.{c,h}
 * Signal characterization calculation repaired
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 12:45a
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * sigsp_ConvertSignalToSetpoint yields setpoint in "hard stops" domain
 * (instead of previous convert_... which was aware of ATO/ATC and stops
 * adjustments)
 * sig_ConvertSignalToStd simply converts signal to percent of signal
 * range (instead of previous convert_... which was aware of ATO/ATC)
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:44a
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:33p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 * Module initilaization table types - ModuleDataItem_t and ModuleData_t
 * removed
 * ram2nvram[Atomic] do not take a pointer to the buffer anymore, but now
 * return an error code
 * Also: Fixed a bug in SignalLimits_range initialization
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/17/08    Time: 4:56p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Second step toward uniform support of engineering units:
 * Valid range limits for any parameter are owned by a corresponding
 * module and exposed as an array of 2 elements (low and high limits)
 * THERE ARE POTENTIAL INCONSISTENCIES HERE: REVISIT!
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/21/08    Time: 3:38p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Misplaced RAM test removed
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/15/08    Time: 3:37p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Oops. Now computes computed signal data on iniialization. (Needed a
 * Set, not a Test, function)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 6:22p
 * Created in $/MNCB/Dev/FIRMWARE/posctl
 * A new file that takes care of signal to position setpoint conversion.
 * Includes the (modified) content of the removed convsig2sp.c
*/

#include "mnwrap.h"
#include "mncbtypes.h"
#include "position.h"
#include "signalsp.h"
#include "nvram.h"
#include "faultpublic.h"
#include "poscharact.h"

#define F_LOW_SIGNAL_DEF (4.0F)
#define F_HIGH_SIGNAL_DEF (20.0F)

static SignalData_t            m_SignalData;
static ComputedSignalData_t    m_ComputedSignalData;

/** Default configuration - no split range */
static const SignalData_t  def_SignalData =
{
    .LowSignal     = STD_FROM_MA(F_LOW_SIGNAL_DEF),                //(4.0F)
    .HighSignal    = STD_FROM_MA(F_HIGH_SIGNAL_DEF),               //(20.0F)
    CHECKFIELD_CLEAR()
};

const sig_t SignalLimits_range[Xends][Xends] =
{
    [Xlow] = //lower limit range
    {
        [Xlow] = LOW_SIGNAL_LOW_LIMIT_INT, //lower
        [Xhi] = LOW_SIGNAL_HIGH_LIMIT_INT, //upper
    },
    [Xhi] = //upper limit range
    {
        [Xlow] = HIGH_SIGNAL_LOW_LIMIT_INT, //lower
        [Xhi] = HIGH_SIGNAL_HIGH_LIMIT_INT, //upper
    },
};

const sig_t SignalSP_range[Xends] = //range of signal setpoint - the whole wide range
{
    [Xlow] = LOW_SIGNAL_LOW_LIMIT_INT, //lower
    [Xhi] = HIGH_SIGNAL_HIGH_LIMIT_INT, //upper
};

/** \brief Tests the validity of the supplied range values
  \param[in] sd - a pointer to a signal configuration structure
  \return ERR_OK if no error, an error code otherwise
*/
static ErrorCode_t    cnfg_TestSignalData(const SignalData_t *sd)
{
    /** validate input */
    if( (sd->LowSignal < SignalLimits_range[Xlow][Xlow]) ||
        (sd->LowSignal > SignalLimits_range[Xlow][Xhi])||
        (sd->HighSignal < SignalLimits_range[Xhi][Xlow])||
        (sd->HighSignal > SignalLimits_range[Xhi][Xhi]) )

    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    if( (sd->HighSignal - sd->LowSignal) < SIGNAL_DIFFERENCE_INT )
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_CNFG_SIGNAL_RANGE;
    }

    return ERR_OK;
}

#define SIGNAL_STD_SHIFT 16u
#define SIGNAL_STD_SCALE ((s32)(1U<<SIGNAL_STD_SHIFT))


/** \brief Calculates the signal conversion factors

  Calculates the signal scaling factors used
  by conversion of signal to percentage setpoint.

  Note:  This routine is called after setting some other data.  It must be
  set atomically but is the responsibility of the calling function

  \param[out] pComputedSignalData - the structure to put the results -
        usually the member structure
  \param[in] pSignalData - a pointer to signal data
*/
static void     cnfg_CalculateComputedSignalData(ComputedSignalData_t* pComputedSignalData,
            const SignalData_t* pSignalData)
{
    ///calculate nLowSignal and nHighSignal
    pComputedSignalData->nLowSignal = pSignalData->LowSignal;
    pComputedSignalData->nHighSignal = pSignalData->HighSignal;

    ///calculate the conversion to standard scale factor
    pComputedSignalData->SlopeStandard =
        (SIGNAL_STD_SCALE  * STANDARD_RANGE)
        / (pComputedSignalData->nHighSignal - pComputedSignalData->nLowSignal);
}


/** \brief Sets the supplied range values in RAM (and NVMEM if RTOS is running)
  \param[in] sd - a pointer to a signal configuration structure
  \return ERR_OK if no error, an error code otherwise
*/
ErrorCode_t TypeUnsafe_cnfg_SetSignalData(const void *src)
{
    const SignalData_t *sd = src;
    if(sd == NULL)
    {
        sd = &def_SignalData;
    }
    //SignalData_t sd = {.LowSignal=nLowSignal, .HighSignal=nHighSignal};
    ErrorCode_t err = cnfg_TestSignalData(sd);
    if(err == ERR_OK)
    {
        ComputedSignalData_t ComputedSignalData;
        /** compute low and high interger signal ranges*/
        cnfg_CalculateComputedSignalData(&ComputedSignalData, sd);

        MN_ENTER_CRITICAL();
            Struct_Copy(SignalData_t, &m_SignalData, sd);
            Struct_Copy(ComputedSignalData_t, &m_ComputedSignalData, &ComputedSignalData);
        MN_EXIT_CRITICAL();

        err = ram2nvramAtomic(NVRAMID_SignalData);
    }
    return err;
}

/** \brief returns the computed signal data

  returns the computed signal data
  \return A pointer to the data requested
*/
const ComputedSignalData_t *sig_GetComputedSignalData(void)
{
    return STRUCT_TESTGET(&m_ComputedSignalData, NULL);
}

/** \brief returns the signal data
  \return a pointer to signal data
*/
const void* TypeUnsafe_cnfg_GetSignalData(void *dst)
{
    return STRUCT_TESTGET(&m_SignalData, dst);
}

/** \brief Convert signal to percent of the signal range according to the configured ranges
\param n2Signal - signal to convert
\return A linearly mapped value in the percent range
*/
percent_least_t sig_ConvertSignalToStd(sig_t n2Signal)
{
    pos_least_t n4Output;
    //args
    //Input Range Validation - none

    //Processing
    n4Output = intscale32(m_ComputedSignalData.SlopeStandard, n2Signal - m_ComputedSignalData.nLowSignal, STANDARD_ZERO, SIGNAL_STD_SHIFT);

    //Output Range Validation
    return CLAMP(n4Output, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION);
}


/**
\brief Converts a signal to position setpoint in "hard stops" domain.
\param n2Signal – The temperature corrected, signal A/D reading.
\return the position setpoint in "hard stops" domain in standard range (characterized)
*/
s32 sigsp_ConvertSignalToSetpoint(sig_t n2Signal)
{
    s32 out;
    //Input Range Validation: - none
    //Processing
    out = sig_ConvertSignalToStd(n2Signal);
    /* Percent of signal range is treated as a percent of position range in
    the adjusted (soft stops) domain; by Masoneilan convention 100% signal corresponds
    to the energized limit, and 0% to the de-energized limit, i.e.
    ATO 0% ~ 0% pos, 100% ~ 100% pos
    ATC 0% ~ 100% pos, 100% ~ 0% pos
    To apply (flow) characterization, we need to do it on the scaled value before
    mapping this to the un-adjusted (full-travel) range of position; this is how
    characterization is defined (= mapping of valve movement range, not the actuator
    movement range. Stops find the latter, stop adjustments define the former.
    However, we do need to map to open/close range vs. energized/de-energized before
    applying the characterization.
    */
    bool_t bATO = pos_GetComputedPositionConf(NULL)->direct_adjusted;
    if(!bATO)
    {
        out = STANDARD_100 - out;
    }
    out = poscharact_Direct(out);
#if 0
    if(!bATO)
    {
        out = STANDARD_100 - out;
    }

    out = pos_UnAdjust(out);
    /* Now `out' maps correctly to the "hard stops" domain where 100% is the energized end
    */
#endif
    return out;
}
/* This line marks the end of the source */
