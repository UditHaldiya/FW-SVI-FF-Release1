/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file inpv.h
    \brief API related to System-level PV read

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/inpv.h $
    $Date: 12/09/11 1:57p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: inpv.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 1:57p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8370 - header untangling - no code change
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 11:35a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:09p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * SYSIO-level stuff extracted from where it was ad hoc.
 * Now eqipped with new temperature correction scheme
*/
#ifndef INPV_H_
#define INPV_H_

#include "signaldef.h"
#include "tempcomp.h"

extern  void    MN_API_FUNC(inpv_Update)(ADRaw_t adval);
extern  sig_t   MN_API_FUNC(inpv_GetPV)(void);

TEMPCOMP_H_ENTRY(inpv);

// ---------------------------------- PV (volts) -----------------------------------
#if 0 //alternative (old) low range mapping; no more merits
#define PV_LOW_COUNTS 4000u
#define PV_LOW_VOLTS 1.0
#endif
#define PV_HIGH_COUNTS 20000u
#define PV_HIGH_VOLTS 5.0

//Presentation
#define H_VOLT 58  //hart units for volts (PV)
#define UNITSID_PV_a 0
#define UNITSID_PV_b PV_HIGH_COUNTS
#define UNITS_VOLT_A 0.0
#define UNITS_VOLT_B PV_HIGH_VOLTS
#define UNITS_VOLT_N 4

#endif //INPV_H_
/* This line marks the end of the source */
