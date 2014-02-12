/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file param.h
    \brief Device-wide const parameters and derived constants

    An important part of this file is a map between the physical world and
    internal representation of the physical quantities.
    The goal is to define quantities in physical terms and have the compiler
    do the conversion.
    The following physical variables are used in the MNCB:
    - position
    - pressure
    - signal current
    - temperature
    - time
    Each of them may have the following properties:
    - defined internal representation in integer format
    - defined conversion of internal format from and to a floating-point format
    - defined display format
    - fundamental constants and/or tuning parameters related to the variables

    i. Internal representation

    ii. Conversion to floating point

    iii.

     CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/

/* $History: param.h $
 *
 * *****************  Version 32  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 1:55p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8370 - header untangling - no code change
 *
 * *****************  Version 31  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:05p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8313 Lint
 *
 * *****************  Version 30  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 4:31p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 28  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 4:43p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:2489
 *
 * *****************  Version 25  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 3:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2304
 * TFS:2259
*/

#ifndef PARAM_H_
#define PARAM_H_

//deprecated
#include "position.h"

//position range
#define MAX_ALLOWED_POSITION ((2*STANDARD_100)-1)
#define MIN_ALLOWED_POSITION (-MAX_ALLOWED_POSITION)

//---------- limits ----------------

//natural limits
/*lint -emacro((912,506), LIMIT16)
  912: We do want to take advantage of C automatic type promotion here
  in the argument of LIMIT16
  506: Limiting a const value will have a const comparison (bool). Duh.
*/
#define LIM16 32767 //Do not change: it's the nature of things
#define HILIMIT16(x) (((x)>LIM16)?LIM16:(x))
#define LOLIMIT16(x) (((x)<-LIM16)?(-LIM16):(x))
#define LIMIT16(x) ((s16)HILIMIT16(LOLIMIT16(x)))

//setpoint limits
//human(e)-readable limits: define here but don't use
#define SETPOINT_INTENDED_MAX (1.75) //105% of range from zero
#define SETPOINT_INTENDED_MIN (-0.05) // -5% of range from zero
#define OPENSTOP_ADJ_INTENDED_MAX (1.0) //100% of range from zero
#define OPENSTOP_ADJ_INTENDED_MIN (0.6) //60% of range from zero

//machine-readable limits: use these in the code but don't define directly
//In integer conversion of limits, we want the floor of the upper limit and the ceiling
//of the lower limit
#define BIG_NUMBER 1000000
#define SETPOINT_MAX LIMIT16((s32)(STANDARD_ZERO + (SETPOINT_INTENDED_MAX*STANDARD_RANGE)))
#define SETPOINT_MIN LIMIT16(BIG_NUMBER - (s32)(BIG_NUMBER - (STANDARD_ZERO + (SETPOINT_INTENDED_MIN*STANDARD_RANGE))))
#define OPENSTOP_ADJ_MAX LIMIT16((STANDARD_ZERO + (OPENSTOP_ADJ_INTENDED_MAX*STANDARD_RANGE)))
#define OPENSTOP_ADJ_MIN LIMIT16(BIG_NUMBER - (s32)(BIG_NUMBER-(STANDARD_ZERO + (OPENSTOP_ADJ_INTENDED_MIN*STANDARD_RANGE))))

#endif //PARAM_H_
/* This line marks the end of the source */
