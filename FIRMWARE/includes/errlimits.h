/*
Copyright 2005-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file errlimits.h
    \brief Header for error limits configuration

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/includes/errlimits.h $
    $Date: 4/14/09 11:48p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup Configure
*/
/*  $History: errlimits.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/14/09    Time: 11:48p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * MNCB-style stats header
*/
#ifndef ERRLIMITS_H_
#define ERRLIMITS_H_

#include "position.h"
#include "timebase.h"

typedef struct ErrorLimits_t
{
    pos_t NearPosition;            //!<  Cutoff for "near closed" in % open
    pos_t PositionErrorBand;       //!<  position error band
    ctltick_t PositionTime1;           //!<  position error timeout 1 - warning
    u8 bPosErr1Enable;
                     //!< bPosErr2Enable no longer used
    u16 CheckWord;
} ErrorLimits_t;

//----------------- position errors -------------------------
#define POSBAND_LOW  0.5F
#define POSBAND_HIGH  199.0F
#define NEARPOS_LOW  0.0F
#define NEARPOS_HIGH  20.0F
#define POSTIME1_LOW  0.0F
#define POSTIME1_HIGH  327.0F


#define POSBAND_LOW_INT INT_PERCENT_OF_RANGE(POSBAND_LOW)
#define POSBAND_HIGH_INT INT_PERCENT_OF_RANGE(POSBAND_HIGH)
#define NEARPOS_LOW_INT INT_PERCENT_OF_RANGE(NEARPOS_LOW)
#define NEARPOS_HIGH_INT INT_PERCENT_OF_RANGE(NEARPOS_HIGH)

extern const pos_t errband_range[Xends];
extern const pos_t nearpos_range[Xends];
extern const ctltick_t errtime_range[Xends];

//defaults
#define DEFAULT_fPositionTime1 (10.0) //sec
#define DEFAULT_fNearPosition (1.0F)
#define DEFAULT_fPositionErrorBand (5.0F)
#define DEFAULT_ERR_DISABLE 0

extern const ErrorLimits_t* pos_GetErrorLimits(ErrorLimits_t *dst);
extern ErrorCode_t pos_SetErrorLimits(const ErrorLimits_t* src);
UNTYPEACC(pos_SetErrorLimits, pos_GetErrorLimits);


#endif //ERRLIMITS_H_
/* This line marks the end of the source */
