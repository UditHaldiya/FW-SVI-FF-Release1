/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file charact.c
    \brief characterization module

     CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
#include <stddef.h>
#include "mnwrap.h"
#include "oswrap.h"
#include "numutils.h"
#include "mnassert.h"
#include "charact.h"
#include "charact_tables.h"


/*
The characterization lookup tables.  Each table must contain TABLE_NUMPOINTS
XY pairs (as a single dimension array of 32 s16 values where X values
are the even entries and the Y values are the odd entries).
X represents the input and Y represents the output.  All of these tables
are in flash (constants section) except the custom characterization
table which is in RAM (and FRAM).
*/


/**
  Here we search for the interpolation segment point counting forward or
  backward from the previous point, whatever it was.
  Since we linearize a physical signal with a short period, we should
  stay in the same segment or move at most one segment at a time.
  This should beat binary search hands down, except the very first
  time on init (currpoint is 0), or when the characterization option
  changes. A dummy call to linearization will fix it.
  \param val - value to linearize
  \param currpoint - previous
  \return new segment point
*/
static u8_least find_point(s32 val, u8_least currpoint, const stdXY_t *table, u32 numpoints)
{
    //get current point
    if(currpoint >= numpoints)
    {
        currpoint = numpoints/2u; //paranoia; should never happen
    }
    if(table[currpoint].x <= val)
    {
        //going forward
        while(currpoint < numpoints)
        {
            if(table[currpoint+1u].x > val)
            {
                break; //return currpoint; // 'cause MISRA can't break
            }
            else
            {
                ++currpoint;
            }
        }
    }
    else
    {
        //going backward
        while(currpoint != 0u)
        {
            if(table[currpoint].x > val)
            {
                --currpoint;
            }
            else
            {
                 break; //return currpoint; // 'cause MISRA can't break
            }
        }
    }
    return currpoint;
}

static s32 interpolate(s32 val, const stdXY_t *segment)
{
    //return ((((val - segment->x) * segment->slope)/(1<<16))) + segment->y;
    //better precision, same worst case, slightly slower
    return (((((val - segment->x) * segment->slope) +(s32)(1U<<15))/(s32)(1U<<16))) + segment->y;
}

static s32 compute(s32 val, u8 *pCurrpoint, const stdXY_t *segTable, u32 numpoints)
{
    u8_least currpoint = find_point(val, (u8_least)*pCurrpoint, segTable, numpoints);
    *pCurrpoint = (u8)currpoint; //save for the next computation
    return interpolate(val, segTable + currpoint);
}

//The initial values are don't care, but to keep the compiler happy...
static u8 prevHallpoint = 0;

/**
\brief interpolation using a 16 XY pair look-up table.
The input must be a standard value and the output is a standard value.
If the input is outside of 0 and 16384, no interpolation is performed
and the output is set equal to the input.

Notes:
This routine is called every control cycle and must be as fast as possible.
No floating point math should be used.
To speed up the routine, a binary search of for the pair of X values containing
the input value should be used.
A value of 0xFF for ui1Characterization means to use the Hall sensor
linearization table.

\param n4Input - the standard range input value
\param ui1Characterization - the index of the characterization curve to use
    which is a member of {LINEAR=0, EQUAL30, EQUAL50, QUICK50, CAMFLEXEQ, and CUSTOM}
    or HALL_LINEARIZATION
\return n4Output - the characterized value interpolated from the table
*/

static s32 computeHall(s32 val)
{
    MN_ASSERT(val>=0); //by design
    if(val > MAX_HALL_INPUT_ALLOWED)
    {
        //nothing to do
        //AK: That's according to the design but different from SVI2
        //where, per Larry, there is extrapolation with the closest slope

        //LS 3/10/05 - clamp at max output
        val = MAX_HALL_OUTPUT_ALLOWED;
    }
    else
    {
        val = compute(val, &prevHallpoint, charactTable_Hall, CHARACT_TABLE_NUMPOINTS);
    }
    return val;
}

s32 charact_HallCharacterization(s32 val)
{
    s32 result = computeHall(ABS(val));
    if(val < 0)
    {
        result = -result; //Odd function
    }
    return result;
}


/* This line marks the end of the source */
