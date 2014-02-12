/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file percent.h
    \brief Description of percent data type

Valve position (or, for DLT, level) is computed in several steps of which this file
prepresents the last one (and overal organization).
1. Sensor-specific raw data compensation
2. Sensor-specific linearization
3. Application-specific sensor-independent scaling to the range ("stops"), which may need
    to account for stops drift/adjustment

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/percent.h $
    $Date: 12/09/11 1:49p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: percent.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 1:49p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8370 - header untangling - no code change
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:25a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
*/
#ifndef PERCENT_H_
#define PERCENT_H_
/* A type representing generic percent must be sufficient to hold 199.99% and withstand
   application-defined scalings without overflow.
   This may (and currently does) impose restrictions on valid scaling factors; the
   numeric values of the constraints ABSOLUTELY must be checked; preferably at
   compile time (second choice is runtime code optimized out by the compiler, last
   resort is error_SetFault()).
*/

typedef s16 percent_t;
typedef s16_least percent_least_t;

#define STANDARD_NUMBITS 14
#define STANDARD_100 ((s16)(1U<<STANDARD_NUMBITS)) //16384 ~ 100%
#define STANDARD_ZERO 0 //0 ~ 0%
#define STANDARD_RANGE (STANDARD_100 - STANDARD_ZERO) //of course

//Scaling and Presentation parameters
#define H_PERCENT 57
//units for percent of anything
#define H_PUREPERCENT H_PERCENT
//PURE PERCENT (of anything) - happens to format the same way as position percent of range
#define UNITSID_PERCENT_a STANDARD_ZERO
#define UNITSID_PERCENT_b STANDARD_100
#define UNITS_PUREPERCENT_A 0.0
#define UNITS_PUREPERCENT_B 100.0
#define UNITS_PUREPERCENT_N 2


#endif //PERCENT_H_
/* This line marks the end of the source */
