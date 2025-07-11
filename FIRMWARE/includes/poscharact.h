/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file poscharact.h
    \brief API for setpoint and position characterization

    CPU: Any

    OWNER: AK

    \ingroup sysio
*/

#ifndef POSCHARACT_H_
#define POSCHARACT_H_
#include "dimensions.h" //for charact table dimensions
#include "errcodes.h" //for linearization table dimensions
#include "percent.h"

#if 0 //abandoned segmented design
#define POSCHARACT_BLOCKSZ 4 //! Number of characterization points in a fine table's block
#define POSCHARACT_COARSE_PTS (POSCHARACT_COARSE_SEGS+1)

/* Total number of segments =
(1+POSCHARACT_BLOCKSZ)*POSCHARACT_COARSE_SEGS
*/
#endif //0

#define CHARACT_LINEAR 0
#define CHARACT_EQUAL_30 1
#define CHARACT_EQUAL_50 2
#define CHARACT_QUICK_OPEN 3
#define CHARACT_CUSTOM 4
#define CHARACT_CAMFLEX_EQ30 5

//move to project-specific header FBO DLT
#if 0
typedef s32 charactxy_t; //! Characterization table x or y value type
typedef s64 charactproduct_t;
#else
typedef percent_t charactxy_t; //! Characterization table x or y value type
typedef s32 charactproduct_t;
#endif

#define POSCHARACT_SEGS 20

typedef struct charactXY_t //! a type of linerarization point
{
    charactxy_t x[Xends];
} charactXY_t;


typedef struct PosCharact_t
{
    u8 NumSeg; //1 + number of internal points;
    charactXY_t point[POSCHARACT_SEGS+1];
    u16 CheckWord;
} PosCharact_t;

extern const PosCharact_t *poscharact_GetCustomTable(PosCharact_t *dst);
extern ErrorCode_t poscharact_SetCustomTable(const PosCharact_t *src);
UNTYPEACC(poscharact_SetCustomTable, poscharact_GetCustomTable);

extern const PosCharact_t *poscharact_GetTable(u8 charact, PosCharact_t *dst);

typedef struct PosCharactSel_t
{
    u8 CharactSel;
    u16 CheckWord;
} PosCharactSel_t;

//Pre-built characterizations
extern const PosCharact_t charactFlatTable_LINEAR;
extern const PosCharact_t charactFlatTable_EQUAL_30;
extern const PosCharact_t charactFlatTable_EQUAL_50;
extern const PosCharact_t charactFlatTable_QUICK_OPEN;
extern const PosCharact_t charactFlatTable_CAMFLEX_EQ30;

//NVMEM interface
extern ErrorCode_t TypeUnsafe_poscharact_SetCharacterizationSel(const void *src);
//extern const void *TypeUnsafe_poscharact_GetCharacterizationSel(void *dst);
SAFEGET(poscharact_GetCharacterizationSel, PosCharactSel_t);


//Runtime interface
extern s32 poscharact_Direct(s32 val);
extern s32 poscharact_Inverse(s32 val);
extern ErrorCode_t poscharact_Select(u8 Characterization);

#define CHARACT_MASK 0xFFu //full byte mask asked for by UI table; FUTURE: deprecated

//Deferred functionality
extern void poscharact_Mopup(void);

#endif //POSCHARACT_H_

/* This line marks the end of the source */
