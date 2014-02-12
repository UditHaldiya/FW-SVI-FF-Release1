/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file position.c
    \brief System-level data and services related to position measurement

Valve position (or, for DLT, level) is computed in several steps of which this file
prepresents the last one (and overal organization).
1. Sensor-specific raw data compensation
2. Sensor-specific linearization
3. Application-specific sensor-independent scaling to the range ("stops"), which may need
    to account for stops drift/adjustment

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/position.c $
    $Date: 1/09/12 3:17p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: position.c $
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 3:17p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Added OpenStopAdj_range for UI wiring FBO TFS:8577
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/17/11   Time: 11:01a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8202 decouple I/O subsystem - vpos_GetRawPosition corrected;
 * ExtraPos... corrected
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:19a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for position calculations
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 25  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 7:27p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Removed cnfg_TestPositionStopEqualDefault per manufacturing practice
 * and Ernie's recommendation
 *
 * *****************  Version 24  *****************
 * User: Arkkhasin    Date: 1/04/10    Time: 11:32a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Fixed a bug in getting stop adjustment
 *
 * *****************  Version 23  *****************
 * User: Arkkhasin    Date: 8/06/09    Time: 11:56a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Restored MNCB builds (broken by latest DLT changes)
 *
 * *****************  Version 22  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 3:18a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Removed unused headers
 *
 * *****************  Version 21  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 8:39p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * For MNCB, reverted to ver. 11 (with bug fixes)
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 12/04/08   Time: 1:08p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Position calibration is now driven by a state machine ignorant of
 * position/level implementation details
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 10/13/08   Time: 12:27a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Added asserts
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 2:13p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Light linting
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 6:08p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Removed obsolete conversion.{c,h}
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 7/07/08    Time: 10:30p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Fixing newly introduced bugs:
 * - corrected the definition of unadjust scaler
 * - corrected computation of adjustment direction flag (a.k.a. bATO)
 * - fixed computation of the filter state
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 7/04/08    Time: 9:00p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Position-related code refactored as follows:
 * position (or, for DLT, level) is computed in several steps
 * 1. Sensor-specific raw data compensation
 * 2. Sensor-specific linearization
 * 3. Application-specific sensor-independent scaling to the range
 * ("stops"), which may need
 *     to account for stops drift/adjustment
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 5:27p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * (Preliminarily) adapted to new tempcomp
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 4/09/08    Time: 11:57p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Partial rearrangement of source files by modules
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/22/08    Time: 12:56p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * convert_ConvertPositionToStd has now pos_t type
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/15/08    Time: 6:10p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Updated for new API
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 11:27p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * New sysio interfaces extracted from various places for better
 * encapsulation and modularization
*/
#include "mnwrap.h"
#include "crc.h"
#include "param.h"
#include "errcodes.h"
#include "position.h"
#include "smoothing.h"
#include "nvram.h"
#include "mnassert.h"


#define EXTRA_POS_CLOSED  INT_PERCENT_OF_RANGE(1.0)     //160
#define EXTRA_POS_OPEN  INT_PERCENT_OF_RANGE(3.0)       //492

const percent_t OpenStopAdj_range[Xends] =
{
    OPENSTOP_ADJ_MIN,
    OPENSTOP_ADJ_MAX
};

const pos_t Position_FullRange[Xends] =
{
    MIN_ALLOWED_POSITION,
    MAX_ALLOWED_POSITION
};


static PositionConf_t m_PositionConf; //! Position or level configuration
static ComputedPositionConf_t m_ComputedPositionConf; //! Pre-processed position or level configuration
static adjpos_t pos_AdjustStdPosition(pos_least_t pos);


const ComputedPositionConf_t *pos_GetComputedPositionConf(ComputedPositionConf_t *dst)
{
    return STRUCT_TESTGET(&m_ComputedPositionConf, dst);
}

/** \brief returns pointer to and fills the position/level configuration
  \return A pointer to the position/level configuration
*/
const void *TypeUnsafe_pos_GetPositionConf(void *dst)
{
    return STRUCT_TESTGET(&m_PositionConf, dst);
}

//These should probably be expanded:
#define POS_SCALEDMIN INT_PERCENT_OF_RANGE(0.0)
#define POS_SCALEDMAX INT_PERCENT_OF_RANGE(100.0)

#define POS_SCALE_DIFF_MIN INT_PERCENT_OF_RANGE(60.0)

static ErrorCode_t pos_TestPositionConf(const PositionConf_t* pPositionConf)
{
    /** validate the input */
    if(pPositionConf == NULL)
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    /* PositionRange values must be within limits and range-separated.
       The values involved must address the reolution requirements as well as
       (non-)overflow of the resulting computation vs. any participating container sizes
    */
    pos_t a = pPositionConf->PositionRange[Xlow];
    pos_t b = pPositionConf->PositionRange[Xhi];
    if((a < POS_SCALEDMIN) || (b > POS_SCALEDMAX) || ((b-a)<POS_SCALE_DIFF_MIN) )
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    if( (mn_abs((pPositionConf->rangeval[Xhi] - pPositionConf->rangeval[Xlow])) >= STOPS_MIN_RANGE) )
    {
        /* valid */
    }
    else
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_CNFG_STOPS_RANGE;
    }


    return filter_TestFilter(&pPositionConf->filtercoef);
}

    /* 1. MNCB "find stops" or DLT "calibrations" provide two numbers,
    A = pPositionConf.PositionStop[Xlow]
    and B = pPositionConf.PositionStop[Xhi],
    in the input voodoo domain (raw A/D +tempcomp +linearize) (32-bit for at least DLT), corresponding to 0% and 100%,
    so the value x in the app level domain is mapped from a voodoo value X as
    x = (X-A)/(B-A)*STANDARD_RANGE + STANDARD_ZERO.
    We precompute the slope 1/(B-A)*STANDARD_RANGE here as a scaled number
    Division by B-A consumes, at least, INTLOG(STOPS_MIN_RANGE) bits;
    Multiplying by STANDARD_RANGE adds INTLOG(STANDARD_RANGE) bits,
    so the best shift for intscale32 is 30 - INTLOG(STANDARD_RANGE) + INTLOG(STOPS_MIN_RANGE)
    */
#define POS_SCALE_SHIFT MIN(30, ((30 - INTLOG(STANDARD_RANGE)) + INTLOG(STOPS_MIN_RANGE)))
#define POS_SCALE_MULTIPLIER ((s64)(1ULL<<POS_SCALE_SHIFT))
    /* 2. user range calibrations (distinct from presentation layer units calibrations)
    a = PositionRange[Xlow] and
    b = PositionRange[Xhi] in the app level domain map [a, b] to [0%, 100%]
    (compare to MNCB open/closed "stop adjustments"), so that the scaled value
    used by the app is
    u = (x-a)/(b-a)* STANDARD_RANGE + STANDARD_ZERO.
    It is probably unwise to wrap this mapping in the previous calculation because changing it
    several times will lead to error accumulation.
    So we need to keep this calculation separate; we compute the second slope and copy the datum a
    Division by b-a consumes, at least, INTLOG(POS_SCALE_DIFF_MIN) bits;
    so the best shift for intscale32 is 30 - INTLOG(STANDARD_RANGE) + INTLOG(POS_SCALE_DIFF_MIN)
    */
#define POS_SCALEADJ_SHIFT MIN(30, ((30 - INTLOG(STANDARD_RANGE)) + INTLOG(POS_SCALE_DIFF_MIN)))
#define POS_SCALEADJ_MULTIPLIER ((s64)(1ULL<<POS_SCALEADJ_SHIFT))
    /* 3. There is also a need to convert from the range-calibrated domain to the hard-stops domain.
    E.g. signal setpoint converted to the position setpoint is thought of as in range-calibrated
    domain.
    We have
    x = a + (u - STANDARD_ZERO)*(b-a)/STANDARD_RANGE
    */
#define POS_SCALE_UNADJ_SHIFT MIN(30, ((30 + INTLOG(STANDARD_RANGE)) - INTLOG(POS_SCALE_DIFF_MIN)))
#define POS_SCALE_UNADJ_MULTIPLIER ((s64)(1ULL<<POS_SCALE_UNADJ_SHIFT))


/** \brief compute soft stops and position scale factor

  compute soft stops and position scale factor
  \param[out] pComputedPositionStop - a pointer to the newly-computed position stop data
  \param[in] pPositionStop - a pointer to stored position stop data
*/
static void pos_CalculateComputedPositionConf(ComputedPositionConf_t* pComputedPositionConf,
            const PositionConf_t* pPositionConf)
{
    s64 slope;
    s64 stopsdiff = (s64)pPositionConf->rangeval[Xhi] - (s64)pPositionConf->rangeval[Xlow];
    slope = (POS_SCALE_MULTIPLIER*STANDARD_RANGE)/stopsdiff;

    MN_DBG_ASSERT((slope <= INT32_MAX) && (slope >= INT32_MIN)); //That's how the multipliers are chosen
    pComputedPositionConf->slope_scaled = (s32)slope;

    pComputedPositionConf->offset_scaled = pPositionConf->rangeval[Xlow];

    pos_t a = pPositionConf->PositionRange[Xlow];
    pos_t b = pPositionConf->PositionRange[Xhi];
    /*So, a=closed stop adjustment, b=open stop adjustment;
    */
    //For ATC, transpose it (Masoneilan convention on position)
    if(!pPositionConf->bATO)
    {
        a = STANDARD_100 - a;
        b = STANDARD_100 - b;
    }
    /* Now,
    a = deenrgized std stop value corresponding to STANDARD_ZERO in adjusted domain,
    b = energized std stop value corresponding to STANDARD_100 in adjusted domain
    From raw mapped domain deenergized STANDARD_ZERO~=0%, energized STANDARD_100~=100%
    to adjusted to closed/open stop adjustments is
    x = STANDARD_ZERO + STANDARD_RANGE/(b-a) * (y - a)
    and of course, the transformation back from adjusted to raw mapped domain is
    y = a +  (b-a)/STANDARD_RANGE * (x - STANDARD_ZERO)
    The rest is pre-computing the coefficent making these calculations efficient
    in fixed point; basically, shoehorning them for intscale32
    */
    slope = (POS_SCALEADJ_MULTIPLIER*STANDARD_RANGE)/(b - a);
    MN_DBG_ASSERT((slope <= INT32_MAX) && (slope >= INT32_MIN)); //That's how the multipliers are chosen
    pComputedPositionConf->slope_adjusted = (s32)slope;

    pComputedPositionConf->offset_adjusted = a;

    slope = (POS_SCALE_UNADJ_MULTIPLIER*(b - a))/STANDARD_RANGE;
    MN_DBG_ASSERT((slope <= INT32_MAX) && (slope >= INT32_MIN)); //That's how the multipliers are chosen
    pComputedPositionConf->slope_unadjust = (s32)slope;

    /*  As a courtesy to the app layer, store (cache) the mapping direction for scaled and adjusted versions
    */
    pComputedPositionConf->direct_scaled = (pPositionConf->rangeval[Xhi] > pPositionConf->rangeval[Xlow]);
    pComputedPositionConf->direct_adjusted = pPositionConf->bATO;
    //backward-compatible kludge
    pComputedPositionConf->ExtraPosLow = STANDARD_ZERO + EXTRA_POS_CLOSED;
    pComputedPositionConf->ExtraPosHigh = STANDARD_100 - EXTRA_POS_OPEN;
    pComputedPositionConf->n4OpenStopAdj = pPositionConf->PositionRange[Xhi];
    //end backward-compatible kludge
}

/** \brief Converts a A/D value of position to a “standard”
   value where 0%=STANDRAD_ZERO is de-energized stop and 100%=STANDARD_100 is energized stop.

This function makes no correction for ATO/ATC or closed/open stop adjustments

    The standard value can be outside this range,
    which indicates negative percent or greater than 100 percent.

    \param hardpos – The temperature corrected, linearized (for hall sensor) position A/D reading.
    \param pComputedPositionConf – a pointer to a structure containing the stops information
    \return the position in normalized standard range, before range adjustment
*/
static pos_t pos_MapPositionToStd(HardPos_t hardpos, const ComputedPositionConf_t *pComputedPositionConf, ErrorCode_t *err)
{
    //This can cause overflow: n4Output = (((s32)ui2Pos - (s32)pPositionStop->LowPositionSoftStop)*pPositionStop->slope)/STD_SCALE_UPSCALE;
    pos_least_t pos = intscale32(hardpos - pComputedPositionConf->offset_scaled,
                     pComputedPositionConf->slope_scaled,
                     STANDARD_ZERO,
                     POS_SCALE_SHIFT);
    //Output Range Validation
    pos_least_t result = CLAMP(pos, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION);
    *err = (result == pos)? ERR_OK : ERR_CAL_POSITION_OUT_OF_RANGE; //Or invent a better error name
    return (pos_t)pos; //safe cast: see limits
}


pos_t pos_AdjustStdPositionExt(adjpos_least_t pos, const ComputedPositionConf_t *posconf)
{
    pos = intscale32(pos - posconf->offset_adjusted,
                     posconf->slope_adjusted,
                     STANDARD_ZERO,
                     POS_SCALEADJ_SHIFT);
    //Output Range Validation
    pos = CLAMP(pos, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION);
    return (pos_t)pos; //safe cast: see limits
}

/** \brief Converts a position in standard range from normalized "hard stops" domain to the scaled (adjusted) domain
*/
static pos_t pos_AdjustStdPosition(pos_least_t pos)
{
    return pos_AdjustStdPositionExt(pos, &m_ComputedPositionConf);
}

pos_t pos_UnAdjustExt(adjpos_least_t pos, const ComputedPositionConf_t *posconf)
{
    pos = intscale32(pos - STANDARD_ZERO,
                     posconf->slope_unadjust,
                     posconf->offset_adjusted,
                     POS_SCALE_UNADJ_SHIFT);
    pos = CLAMP(pos, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION);
    return (pos_t)pos; //safe cast: see limits
}

/** \brief Converts a position in standard range from the scaled domain adjusted for air action
and stops adjustments to normalized "hard stops" domain
*/
pos_t pos_UnAdjust(pos_least_t pos)
{
    return pos_UnAdjustExt(pos, &m_ComputedPositionConf);
}

/** \brief Store the position stop calibration data and recompute the scaling data

  Store the position stop calibration data and recompute the scaling data
  \param[in] pPositionStop - local copy of the position calibration data
  \return ERR_OK if no error, ERR_CNFG_STOPS_RANGE if stops are too close
            together, ERR_INVALID_PARAMETER if invalid open stop adjustment
*/
ErrorCode_t TypeUnsafe_pos_SetPositionConf(const void *src)
{
    const PositionConf_t* pPositionConf = src;
    if(pPositionConf == NULL)
    {
        pPositionConf = &def_PositionConf;
    }
    ErrorCode_t err = pos_TestPositionConf(pPositionConf);
    if(err != ERR_OK)
    {
        return err;
    }

    ComputedPositionConf_t ComputedPositionConf;
    pos_CalculateComputedPositionConf(&ComputedPositionConf, pPositionConf);

    MN_ENTER_CRITICAL();
        //adjust setpoint to prevent bump when changing from ATO/ATC and/or stop adjustment
        cnfghook_CorrectForNewPositionConf(&m_ComputedPositionConf, &ComputedPositionConf);


        Struct_Copy(PositionStop_t, &m_PositionConf, pPositionConf);



        Struct_Copy(ComputedPositionConf_t, &m_ComputedPositionConf, &ComputedPositionConf);

        pos_InitFilter();
        pos_InitFilterRaw();
    MN_EXIT_CRITICAL();
    return ram2nvramAtomic(NVRAMID_PositionConf);
}

#if 0
/** \brief Synchronizes with changed stops (raw range values)
*/
void pos_AdjustForStopsRange(void)
{
    ComputedPositionConf_t ComputedPositionConf;
    pos_CalculateComputedPositionConf(&ComputedPositionConf, pos_GetPositionConf(NULL));
    Struct_Copy(ComputedPositionConf_t, &m_ComputedPositionConf, &ComputedPositionConf);
    //TBD if we want to reinit the filter
}
#endif

static PosState_t m_PosState; //! Everything interesting to the app about valve position

/** \brief A mandatory initializer of the input channel filter for position (or level)
*/
void pos_InitFilter(void)
{
    filter_Init(m_PosState.pos, &m_PosState.posfilter, &m_PositionConf.filtercoef);
}

/** \brief A mandatory initializer of the input channel filter for position (or level)
*/
void pos_InitFilterRaw(void)
{
    filter_Init(m_PosState.hardpos, &m_PosState.posfilterRaw, &m_PositionConf.filtercoef);
}

/** \brief The common finishing touch for all position-related channel functions
\param hardpos - tempcomp'ed and linearized
\param err - error code propagating from the channel function
*/
void pos_UpdateState(HardPos_t hardpos, ErrorCode_t err)
{
    PosState_t state;

    (void)pos_GetPosState(&state);

    if ( ERR_OK != err )
    {
        //Do not recompute anything; just save the error and the linearized read
        if(state.lifeleft > 0)
        {
            --state.lifeleft;
        }
    }
    else
    {
        state.pos = pos_MapPositionToStd(hardpos, pos_GetComputedPositionConf(NULL), &err); //test data at the same time

        //TODO:
        //If(err) fashion a fault like FAULT_POSCLAMP in position_dlt.c
        //TODO:
        //Magnet compensation is out; it should/must be built into the tempcomp table by MMA
        //Or at least move it to posint.c (from compiled out piece in tempcomp.c)

        state.hardpos = hardpos;
        state.pos_adjusted = pos_AdjustStdPosition(state.pos); //make it readily available for all who wants it
        //state.pos_adjusted = pos_UnAdjust(state.pos); //make it readily available for all who wants it

        /* add a positon (adjusted) sample to the smoothed data
        NOTE: This is compatible with the existing MNCB use patterns; we may want to revisit whether the adjusted
        or the scaled-only position is filtered, or both.
        */
        state.posfilter = m_PosState.posfilter;
        filter_Update(state.pos_adjusted, &state.posfilter, &pos_GetPositionConf(NULL)->filtercoef);
        state.posfilterRaw = m_PosState.posfilterRaw;
        filter_Update(state.hardpos, &state.posfilterRaw, &pos_GetPositionConf(NULL)->filtercoef);

        if (state.lifeleft < POS_LIFELEFT_MAX + 1)
        {
            state.lifeleft++;
        }
    }
    //Save the computed stuff atomically
    Struct_Copy(PosState_t, &m_PosState, &state);
}

const PosState_t *pos_GetPosState(PosState_t *dst)
{
    return STRUCT_GET(&m_PosState, dst);
}


/** \brief set the stop adjustments in terms of open/closed (or left/right) rather than energized/de-energized
  \param[in] OpenStopAdjustment - the open stop adjustment amount
  \return ERR_OK if no error, ERR_INVALID_PARAMETER if adjustment out of range
*/
ErrorCode_t pos_SetStopAdjustment(pos_least_t ClosedStopAdjustment, pos_least_t OpenStopAdjustment)
{
    PositionConf_t PositionConf;

    (void)pos_GetPositionConf(&PositionConf);
    //AK:TODO: Add limits check!
    //Decide what's closed and what's open
    pos_least_t r;

    r = STANDARD_ZERO + ClosedStopAdjustment;
    r = CLAMP(r, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION); //get in range to reject later
    PositionConf.PositionRange[Xlow] = (pos_t)r;

    r = STANDARD_ZERO + OpenStopAdjustment;
    r = CLAMP(r, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION); //get in range to reject later
    PositionConf.PositionRange[Xhi] = (pos_t)r;

    return pos_SetPositionConf(&PositionConf);
}

/** \brief retrieves the stop adjustments in terms of open/closed (or left/right) rather than energized/de-energized
  \param[out] dst - a pointer to hold the stop adjustments
  \return a flag of direction (ATO/ATC for MNCB)
*/
bool_t pos_GetStopAdjustment(pos_t dst[Xends])
{
    if(dst != NULL)
    {
        dst[Xlow] = m_PositionConf.PositionRange[Xlow] - STANDARD_ZERO;
        dst[Xhi] = m_PositionConf.PositionRange[Xhi] - STANDARD_ZERO;
    }
    return m_PositionConf.bATO;
}

//---------------- Runtime ----------------
/** \brief Returns scaled (to position stop info) position
\return scaled position
*/
pos_t vpos_GetScaledPosition(void)
{
    return m_PosState.pos_adjusted;
}

/** \brief Returns filtered scaled (to position stop info) position
\return filtered scaled position
*/
pos_least_t vpos_GetSmoothedScaledPosition(void)
{
    return filter_Report(&m_PosState.posfilter, &pos_GetPositionConf(NULL)->filtercoef);
}

/** \brief Returns filtered raw  position
\return filtered raw position
*/
pos_least_t vpos_GetSmoothedRawPosition(void)
{
    return filter_Report(&m_PosState.posfilterRaw, &pos_GetPositionConf(NULL)->filtercoef);
}

/** \brief Returns temperature-corrected raw position
\return positionadjusted to stops info
TODO: The name is terrible; needs replacement!
*/
HardPos_t vpos_GetRawPosition(void)
{
    return m_PosState.hardpos;
}
/* This line marks the end of the source */
