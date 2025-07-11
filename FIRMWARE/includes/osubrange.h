/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file osubrange.h
    \brief API for output subranging (internally, both position and output)

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/includes/osubrange.h $
    $Date: 11/18/09 12:37p $
    $Revision: 6 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: osubrange.h $
 * 
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 11/18/09   Time: 12:37p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Added ao.h to resolve ao_t
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 12:55a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Repaired "lint fixes"
 *
 * *****************  Version 4  *****************
 * User: Sergey Kruss Date: 4/10/09    Time: 5:10p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Fixed Lint messages
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/03/09    Time: 7:24p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Lint
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/12/09    Time: 1:39a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Fixed range constants use and internal calculations
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/10/09    Time: 3:03p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * R/W output direction and ranges reimplemented and extracted
*/

#ifndef OSUBRANGE_H_
#define OSUBRANGE_H_

#include "dimensions.h"
#include "ao.h"

typedef struct ChannelAORangeValues_t
{
    pos_t posRV[Xends];
    ao_t aoRV[Xends];
    bool_t  AOdirFlag;
} ChannelAORangeValues_t; //! position retransmit scalers

typedef struct AORangeValues_t
{
    ChannelAORangeValues_t channelRV[AO_NUM_CHANNELS];
    u16 CheckWord;
} AORangeValues_t;


typedef struct AOComputedRangeValues_t
{
    s32 SpanPosition2AO;
    pos_t posLRV; //cashed for locality of reference
    //The two below are unnecessary (could be replaced with constants) if representation of all AO were the same
    ao_t PosMapLRV;  //cached value from aotab for locality of reference
    ao_t PosMapURV;  //cached value from aotab for locality of reference
    u8 shift; //exponent of power of 2 to scale down the pos->ao conversion
    u16 CheckWord;
} AOComputedRangeValues_t;

SAFEGET(ao_GetAORangeValues, AORangeValues_t);
SAFESET(ao_SetAORangeValues, AORangeValues_t);

//All accessor functions and data may or may not be used in a particular project.
//They are so declared; but basic accessors above must be present.

MN_DECLARE_API_FUNC(osubr_SetAODirection, osubr_SetAORangeValues, osubr_GetAORange, osubr_SetPosRangeValues, osubr_GetPosRange)
extern ErrorCode_t osubr_SetAODirection(u8_least mask, bool_t dir);
extern ErrorCode_t osubr_SetAORangeValues(u8_least mask, ao_t aoLRV, ao_t aoURV);
extern ErrorCode_t osubr_GetAORange(u8_least index, bool_t *dir, ao_t *lrv, ao_t *urv);
extern ErrorCode_t osubr_GetPosRange(u8_least index, bool_t *dir, pos_t *lrv, pos_t *urv);
extern ErrorCode_t osubr_SetPosRangeValues(u8_least mask, pos_t posLRV, pos_t posURV);

MN_DECLARE_API_DATA(AOPosRange, AORangeRange)
extern const pos_t AOPosRange[AO_NUM_CHANNELS][Xends+1];
extern const ao_t AORangeRange[AO_NUM_CHANNELS][Xends+1];
#endif //OSUBRANGE_H_
/* This line marks the end of the source */
