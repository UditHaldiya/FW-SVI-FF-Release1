/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file posext.h
    \brief System-level data and services related to DLT AMR level measurement

1. Sensor-specific raw data compensation
2. Sensor-specific linearization

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/DLT_Release_1.1.1/FIRMWARE/sysio/dltamr.c $
    $Date: 7/17/09 6:31p $
    $Revision: 32 $
    $Author: Anatoly Podpaly $

    \ingroup sysio
*/
#include "mnwrap.h"
#include "numutils.h"
#include "errcodes.h"
#include "crc.h"
#include "poscharact.h"
#include "mnassert.h"
#include "position.h"
#include "nvram.h"

static PosCharact_t CustomCharact;
static PosCharactSel_t PosCharactSel; //! characterization selector

/** \brief Deferred checksum tests
*/
void poscharact_Mopup(void)
{
    Struct_Test(PosCharactSel_t, &PosCharactSel);
    Struct_Test(PosCharact_t, &CustomCharact);
}


static const PosCharact_t * const PosCharactTab[]= //! Central access point to predefined characterizations
{
    [CHARACT_LINEAR] = &charactFlatTable_LINEAR,
    [CHARACT_EQUAL_30] = &charactFlatTable_EQUAL_30,
    [CHARACT_EQUAL_50] = &charactFlatTable_EQUAL_50,
    [CHARACT_QUICK_OPEN] = &charactFlatTable_QUICK_OPEN,
    [CHARACT_CUSTOM] = &CustomCharact,
    [CHARACT_CAMFLEX_EQ30] = &charactFlatTable_CAMFLEX_EQ30,
};

/** \brief Writes on of the predefined characterizations
\param Characterization - a selected characterization
\return an error code
*/
ErrorCode_t poscharact_Select(u8 Characterization)
{
    PosCharactSel_t s;
    (void)TypeUnsafe_poscharact_GetCharacterizationSel(&s);
    s.CharactSel = Characterization;
    return TypeUnsafe_poscharact_SetCharacterizationSel(&s);
}

/** \brief Required "set" accessor for PosCharactSel object
\param src - a pointer to desired value of PosCharactSel object
\return an error code
*/
ErrorCode_t TypeUnsafe_poscharact_SetCharacterizationSel(const void *src)
{
    const PosCharactSel_t *s = src;
    PosCharactSel_t deflt;
    if(s == NULL)
    {
        deflt.CharactSel = CHARACT_LINEAR;
        s = &deflt;
    }
    if(s->CharactSel >= NELEM(PosCharactTab))
    {
        return ERR_INVALID_PARAMETER;
    }
    Struct_Copy(PosCharactSel_t, &PosCharactSel, s);
    return ram2nvramAtomic(NVRAMID_PosCharactSel);
}

/** \brief Required "get" accessor for PosCharactSel object
\param src - a pointer to hold the value of PosCharactSel object
\return a pointer to PosCharactSel object
*/
const void *TypeUnsafe_poscharact_GetCharacterizationSel(void *dst)
{
    return STRUCT_TESTGET(&PosCharactSel, dst);
}

/** \brief An interpolation function
This is a dumb no-frills implementation; might be optimized if need be.
\param x - input value within the segment (or slightly outside an end segment)
\param left - a pointer to the left end of the interpolation segment
\param right - a pointer to the right end of the interpolation segment
\param dir - one of Xlow (direct interpolation) or Xhi (inverse interpolation)
\return the linear interpolation of x to the range segment
*/
static s32 poscharact_Interpolate(s32 z, const charactXY_t *left, const charactXY_t *right, s8_least dir)
{
    //Interpolation yields a value in the original range so the cast is valid
    s32 denom = right->x[dir] - left->x[dir];
    charactproduct_t num = (z - left->x[dir])*(charactproduct_t)(right->x[Xhi-dir] - left->x[Xhi-dir]);
    return (s32)(((2*num) + denom)/(2*denom)) + left->x[Xhi-dir]; //lint !e776 no addition overflow possible
}

#define CHUNK 5
/** \brief Finds interpolation segment in the array of points
\param z - input value within the segment (or slightly outside an end segment)
\param point - a pointer to an array of points
\param size - the number of points (1 greater than the number of segments)
\param dir - one of Xlow (direct interpolation) or Xhi (inverse interpolation)
\return 1-based index of the interpolation segment (0 or `size' mean out of table on the respective side)
*/
static s8_least charact_FindIndex(s32 z, const charactXY_t *point, s8_least size, s8_least dir)
{
#if 0
    u8_least left=1U, right = size;
    u8_least diff = right - left;
    do
    {
        index = left + diff/2U;
        if(point[index].x[dir] > z)
        {
            left = index;
        }
        else
        {
            right = index;
        }
        diff = right - left;
    } while(diff > 1U);

    if((point[left].x[dir] > z)
    {
        right = left;
    }
    return right;
#endif

    s8_least index;
    for(index=0; index < size; index+=CHUNK)
    {
        if(point[index].x[dir] > z)
        {
            break; //the previous (index-1) block contains the left point
        }
    }
    if(index >= CHUNK)
    {
        s8_least upper = MIN(index, size);
        for(index = index - CHUNK; index < upper; index++)
        {
            if(point[index].x[dir] > z)
            {
                break; //the previous (index-1) block contains the left point
            }
        }
    }
    return index;
}


/** \brief Position/Setpoint characterization routine
\param dir - Xlow means direct (x-->y); Xhi means inverse (y-->x)
\param z - value to characterize (x if dir==Xlow, y if dir = Xhi)
\pctab - a pointer to a tiered characterization table
\return characterized value
*/
static s32 poscharact_Characterize(s32 z, const PosCharact_t *pctab, s8 dir)
{
    s8_least index;
    const charactXY_t *left;
    const charactXY_t *right;

    index = charact_FindIndex(z, pctab->point, pctab->NumSeg+1, dir);

    /* A valid segment is [index-1, index]. There are 1 less segments than endpoints.
    */

    if((index == 0) || (index == pctab->NumSeg+1))
    {
        //we are below the lowest point or above the highest point; leave identical characterization
    }
    else
    {
        left = &pctab->point[index-1];
        right = &pctab->point[index];
        z = poscharact_Interpolate(z, left, right, dir);
    }
    return z;
}

/** \brief Direct characterization
\brief val - a value to characterize
\return characterized value
*/
s32 poscharact_Direct(s32 val)
{
    return poscharact_Characterize(val, PosCharactTab[PosCharactSel.CharactSel], Xlow);
}

/** \brief Direct characterization
\brief val - a value to characterize
\return characterized value
*/
s32 poscharact_Inverse(s32 val)
{
    return poscharact_Characterize(val, PosCharactTab[PosCharactSel.CharactSel], Xhi);
}



#define CHARACT_MAX_SLOPE 20 //! Max segment slope (direct or inverse) considered no big deal in numeric error
//NOTE: CHARACT_MAX_SLOPE must be such that no overflow occurs
CONST_ASSERT( (INT_PERCENT_OF_RANGE(100.0)*(s64)CHARACT_MAX_SLOPE) == (INT_PERCENT_OF_RANGE(100.0)*CHARACT_MAX_SLOPE) );

/* \brief Test whether a pair of points is monotonic and well conditioned
\param left - a pointer to the left endpoint
\param right - a pointer to the right endpoint
\return an error code
*/
static ErrorCode_t poscharact_TestPair(const charactXY_t *left, const charactXY_t *right)
{
    s32 Xdiff = right->x[Xlow] - left->x[Xlow];
    s32 Ydiff = right->x[Xhi] - left->x[Xhi];
    if((Xdiff <= 0) || (Ydiff <= 0))
    {
        return ERR_CHARACT_CUSTOM_ORDER;
    }
    //For numeric conditioning, we want 1/MaxSlope <= Xdiff/Ydiff <= MaxSlope
    if( (Xdiff > (CHARACT_MAX_SLOPE*Ydiff)) || (Ydiff > (CHARACT_MAX_SLOPE*Xdiff)) )
    {
        return ERR_CHARACT_ERROR;
    }
    return ERR_OK;
}

/** \brief Tests that an array of points is monotonic on X and Y
\param point - a pointer to an array of points
\param numseg - number of segments (1 less than number of points)
\return an error code
*/
static ErrorCode_t poscharact_TestTablePoints(const charactXY_t *point, u8_least numseg)
{
    u8_least i;
    ErrorCode_t err = ERR_OK;
    for(i = 0; i < numseg; i++)
    {
        //if((point[i].x[Xlow] >= point[i+1].x[Xlow]) || (point[i].x[Xhi] >= point[i+1].x[Xhi]))
        err = poscharact_TestPair(&point[i], &point[i+1]);
        if(err != ERR_OK)
        {
            break;
        }
    }
    return err;
}



/** \brief "get" accessor to custom characterization object
*/
const PosCharact_t *poscharact_GetCustomTable(PosCharact_t *dst)
{
    return STRUCT_TESTGET(&CustomCharact, dst);
}

/** \brief Reads coarse characterization table
\param charact - one of the predefined characterizations
\param dst - a pointer to the recipent object
\return a pointer to the coarse table; NULL if doesn't exist
*/
const PosCharact_t *poscharact_GetTable(u8 charact, PosCharact_t *dst)
{
    if(charact >= NELEM(PosCharactTab))
    {
        return NULL;
    }
    else if(charact == CHARACT_CUSTOM)
    {
        //separate case: need checksum test
        return poscharact_GetCustomTable(dst);
    }
    else if(PosCharactTab[charact] == NULL)
    {
        return NULL;
    }
    else
    {
        return STRUCT_GET(PosCharactTab[charact], dst);
    }
}

/** \brief Required "set" accessor to the custom characterization table
*/
ErrorCode_t poscharact_SetCustomTable(const PosCharact_t *src)
{
    if(src == NULL)
    {
        src = &charactFlatTable_LINEAR;
    }
    ErrorCode_t err = ERR_OK;

    if((src->NumSeg == 0) || (src->NumSeg > POSCHARACT_SEGS))
    {
        err = ERR_INVALID_PARAMETER;
    }
    if(err == ERR_OK)
    {
        err = poscharact_TestTablePoints(src->point, src->NumSeg);
    }
    if(err == ERR_OK)
    {
        //Force the endpoints
        if( (src->point[0].x[Xlow] != INT_PERCENT_OF_RANGE(0.0))
           || (src->point[0].x[Xhi] != INT_PERCENT_OF_RANGE(0.0))
               || (src->point[src->NumSeg].x[Xlow] != INT_PERCENT_OF_RANGE(100.0))
                   || (src->point[src->NumSeg].x[Xhi] != INT_PERCENT_OF_RANGE(100.0))
                       )
        {
            err = ERR_INVALID_PARAMETER;
        }
    }
    if(err == ERR_OK)
    {
        Struct_Copy(PosCharact_t, &CustomCharact, src);
        err = ram2nvramAtomic(NVRAMID_PosCharact);
    }
    return err;
}

/* This line marks the end of the source */
