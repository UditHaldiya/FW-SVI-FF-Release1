/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pressures.h
    \brief Pressures-related basic shared definitions

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/pressuredef.h $
    $Date: 11/14/11 11:37p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: pressuredef.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/14/11   Time: 11:37p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 9:11p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Pressures-related basic shared definitions
*/
#ifndef PRESSUREDEF_H_
#define PRESSUREDEF_H_

/* ------------------------------ pressure -----------------------------------
   Physical units - PSI (native), KPA, BAR
   Mapping - [0..60]PSI<==>[0..15000]
*/
// **LS 7/14/06 This is the new scaling to allow 150 psi to fit into s16
//      Pressure scaling is 60 psi = 12000 or equivalently 150 psi = 30000
//Presentation
// definitions of HART represenations of pressure units
#define H_PSI 6
#define H_BAR 7
#define H_KPA 12
#define UNITSID_PRESSURE_a 0
#define UNITSID_PRESSURE_b 12000.0 //internal counts;
#define UNITS_PSI_A 0.0
#define UNITS_KPA_A 0.0
#define UNITS_BAR_A 0.0
#define UNITS_PSI_B (60.0) //PSI value of the UNITSID_PRESSURE_b counts
#define UNITS_KPA_B (UNITS_PSI_B*PRESSURE_FACTOR_PSI2KPA)
#define UNITS_BAR_B (UNITS_PSI_B*PRESSURE_FACTOR_PSI2BAR)
#define UNITS_PSI_N 1
#define UNITS_KPA_N 1
#define UNITS_BAR_N 3
#define UNITS_PSI_N_ENTRY 2
#define UNITS_KPA_N_ENTRY 2
#define UNITS_BAR_N_ENTRY 4

/* A type representing pressure must be sufficient to hold 120 PSI and withstand
   application-defined scalings, without overflow.
   This may (and currently does) impose restrictions on valid scaling factors; the
   numeric values of the constraints ABSOLUTELY must be checked; preferably at
   compile time (second choice is runtime code optimized out by the compiler, last
   resort is error_SetFault()).
*/
//FUTURE lint -strong(AXJ, pres_t)
typedef s16 pres_t;
//FUTURE lint -strong(AzXJ, pres_least_t) -father(s16_least, pres_least_t)
typedef s16_least pres_least_t;

//Units conversion factors
#define PRESSURE_FACTOR_PSI2BAR 0.06895 //true value, 0.068947625, might not yield a better conversion
#define PRESSURE_FACTOR_PSI2KPA 6.895 //true value, 6.8947625, might not yield a better conversion

#define STD_FROM_PSI(x) ((pres_t)(((UNITSID_PRESSURE_b-UNITSID_PRESSURE_a)*(x))/(UNITS_PSI_B-UNITS_PSI_A)))  //converts psi to counts

#define PRESSURE_INVALID INT16_MIN //!< Trap representation for pressure
CONST_ASSERT(PRESSURE_INVALID < -STD_FROM_PSI(UNITS_PSI_B));

//constants used by FPressureToPressure and limits
#define PRESSURE_MAX_PSI (160.0) //! upper range limit of pressure in psi
#define PRESSURE_MAX_STD (STD_FROM_PSI(PRESSURE_MAX_PSI))
#define PRESSURE_MIN_PSI (-5.0) //! lower range limit of pressure in psi
CONST_ASSERT(PRESSURE_INVALID < STD_FROM_PSI(PRESSURE_MIN_PSI));

#define LOW_PRESSURE_DEF (STD_FROM_PSI(0.0)) //! default user cal value

#endif //PRESSUREDEF_H_
/* This line marks the end of the source */
