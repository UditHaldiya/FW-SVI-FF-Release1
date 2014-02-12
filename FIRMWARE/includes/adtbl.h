/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file adtbl.h
    \brief Header for A/D sensor sequence tables

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/adtbl.h $
    $Date: 12/07/11 6:31p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup ADTable
*/
/* $History: adtbl.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 6:31p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8255 - AP sequencing fix
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 11:54p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8255 - Header for A/D sensor sequence tables
*/
#ifndef ADTBL_H_
#define ADTBL_H_
#include "errcodes.h"

//lint ++flb Not all values are used by individual projects

#define AD_TABLE_STANDARD 0u
#define AD_TABLE_FACTORY 1u

typedef enum MSeq_t
{
    AD_SEQ_POS_INT,             // Hall sensor used for position
    AD_SEQ_POS_INT_ESD,         // has supply, act1 and act2 at fast rate for step response
    AD_SEQ_POS_REM,             // remote sensor used for position
    AD_SEQ_CALIB,               // has both Hall and remote position at fast rate
    AD_SEQ_POS_INT_INIT,        // Hall sensor used for position - initialization table
    AD_SEQ_POS_REM_INIT,        // remote sensor used for position - initialization table
    AD_SEQ_TEMPR,               // temperature sensor at fast rate for initial tempcomp & startup
    AD_SEQ_LIMIT                // do not use
} MSeq_t;
//lint --flb

#define AD_SEQ_DEFAULT AD_SEQ_LIMIT

extern void         adtbl_SetActiveSequence(MSeq_t seqSel);  // sets A/D table as selected by enum above
extern MSeq_t    adtbl_GetActiveSequence(void);              // gets enum for currently active table
MN_DECLARE_API_FUNC(adtbl_GetADCycleInterval) //may or may not be used
extern u16_least    adtbl_GetADCycleInterval(void);
extern void adtbl_StartAD(void);

extern const u8 * const adtbl_seqTbl[AD_SEQ_LIMIT];

typedef struct SensorType_t
{
    bool_t IsRemotePositionSensor;
    u16 CheckWord;
} SensorType_t;

extern const SensorType_t *ad_GetSensorType(SensorType_t *dst);
extern ErrorCode_t ad_SetSensorType(const SensorType_t *src);
UNTYPEACC(ad_SetSensorType, ad_GetSensorType); //NVMEM interface

#endif // ADTBL_H_

/* This line marks the end of the source */
