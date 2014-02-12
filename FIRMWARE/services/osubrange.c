/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file osubrange.c
    \brief Implementation of output subranging (internally, both position and output)

NOTE: The `read` API is per output channels
      The `write` API` is for a requested set of channels

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/osubrange.c $
    $Date: 1/12/12 12:43p $
    $Revision: 10 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: osubrange.c $
 * 
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 12:43p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * TFS:8674 - added AO subrange
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 1:02a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Repaired "lint fixes"
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 4/13/09    Time: 4:29p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Changed limit of AO1 URV from 23.0 to 20.5 mA (bug 674)
 *
 * *****************  Version 6  *****************
 * User: Sergey Kruss Date: 4/10/09    Time: 5:11p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Fixed Lint messages
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 4/08/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Repaired AP and ESD builds
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/23/09    Time: 3:29p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Corrected output range checking and added OUTPUT_SATURATED fault
 * definition
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/13/09    Time: 4:22p
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Fixed bug 555
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/12/09    Time: 1:39a
 * Updated in $/MNCB/Dev/FIRMWARE/services
 * Fixed range constants use and internal calculations
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/10/09    Time: 3:02p
 * Created in $/MNCB/Dev/FIRMWARE/services
 * R/W output direction and ranges reimplemented and extracted
*/
#include "mnwrap.h"
#include "dimensions.h"
#include "ao.h"
#include "configure.h"
#include "osubrange.h"
#include "mnassert.h"
#include "oswrap.h"

/** defines for position retransmit */
#define RV_LOW_VALUE (-199.0F) //percent of position range
#define RV_HIGH_VALUE (199.0F)
#define RV_MIN_SPAN_AO (1.0F) //as in "classic" MNCB, not explicitly defined in DLT


//#define RV_LOW_VALUE_INT ((s32)((((cdouble_t)RV_LOW_VALUE*(cdouble_t)STANDARD_RANGE)/100.0)+0.5)) //round up
#define RV_LOW_VALUE_INT ((s32)((((cdouble_t)RV_LOW_VALUE*(cdouble_t)STANDARD_RANGE)/100.0))) //truncate - negative
CONST_ASSERT(RV_LOW_VALUE_INT>MIN_ALLOWED_POSITION);
#define RV_HIGH_VALUE_INT ((s32)((((cdouble_t)RV_HIGH_VALUE*(cdouble_t)STANDARD_RANGE)/100.0))) //truncate
CONST_ASSERT(RV_HIGH_VALUE_INT<MAX_ALLOWED_POSITION);
#define RV_MIN_SPAN ((s32)((((cdouble_t)RV_MIN_SPAN_AO*(cdouble_t)STANDARD_RANGE)/100.0))) //truncate

//limits of position range boundaries: LRV, URV value limits, and min difference
const pos_t AOPosRange[AO_NUM_CHANNELS][Xends+1] =
{
    [0] = //secondary (retransmit) output
    {
        [Xlow] = RV_LOW_VALUE_INT,
        [Xhi] = RV_HIGH_VALUE_INT,
        [Xends] = RV_MIN_SPAN,
    },
#if AO_NUM_CHANNELS > 1
    [1] = //primary (loop current) output
    {
        [Xlow] = RV_LOW_VALUE_INT,
        [Xhi] = RV_HIGH_VALUE_INT,
        [Xends] = RV_MIN_SPAN,
    },
#endif //AO_NUM_CHANNELS
};

//limits of output current range boundaries: LRV, URV value limits, and min difference
const ao_t AORangeRange[AO_NUM_CHANNELS][Xends+1] =
{
    [0] = //secondary (retransmit) output
    {
        [Xlow] = STD_FROM_MA(3.8),
        [Xhi] = STD_FROM_MA(23.0),
        [Xends] = STD_FROM_MA(5.0),
    },
#if AO_NUM_CHANNELS > 1
    [1] = //primary (loop current) output
    {
        [Xlow] = STD_FROM_MA(3.8),
        [Xhi] = STD_FROM_MA(20.5), //NAMUR NE-43 limit
        [Xends] = STD_FROM_MA(5.0),
    },
#endif //AO_NUM_CHANNELS
};

#define DEFAULT_fLRV (0.0F)
#define DEFAULT_fURV (100.0F)

//Common for all AO channels
static const ChannelAORangeValues_t def_AORangeValues =
{
    .posRV =
    {
        [Xlow] =INT_PERCENT_OF_RANGE(DEFAULT_fLRV),
        [Xhi] = INT_PERCENT_OF_RANGE(DEFAULT_fURV),
    },
    .aoRV =
    {
        [Xlow] = AO_MAP_LRV,
        [Xhi] = AO_MAP_URV,
    },
    .AOdirFlag = true, //direct
};

static AOComputedRangeValues_t m_AOComputedRangeValues[AO_NUM_CHANNELS]; //! Computed range values


static AORangeValues_t m_AORangeValues; //! Combined range values for all channels

//accessors

/** \brief Public API to retrieve output direction and AO output range values
\param index - output channel index
\param dir - a pointer for output direction
\param lrv - a pointer to output LRV
\param urv - a pointer to output URV
*/
ErrorCode_t osubr_GetAORange(u8_least index, bool_t *dir, ao_t *lrv, ao_t *urv)
{
    if(index >= AO_NUM_CHANNELS)
    {
        return ERR_INVALID_PARAMETER;
    }
    if(dir != NULL)
    {
        *dir = m_AORangeValues.channelRV[index].AOdirFlag;
    }
    if(lrv != NULL)
    {
        *lrv = m_AORangeValues.channelRV[index].aoRV[Xlow];
    }
    if(urv != NULL)
    {
        *urv = m_AORangeValues.channelRV[index].aoRV[Xhi];
    }
    Struct_Test(AORangeValues_t, &m_AORangeValues);
    return ERR_OK;
}

/** \brief Public API to retrieve output direction and position/level output range values
\param index - output channel index
\param dir - a pointer for output direction
\param lrv - a pointer to output LRV
\param urv - a pointer to output URV
*/
ErrorCode_t osubr_GetPosRange(u8_least index, bool_t *dir, pos_t *lrv, pos_t *urv)
{
    if(index >= AO_NUM_CHANNELS)
    {
        return ERR_INVALID_PARAMETER;
    }
    if(dir != NULL)
    {
        *dir = m_AORangeValues.channelRV[index].AOdirFlag;
    }
    if(lrv != NULL)
    {
        *lrv = m_AORangeValues.channelRV[index].posRV[Xlow];
    }
    if(urv != NULL)
    {
        *urv = m_AORangeValues.channelRV[index].posRV[Xhi];
    }
    Struct_Test(AORangeValues_t, &m_AORangeValues);
    return ERR_OK;
}

/** \brief get combined output range values. Not useful other than for NVMEM interface.
    \return pointer to m_AORangeValues
*/
const void *TypeUnsafe_ao_GetAORangeValues(void *dst)
{
    return STRUCT_TESTGET(&m_AORangeValues, dst);
}

/** \brief A helper to produce pre-computed variables from a channel's configuration
\param aorval - a pointer to configured params
\param caorval[out] - a pointer to computed params
return an error code
*/
static ErrorCode_t osubr_Compute(const ChannelAORangeValues_t *aorval, AOComputedRangeValues_t *caorval)
{
    ao_t lrv, urv;
    //calculate span and offset for scaling position to LRV URV

    lrv = aorval->aoRV[Xlow];
    urv = aorval->aoRV[Xhi];
    if ( !aorval->AOdirFlag )
    {
        //reverse
        ao_t temp;
        temp = lrv;
        lrv = urv;
        urv = temp;
    }
    caorval->PosMapLRV = lrv;
    caorval->PosMapURV = urv;

    s32 numerator = urv - lrv;
    s32 denominator = aorval->posRV[Xhi] - aorval->posRV[Xlow];
    u8_least shift = (29U - intlog((u32)ABS(numerator))) + intlog((u32)ABS(denominator));
    shift = MIN(shift, 29U);
    s64 scale = ((s64)(1ULL << shift)*numerator ) / denominator;
    MN_DBG_ASSERT(scale == (s32)scale);
    caorval->SpanPosition2AO = (s32)scale; //cast is OK due to the design of `shift`
    caorval->posLRV = aorval->posRV[Xlow];
    caorval->shift = (u8)shift;
    return ERR_OK;
}

/** \brief A helper to set all requested channels atomically
\param mask - a bitwise OR of 1<<requested channels
\param aorval - a pointer to combined output range values configuration
\param caorval - an array of the corresponding precomputed data
\return an error code
*/
static ErrorCode_t osubr_Commit(u8_least mask, const AORangeValues_t *aorval, const AOComputedRangeValues_t caorval[AO_NUM_CHANNELS])
{
    MN_ENTER_CRITICAL(); //TODO: Can we afford it?
        u8_least rolling_one=1U;
        for(u8_least index = 0; index<NELEM(m_AOComputedRangeValues); index++)
        {
            if((mask & rolling_one) != 0U)
            {
                Struct_Copy(AOComputedRangeValues_t, &m_AOComputedRangeValues[index], &caorval[index]);
            }
            rolling_one<<=1;
        }
        Struct_Copy(AORangeValues_t, &m_AORangeValues, aorval);
    MN_EXIT_CRITICAL();
    return ram2nvramAtomic(NVRAMID_AORangeValues);
}

/** \brief Public API to set direction in all requested channels
\param mask - a bitwise OR of 1<<requested channels
\param dir - common requested output direction
\return an error code
*/
ErrorCode_t osubr_SetAODirection(u8_least mask, bool_t dir)
{
    AORangeValues_t aorval;
    AOComputedRangeValues_t caorval[AO_NUM_CHANNELS]; //hmm...
    (void)ao_GetAORangeValues(&aorval);
    u8 rolling_one=1U;
    for(u8_least index = 0; index<NELEM(m_AOComputedRangeValues); index++)
    {
        if((mask & rolling_one) != 0U)
        {
            aorval.channelRV[index].AOdirFlag = dir;
            ErrorCode_t err = osubr_Compute(&aorval.channelRV[index], &caorval[index]);
            if(err != ERR_OK)
            {
                return err; //no side effects created yet
            }
        }
        rolling_one<<=1;
    }
    return osubr_Commit(mask, &aorval, caorval); //lint !e645 caorval not init'ed - pieces that are used are init'ed
}

/** \brief Public API to set position output range values in all requested channels
\param mask - a bitwise OR of 1<<requested channels
\param posLRV - position LRV
\param posURV - position URV
\return an error code
*/
ErrorCode_t osubr_SetPosRangeValues(u8_least mask, pos_t posLRV, pos_t posURV)
{
    AORangeValues_t aorval;
    AOComputedRangeValues_t caorval[AO_NUM_CHANNELS]; //hmm...
    (void)ao_GetAORangeValues(&aorval);
    u8 rolling_one=1U;
    for(u8_least index = 0; index<NELEM(m_AOComputedRangeValues); index++)
    {
        if((mask & rolling_one) != 0U)
        {
            //This index is requested to change
            if(posLRV < AOPosRange[index][Xlow])
            {
                /* Legitimate early return: Pre-requisites check */
                return ERR_HART_RV_LOW_TOO_LOW;
            }
            else if    (posURV < AOPosRange[index][Xlow])
            {
                 /* Legitimate early return: Pre-requisites check */
                return ERR_HART_RV_HIGH_TOO_LOW;
            }
            else if    (posLRV > AOPosRange[index][Xhi])
            {
                /* Legitimate early return: Pre-requisites check */
                return ERR_HART_RV_LOW_TOO_HIGH;
            }
            else if    (posURV > AOPosRange[index][Xhi])
            {
                /* Legitimate early return: Pre-requisites check */
                return ERR_HART_RV_HIGH_TOO_HIGH;
            }
            else if( (posURV-posLRV) < AOPosRange[index][Xends])
            {
                /* Legitimate early return: Pre-requisites check */
                return ERR_HART_RV_SPAN_LOW;
            }
            else
            {
                //ok
                aorval.channelRV[index].posRV[Xlow] = posLRV;
                aorval.channelRV[index].posRV[Xhi] = posURV;
                ErrorCode_t err = osubr_Compute(&aorval.channelRV[index], &caorval[index]);
                if(err != ERR_OK)
                {
                    return err; //no side effects created yet
                }
            }
        }
        rolling_one<<=1;
    }
    return osubr_Commit(mask, &aorval, caorval); //lint !e645 caorval not init'ed - pieces that are used are init'ed
}

/** \brief A helper to test AO range values for all requested channels
\param mask - a bitwise OR of 1<<requested channels
\param aoLRV - output current LRV
\param aoURV - output current URV
\return an error code
*/
static ErrorCode_t osubr_TestAORangeValues(u8_least index, ao_t aoLRV, ao_t aoURV)
{
    if(aoLRV < AORangeRange[index][Xlow])
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_HART_RV_LOW_TOO_LOW;
    }
    else if    (aoURV < AORangeRange[index][Xlow])
    {
         /* Legitimate early return: Pre-requisites check */
        return ERR_HART_RV_HIGH_TOO_LOW;
    }
    else if    (aoLRV > AORangeRange[index][Xhi])
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_HART_RV_LOW_TOO_HIGH;
    }
    else if    (aoURV > AORangeRange[index][Xhi])
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_HART_RV_HIGH_TOO_HIGH;
    }
    else if ((aoURV-aoLRV) < AORangeRange[index][Xends])
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_HART_RV_SPAN_LOW;
    }
    else
    {
        return ERR_OK;
    }
}

/** \brief Public API to set AO range values in all requested channels
\param mask - a bitwise OR of 1<<requested channels
\param aoLRV - output current LRV
\param aoURV - output current URV
\return an error code
*/
ErrorCode_t osubr_SetAORangeValues(u8_least mask, ao_t aoLRV, ao_t aoURV)
{
    AORangeValues_t aorval;
    AOComputedRangeValues_t caorval[AO_NUM_CHANNELS]; //hmm...
    (void)ao_GetAORangeValues(&aorval);
    u8_least rolling_one=1U;
    for(u8_least index = 0; index<NELEM(m_AOComputedRangeValues); index++)
    {
        if((mask & rolling_one) != 0U)
        {
            //This index is requested to change
            ErrorCode_t err = osubr_TestAORangeValues(index, aoLRV, aoURV);
            if(err != ERR_OK)
            {
                return err;
            }
            else
            {
                //ok
                aorval.channelRV[index].aoRV[Xlow] = aoLRV;
                aorval.channelRV[index].aoRV[Xhi] = aoURV;
                err = osubr_Compute(&aorval.channelRV[index], &caorval[index]);
                if(err != ERR_OK)
                {
                    return err; //no side effects created yet
                }
            }
        }
        rolling_one<<=1;
    }
    return osubr_Commit(mask, &aorval, caorval); //lint !e645 caorval not init'ed - pieces that are used are init'ed
}




/** \brief set position retransmit LRV and URV for all outputs in AO and position domains
    \param src - pointer to a copy of new values
    \return ERR_OK iff no error
*/
ErrorCode_t TypeUnsafe_ao_SetAORangeValues(const void *src)
{
    MN_ASSERT(!oswrap_IsOSRunning()); //i.e. this is only good for initialization
    const AORangeValues_t* pAORangeValues;

    u8_least index;
    u8_least rolling_one;

    //Copy the stuff unconditionally (to save stack memory) - that's why init only
    if(src == NULL)
    {
        for(index = 0; index < AO_NUM_CHANNELS; index++)
        {
            m_AORangeValues.channelRV[index] = def_AORangeValues;
        }
        STRUCT_CLOSE(AORangeValues, &m_AORangeValues);
    }
    else
    {
        pAORangeValues = src;
        Struct_Copy(AORangeValues, &m_AORangeValues, pAORangeValues);
    }

    //Now, set values in place while checking the input
    /* Here's what we check:
    - Direction doesn't require anything special
    - Test AO RV insted of setting them to avoid duplicate recalculation
    - Set Pos RV using the official API
    */
    ErrorCode_t err;
    rolling_one=1U;
    for(index = 0; index < AO_NUM_CHANNELS; index++)
    {
        err = osubr_TestAORangeValues(index, m_AORangeValues.channelRV[index].aoRV[Xlow], m_AORangeValues.channelRV[index].aoRV[Xhi]);
        if(err == ERR_OK)
        {
            err = osubr_SetPosRangeValues(rolling_one, m_AORangeValues.channelRV[index].posRV[Xlow], m_AORangeValues.channelRV[index].posRV[Xhi]);
        }
        if(err != ERR_OK)
        {
            break;
        }
        rolling_one<<=1;
    }

    return err;
}


/** \brief get pointer to compute position retransmit range values used for scaling
    \return pointer to m_AOComputedRangeValues
*/
static const AOComputedRangeValues_t *ao_GetComputedRangeValues(u8_least index, AOComputedRangeValues_t *dst)
{
    return STRUCT_TESTGET(&m_AOComputedRangeValues[index], dst);
}

/** \brief converts a standard position to the D/A scale for position retransmit
    \param PositionStd - the position to convert
    \param pAOComputedRangeValues - LRV, URV scale factors
    \return current in internal representation (before user cal and tempcomp)
*/
static ao_least_t ao_PositionToAO(pos_least_t PositionStd, const AOComputedRangeValues_t*
    pAOComputedRangeValues)
{
    ao_least_t PositionAOPreUser;

    //convert position to AO range (as specified) using position LRV, URV
    PositionAOPreUser = intscale32(pAOComputedRangeValues->SpanPosition2AO,
        PositionStd - pAOComputedRangeValues->posLRV,
        pAOComputedRangeValues->PosMapLRV,
        pAOComputedRangeValues->shift);

    return PositionAOPreUser;
}

/** \brief Sets the D/A to a value representing the position

  This routine converts a standard position value to the proper scale to output to the D/A.
  It uses the user calibrations.
    \param index - index of the AO channel
    \param[in] PositionStd - the position to set
*/
void ao_SetPositionRetransmit(u8_least index, pos_least_t PositionStd)
{
    ao_least_t AOValue;
    const AOComputedRangeValues_t* pAOComputedRangeValues;

    if(index >= AO_NUM_CHANNELS)
    {
        return; //ignore the request
    }

    /* compute value for position retransmit */
    pAOComputedRangeValues = ao_GetComputedRangeValues(index, NULL);
    AOValue = ao_PositionToAO(PositionStd, pAOComputedRangeValues);

    ao_least_t lolim, hilim;

    //range check
    lolim = aotab[index].aoNormal_range[Xlow];
    hilim = aotab[index].aoNormal_range[Xhi];

    ao_least_t AOValue_out = CLAMP(AOValue, lolim, hilim);

#if AO_NUM_CHANNELS > 1
    if((index == AO_CHANNEL_LOOP_CURRENT) && (AOValue_out != AOValue))
    {
        error_SetFault(FAULT_OUTPUT_SATURATED);
    }
    else
    {
        error_ClearFault(FAULT_OUTPUT_SATURATED);
    }
#endif
    ao_SetAOValue(index, AOValue_out, true);
}

/* This line marks the end of the source */
