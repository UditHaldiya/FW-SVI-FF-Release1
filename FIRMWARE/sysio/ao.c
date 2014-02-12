/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file configure1.c
    \brief API and implementation of Analog Output

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/sysio/ao.c $
    $Date: 6/23/09 12:36p $
    $Revision: 34 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* (Optional) $History: ao.c $
 * 
 * *****************  Version 34  *****************
 * User: Arkkhasin    Date: 6/23/09    Time: 12:36p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Repaired for MNCB; no change in DLT
 *
 * *****************  Version 33  *****************
 * User: Arkkhasin    Date: 6/19/09    Time: 2:47p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * 1. Signal generator is not subject to clamping
 * 2. A bug of duplicate application od user trim is fixed
 * 3. Comment on use of 3.75mA clamp added
 *
 * *****************  Version 32  *****************
 * User: Arkkhasin    Date: 6/12/09    Time: 3:42p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added clamping (from below only) for normal AO1 output after user trim
 * applied
 *
 * *****************  Version 31  *****************
 * User: Arkkhasin    Date: 6/08/09    Time: 11:08p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * A preliminary fix of bug 1069 (AO trim). Span trim still needs work
 *
 * *****************  Version 30  *****************
 * User: Arkkhasin    Date: 5/29/09    Time: 2:35a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Corrected HART response codes using Error forming utilities
 *
 * *****************  Version 29  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 6:01p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added ao_GetAOValueTrimmed()
 *
 * *****************  Version 28  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:11p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Removed wrapper header utility.h
 *
 * *****************  Version 26  *****************
 * User: Arkkhasin    Date: 3/10/09    Time: 3:28p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * R/W output direction and ranges reimplemented and extracted
 *
 * *****************  Version 25  *****************
 * User: Arkkhasin    Date: 3/07/09    Time: 12:55p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * First shot at synchronized user trim of AO1 and signal readback:
 * Signal conf separated from trim; added trim storage modules for DLT and
 * MNCB
 * Added sync counter (tag) to trim data
 *
 * *****************  Version 24  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:41a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * A fix for output signal ranges.
 * Perhaps temporary: same limits for normal output as for fixed output,
 * which may turn off the sensor. TBD.
 *
 * *****************  Version 23  *****************
 * User: Arkkhasin    Date: 1/26/09    Time: 2:11p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * A fix for fixed AO limits
 *
 * *****************  Version 22  *****************
 * User: Arkkhasin    Date: 1/26/09    Time: 12:27a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * An attempt to repair the DLT sensor power-up sequence
 *
 * *****************  Version 21  *****************
 * User: Arkkhasin    Date: 1/16/09    Time: 12:43a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * lmon_IsSensorOn test went from ao.c to levelxmit.c (still tentative)
 *
 * *****************  Version 20  *****************
 * User: Sergey Kruss Date: 1/15/09    Time: 9:54a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Fixed for Lint
 *
 * *****************  Version 19  *****************
 * User: Sergey Kruss Date: 1/14/09    Time: 8:13p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Moved direction setting to Cmd_170/171 and removed separate flag field
 * from NVRAM
 *
 * *****************  Version 18  *****************
 * User: Sergey Kruss Date: 1/08/09    Time: 6:23p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Returned direction flag in Ernie's modification.
 *
 * *****************  Version 17  *****************
 * User: Ernieprice   Date: 1/08/09    Time: 4:44p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Hooks for loop current monitoring and AD power switch control
 *
 * *****************  Version 16  *****************
 * User: Sergey Kruss Date: 1/06/09    Time: 2:53p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added current direction
 *
 * *****************  Version 15  *****************
 * User: Sergey Kruss Date: 11/14/08   Time: 10:38a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Returned commented out calibration block. It may be used later.
 *
 * *****************  Version 14  *****************
 * User: Arkkhasin    Date: 11/11/08   Time: 2:01p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Allowed direct AO control in failsafe
 *
 * *****************  Version 13  *****************
 * User: Sergey Kruss Date: 11/04/08   Time: 9:50a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * 1) Removed the 8mA lower limit of Cmd_40
 * 2) Fixed the display of Feedback current by Cmd_221 (went from UINT4 to
 * INT)
 * 3) Removed redundant calibration of fixed AO
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 10/07/08   Time: 1:38a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * AO freeze API simplified
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 9/22/08    Time: 4:55p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added direct-control of (both) ao
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 8/07/08    Time: 10:30p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * FIxed a scaling bug clamping the signal output to 4 mA
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 2:04p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Light linting
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 9:16p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added multiple AO channels support with table-driven configuration
 * AO interfaces became ao_ (from sysio_) and take AO id as a paramter
*/
#include "mnwrap.h"
#include "errcodes.h"
//#include "mnassert.h"
#include "mncbtypes.h"
#include "nvram.h"
#include "param.h"
#include "faultpublic.h"
#include "ao.h"
#include "aosig.h"

/** member data */
static AOCalibration_t m_AOCalibration[AO_NUM_CHANNELS];
static AOComputedCalibration_t m_AOComputedCalibration[AO_NUM_CHANNELS];

typedef struct AOState_t
{
    ao_t m_AOValueTrimmed; //!< last written AO value with user trim applied
    ao_t m_AOValue; //!< last written AO value
    bool_t m_bAOFixedOutput; //!< indicator of fixed AO
    bool_t DirectControl; //!< indicator of direct output control
    u16 DirectControlVal; //!< value to output in case of direct control
    u16 CheckWord;
} AOState_t;

static AOState_t AOState[AO_NUM_CHANNELS] =
{
    {
        .m_AOValueTrimmed = 0,
        .m_AOValue = 0,
        .m_bAOFixedOutput = false,
        .DirectControl = false,
        .DirectControlVal = 0,
        .CheckWord = (u16)~0U, //to ensure valid checksum on C init
    },
#if AO_NUM_CHANNELS > 1
    {
        .m_AOValueTrimmed = 0,
        .m_AOValue = 0,
        .m_bAOFixedOutput = false,
        .DirectControl = false,
        .DirectControlVal = 0,
        .CheckWord = (u16)~0U, //to ensure valid checksum on C init
    },
#endif
};

const AOCalibration_t def_AOCalibration = //! default user calibrations - don't need to be per channel
{
    .AOCal =
    {
        [Xlow] = STD_FROM_MA(DEFAULT_fLowAOCal),
        [Xhi] = STD_FROM_MA(DEFAULT_fHighAOCal),
    },
    .nAOCalAD =
    {
        [Xlow] = STD_FROM_MA(DEFAULT_fLowAOCal),
        [Xhi] = STD_FROM_MA(DEFAULT_fHighAOCal),
    },
    .SeqNumber = 0,
    CHECKFIELD_CLEAR()
};


/** \brief Enables/disables direct control of the output.
Used primarily for calibration and testing
\param index - ao channel number
\param enable - true if direct control
\param DirectControlVal - value to output directly (if enabled)
\return an error code
*/
ErrorCode_t ao_SetDirectControl(u8_least index, bool_t enable, u16 DirectControlVal)
{
    if(index >= NELEM(AOState))
    {
        return ERR_INVALID_PARAMETER;
    }
    if(enable && !error_IsFault(FAULT_FACTORYMODE))
    {
        return ERR_MODE_ILLEGAL_SUBMODE;
    }
    MN_ENTER_CRITICAL();
        storeMemberInt(&AOState[index], DirectControl, enable);
        if(enable)
        {
            storeMemberInt(&AOState[index], DirectControlVal, DirectControlVal);
            aotab[index].writef(AOState[index].DirectControlVal);
        }
    MN_EXIT_CRITICAL();
    return ERR_OK;
}

/* private function prototypes */
static ErrorCode_t  ao_CalculateAOComputedCalibration(u8_least index, AOComputedCalibration_t* pAOComputedCalibration,
            const AOCalibration_t* pAOCalibration);



/** \brief Monitors/cancels whether AO direct control is allowed
\param index - one of the predefined AO channels
\return true iff direct control is in effect
*/
static bool_t ao_DirectControlMon(u8_least index)
{
    bool_t DirectControl = AOState[index].DirectControl;
    if(DirectControl)
    {
        if(error_IsFault(FAULT_FACTORYMODE))
        {
            //OK; it's allowed
            aotab[index].writef(AOState[index].DirectControlVal);
        }
        else
        {
            //Force cancel direct control
            (void)ao_SetDirectControl(index, false, 0);
            DirectControl = false;
        }
    }
    return DirectControl;
}

/** \brief get pointer to the computed user calibration for position retransmit
    \return pointer to m_AOComputedCalibration
*/
static const AOComputedCalibration_t *ao_GetAOComputedCalibration(u8_least index, AOComputedCalibration_t *dst)
{
    return STRUCT_TESTGET(&m_AOComputedCalibration[index], dst);
}

/** \brief applies the user calibration to the D/A
    \param AOValue - the value to correct
    \param pAOComputedCalibration - user calibration factors
    \return the result to send to the d/a
*/
static ao_least_t ao_AOUserCal(u8_least index, ao_least_t AOValue, const AOComputedCalibration_t* pAOComputedCalibration)
{
    ao_least_t PositionAO;

    PositionAO = intscale32(pAOComputedCalibration->Span,
        AOValue - pAOComputedCalibration->PreOffset,
        pAOComputedCalibration->PostOffset,
        aotab[index].aoUsercal_scaleShift);

    //AK:TODO: prove it fits s16
    return PositionAO;
}


/** \brief Sets the analog output with bells and whistles

This routine applies user calibrations if requested and the temperature correction
and outputs the result. It may freeze the output if requested (but cannot unfreeze).
\param index - AO channel number
\param[in] AOValue - AO value in internal representation
\param usrcal - a flag which if true applies user calibration
\param freeze - if true, freezes the output at AOValue
*/
static void ao_SetAOValue_Internal(u8_least index, ao_least_t AOValue, bool_t usrcal, bool_t freeze)
{
    tcorr_t CorrectedAOValue;
    u16 UnsignedCorrectedAOValue;

    if(ao_DirectControlMon(index))
    {
        return; //ignore the request
    }

    if(index >= NELEM(AOState))
    {
        return; //ignore the request
    }

    bool_t oldfreeze = ao_IsFixedAO(index);
    if(oldfreeze && !freeze)
    {
        /* set to last value */
        AOValue = ao_GetAOValue(index);
    }
    else
    {
        //Save the value BEFORE applying user trim calibrations
        MN_ENTER_CRITICAL();
            storeMemberS16(&AOState[index], m_AOValue, (ao_t)AOValue);
            if(freeze)
            {
                ao_Freeze(index, freeze);
            }
        MN_EXIT_CRITICAL();
    }

    if(usrcal)
    {
        const AOComputedCalibration_t* pAOComputedCalibration;
        pAOComputedCalibration = ao_GetAOComputedCalibration(index, NULL);
        AOValue = ao_AOUserCal(index, AOValue, pAOComputedCalibration);

#if AO_NUM_CHANNELS > 1
        /*AK:RED_FLAG
        This is a DLT-specific brutal hack that needs to go away at the earliest opportunity.
        It attempts to ensure that normal output produces at least 3.6 mA on
        the readback input (tempcomped but before user trim) so that loopmon doesn't
        turn the sensor off.
        */
        if(!freeze && !oldfreeze)
        {
            /* Normal output may need to be clamped after user trim because
            it may have implications on power budget elsewhere.
            */
            if(index == AO_CHANNEL_LOOP_CURRENT)
            {
                AOValue = MAX(AOValue, STD_FROM_MA(3.75)); //AK:06-16-2009: Harry's preferred number, from previous 3.8
            }
        }
#endif //AO_NUM_CHANNELS > 1
    }
    MN_ENTER_CRITICAL();
        storeMemberS16(&AOState[index], m_AOValueTrimmed, (ao_t)AOValue);
    MN_EXIT_CRITICAL();

    Struct_Test(AOState_t, &AOState[index]); //a handy place to test data integrity

    /* correct for temperature */
    ADRaw_t aoval = (ADRaw_t)CLAMP(AOValue, 0, INT16_MAX); //use arbitrary harmless limits
    CONST_ASSERT(sizeof(ao_t) == sizeof(s16)); //prove harmlessness
    CorrectedAOValue = aotab[index].tcorr(aoval);

    /* limit the D/A value */
    UnsignedCorrectedAOValue = (u16)CLAMP(CorrectedAOValue, aotab[index].output_hw_range[Xlow], aotab[index].output_hw_range[Xhi]);
    /* send it to the D/A */
    aotab[index].writef(UnsignedCorrectedAOValue);
}

/** \brief Sets the analog output with bells and whistles

This routine applies user calibrations if requested and the temperature correction
and outputs the result.
\param index - AO channel number
\param[in] AOValue - AO value in internal representation
\param usrcal - a flag which if true applies user calibration
*/
void ao_SetAOValue(u8_least index, ao_least_t AOValue, bool_t usrcal)
{
    ao_SetAOValue_Internal(index, AOValue, usrcal, false);
}



/** \brief Sets the AO freeze flag
\param freeze: the flag value to set (true = frozen)
*/
void ao_Freeze(u8_least index, bool_t freeze)
{
    MN_ENTER_CRITICAL();
        storeMemberBool(&AOState[index], m_bAOFixedOutput, freeze);
    MN_EXIT_CRITICAL();
}


/** \brief Sets the D/A to a fixed value

  This routine converts a mA value to the proper scale to output to the D/A.
  It uses the user calibrations.
  \param[in] AOValue - the mA value (in internal representation) to set the D/A to
*/
ErrorCode_t ao_SetAOValueFixed(u8_least index, ao_least_t AOValue)
{
    ao_least_t lolim, hilim;

    if(index >= NELEM(AOState))
    {
        return ERR_INVALID_PARAMETER;
    }

    //range check
    lolim = aotab[index].aoFixed_range[Xlow];
    hilim = aotab[index].aoFixed_range[Xhi];

    ErrorCode_t err = err_CheckValue(AOValue, lolim, hilim);
    if(err != ERR_OK)
    {
        return err;
    }

    /* send the fixed output with user cals */
    ao_SetAOValue_Internal(index, AOValue, true, true);

    return ERR_OK;
}

/** \brief Returns the last value sent to the D/A

  \return a pre-temperature corrected value but with user calibration applied.
*/
ao_t ao_GetAOValue(u8_least index)
{
    return AOState[index].m_AOValue;
}

/** \brief Returns the last value sent to the D/A

  \return a pre-temperature corrected value but with user calibration applied.
*/
ao_t ao_GetAOValueTrimmed(u8_least index)
{
    return AOState[index].m_AOValueTrimmed;
}

/** \brief Reports direct control status (and value) of an AO channel
\param index - AO channel number
\param[out] value - a pointer to direct control value
\return true iff direct control (or invalid index)
*/
bool_t ao_IsDirectControl(u8_least index, u16 *value)
{
    if(index>=NELEM(AOState))
    {
        return true; //doesn't matter
    }
    if(value != NULL)
    {
        *value = AOState[index].DirectControlVal;
    }
    return AOState[index].DirectControl;
}

/** \brief Checks if AO is frozen by a HART command - or for internal reasons
\return true iff frozen for any reason
*/
bool_t ao_IsFixedAO(u8_least index)
{
    if(index>=NELEM(AOState))
    {
        return true;
    }
    return AOState[index].m_bAOFixedOutput;
}


/** \brief gets pointer to user calibration data for position retransmit
    \return AOCalibration_t* - pointer to the calibration data
*/
const void* TypeUnsafe_ao_GetAOCalibration(u8_least index, void* dst)
{
    if(index >= NELEM(m_AOCalibration))
    {
        return NULL;
    }
    return STRUCT_TESTGET(&m_AOCalibration[index], dst);
}


/** \brief sets the user calibration (trim) of an AO channel in RAM but not in NVMEM
    \param index - the id of the AO channel
    \param pAOCalibration - a pointer to new calibration data
    \return ERR_OK iff no error
*/
ErrorCode_t ao_SetWorkingAOCalibration(u8_least index, const AOCalibration_t* pAOCalibration)
{
    AOComputedCalibration_t AOComputedCalibration;

    if(pAOCalibration == NULL)
    {
        pAOCalibration = &def_AOCalibration;
    }

    //perfrom range checking
    ErrorCode_t err ;
    for(s8_least i=Xlow; i<Xends; i++)
    {
        err = err_CheckValue(pAOCalibration->AOCal[i],
                             aotab[index].ao_CalLimits[i][Xlow],
                             aotab[index].ao_CalLimits[i][Xhi]);
        if(err != ERR_OK)
        {
            /* Legitimate early return: Pre-requisites check */
            return err;
        }
    }

    //calculate
    err = ao_CalculateAOComputedCalibration(index, &AOComputedCalibration, pAOCalibration);
    if(err != ERR_OK)
    {
        return err;
    }

    ///move the data to real structures
    MN_ENTER_CRITICAL();
        Struct_Copy(AOCalibration_t, &m_AOCalibration[index], pAOCalibration);
        Struct_Copy(AOComputedCalibration_t, &m_AOComputedCalibration[index], &AOComputedCalibration);
    MN_EXIT_CRITICAL();

    return ERR_OK;
}

/** \brief sets the user calibration (trim) of an AO channel in RAM and NVMEM
    \param index - the id of the AO channel
    \param src - a pointer to new calibration data
    \return ERR_OK iff no error
*/
ErrorCode_t TypeUnsafe_ao_SetAOCalibration(u8_least index, const void *src)
{
    const AOCalibration_t* pAOCalibration = src;
    if(pAOCalibration == NULL)
    {
        pAOCalibration = &def_AOCalibration;
    }

    ErrorCode_t err = ao_SetWorkingAOCalibration(index, pAOCalibration);
    if(err != ERR_OK)
    {
        return err;
    }
    return aosig_WriteAOUserTrim(index, pAOCalibration);
}

/** \brief Performs a user calibration of the position retransmit
  \param[in] CalType - 0=low, 1=high
  \param[in] CalValue - the measured value
  \return ErrorCode_t - ERR_OK if no error
*/
ErrorCode_t  ao_Calibrate(u8_least index, u8 CalType, ao_t CalValue)
{
    ao_least_t CurrentAO, MeasuredAO;
    AOCalibration_t AOCalibration;

    if(CalType > Xhi)
    {
        return ERR_INVALID_PARAMETER;       // early return prerequisites check
    }

    //processing - get the last sent value
    MeasuredAO = CalValue;
    CurrentAO = ao_GetAOValueTrimmed(index);

    ErrorCode_t err;
    err = err_CheckValue(MeasuredAO,
                         CurrentAO - aotab[index].maxCalDifference,
                         CurrentAO + aotab[index].maxCalDifference);
    if(err != ERR_OK)
    {
        error_SetFault(FAULT_CALIBRATION_FAILED);
        return err;           // early return prerequisites check
    }

    //get the current calibration data
    (void)ao_GetAOCalibration(index, &AOCalibration);

    //fill in the new calibration data
    AOCalibration.AOCal[CalType] = CalValue;
    AOCalibration.nAOCalAD[CalType] = (ao_t)CurrentAO;
    AOCalibration.SeqNumber++;

    err = ao_SetAOCalibration(index, &AOCalibration);
    if(err != ERR_OK)
    {
        error_SetFault(FAULT_CALIBRATION_FAILED);
        return err;
    }

    return ERR_OK;
}


/** \brief computes the scaling for user calibration of position retransmit
    \param[out] pAOComputedCalibration - returned calculated user cal data
    \param pAOCalibration - user cal data
    \return ERR_OK iff no error
*/
static ErrorCode_t ao_CalculateAOComputedCalibration(u8_least index, AOComputedCalibration_t* pAOComputedCalibration,
            const AOCalibration_t* pAOCalibration)
{
    ao_least_t nLowAOCal, nHighAOCal, diff;

    nLowAOCal = pAOCalibration->AOCal[Xlow];
    nHighAOCal = pAOCalibration->AOCal[Xhi];

    diff = nHighAOCal - nLowAOCal;

    //perfrom range checking
    if(diff < aotab[index].CalMinRange_Raw)
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_HART_RV_SPAN_LOW;
    }

    //compute span and offset
    u8 shift = aotab[index].aoUsercal_scaleShift;
    s64 prescale = (s64)(1ULL << shift);
    pAOComputedCalibration->Span = //cast is OK due to the design of `shift` (see aotab.c)
        (s32)((prescale * (pAOCalibration->nAOCalAD[Xhi] - pAOCalibration->nAOCalAD[Xlow]))/diff);
    pAOComputedCalibration->PreOffset = pAOCalibration->AOCal[Xlow];
    pAOComputedCalibration->PostOffset = pAOCalibration->nAOCalAD[Xlow];
    return ERR_OK;
}

/* This line marks the end of the source */
