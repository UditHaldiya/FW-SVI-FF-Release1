/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file charact.h
    \brief public interfaces of characterization module

     CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
#ifndef CHARACT_H_
#define CHARACT_H_

#include "errcodes.h"
#include "percent.h"

#define CHARACT_TABLE_NUMPOINTS 17u //number of interpolation points: (0,0) is not assumed

#define MAX_HALL_INPUT_ALLOWED ((u16)(charactTable_Hall[CHARACT_TABLE_NUMPOINTS-1u].x))
#define MAX_HALL_OUTPUT_ALLOWED (charactTable_Hall[CHARACT_TABLE_NUMPOINTS-1u].y)


typedef s32 fixed16_t; //with point after 16 digits

//A type containing just an <X,Y> pair of std_t elements and
// the slope to the next point

typedef struct stdXY_t
{
    percent_t x;
    percent_t y;
    fixed16_t slope; //from current to next
} stdXY_t;

extern s32 charact_HallCharacterization(s32 val);

#endif //CHARACT_H_
/* This line marks the end of the source */
