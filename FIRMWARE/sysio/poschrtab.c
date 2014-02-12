/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file poschrtab.c
    \brief Const tables for the characterization module.

    CPU: Any

    OWNER: AK
*/

#include "mnwrap.h"
#include "poscharact.h"

CONST_ASSERT(STANDARD_ZERO==0);
CONST_ASSERT(STANDARD_100==16384);

const PosCharact_t charactFlatTable_LINEAR =
{
    .NumSeg = 1,
    .point = MN_ALLOW_INIT_ZERODEFAULT()
    {
        [0] = {.x = {0, 0,}},
        [1] = {.x = {16384, 16384,}},
    },
    .CheckWord = 0xFFFFU, //don't care
};

const PosCharact_t charactFlatTable_EQUAL_30 =
{
    .NumSeg = 16,
    .point = MN_ALLOW_INIT_ZERODEFAULT()
    {
        [0] = {.x = {0, 0,}},
        //Fine 1
        [1] = {.x = {2300, 346, }},
        [2] = {.x = {4200, 786, }},
        [3] = {.x = {5800, 1318, }},
        [4] = {.x = {7150, 1928, }},
        [5] = {.x = {8350, 2633,}},
        //fine 2
        [6] = {.x = {9400, 3411, }},
        [7] = {.x = {10360, 4288, }},
        [8] = {.x = {11250, 5273, }},
        [9] = {.x = {12050, 6328, }},
        [10] = {.x = {12800, 7489,}},
        //fine 3
        [11] = {.x = {13470, 8691, }},
        [12] = {.x = {14120, 10028,} },
        [13] = {.x = {14720, 11433, }},
        [14] = {.x = {15270, 12885, }},
        [15] = {.x = {15800, 14449,}},
        //No fine 4
        [16] = {.x = {16384, 16384,}},
    },
    .CheckWord = 0xFFFFU, //don't care
};

const PosCharact_t charactFlatTable_EQUAL_50 =
{
    .NumSeg = 16,
    .point = MN_ALLOW_INIT_ZERODEFAULT()
    {
        [0] = {.x = {0, 0,},},
        [1] = {.x = {2700, 303, }},
        [2] = {.x = {4750, 705, }},
        [3] = {.x = {6400, 1207, }},
        [4] = {.x = {7770, 1803, }},
        [5] = {.x = {8950, 2499,},},
        [6] = {.x = {10000, 3306, }},
        [7] = {.x = {10950, 4233, }},
        [8] = {.x = {11770, 5221, }},
        [9] = {.x = {12530, 6327, }},
        [10] = {.x = {13220, 7520,},},
        [11] = {.x = {13860, 8817, }},
        [12] = {.x = {14450, 10201,} },
        [13] = {.x = {15000, 11679, }},
        [14] = {.x = {15520, 13268, }},
        [15] = {.x = {16000, 14919,},},
        [16] = {.x = {16384, 16384,},},
    },
    .CheckWord = 0xFFFFU, //don't care
};


const PosCharact_t charactFlatTable_QUICK_OPEN =
{
    .NumSeg = 16,
    .point = MN_ALLOW_INIT_ZERODEFAULT()
    {
        [0] = {.x = {0, 0,},},
        [1] = {.x = {384, 1465, }},
        [2] = {.x = {864, 3116, }},
        [3] = {.x = {1384, 4705, }},
        [4] = {.x = {1934, 6183, }},
        [5] = {.x = {2524, 7567,},},
        [6] = {.x = {3164, 8864, }},
        [7] = {.x = {3854, 10057, }},
        [8] = {.x = {4614, 11163, }},
        [9] = {.x = {5434, 12151, }},
        [10] = {.x = {6384, 13078,},},
        [11] = {.x = {7434, 13885, }},
        [12] = {.x = {8614, 14581,} },
        [13] = {.x = {9984, 15177, }},
        [14] = {.x = {11634, 15679, }},
        [15] = {.x = {13684, 16081,},},
        [16] = {.x = {16384, 16384,},},
    },
    .CheckWord = 0xFFFFU, //don't care
};

const PosCharact_t charactFlatTable_CAMFLEX_EQ30 =
{
    .NumSeg = 16,
    .point = MN_ALLOW_INIT_ZERODEFAULT()
    {
        [0] = {.x = {0, 0,},},
        [1] = {.x = {2458, 983,}},
        [2] = {.x = {4915, 1966,}},
        [3] = {.x = {7117, 2949,}},
        [4] = {.x = {8724, 3932,}},
        [5] = {.x = {9981, 4915,},},
        [6] = {.x = {11012, 5898,}},
        [7] = {.x = {11869, 6881,}},
        [8] = {.x = {12593, 7864,}},
        [9] = {.x = {13206, 8847,}},
        [10] = {.x = {13718, 9830,},},
        [11] = {.x = {14156, 10813,}},
        [12] = {.x = {14944, 12780,}},
        [13] = {.x = {15335, 13763,}},
        [14] = {.x = {15729, 14746,}},
        [15] = {.x = {16122, 15729,},},
        [16] = {.x = {16384, 16384,},},
    },
    .CheckWord = 0xFFFFU, //don't care
};


/* This line marks the end of the source */
