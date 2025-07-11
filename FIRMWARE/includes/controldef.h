/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file controldef.h
    \brief Control definitions

Cannot be part of insignal because analog output may use the same scaling
and is optional for projects

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/controldef.h $
    $Date: 1/23/12 4:11p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: controldef.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 4:11p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8762 - Added control output units
*/
#ifndef CONTROLDEF_H_
#define CONTROLDEF_H_

//Presentation - Control output in PSI scaled to nominal AP PWM
//(From TFS:8753)
#define UNITSID_CTLOUT_a 0
#define UNITSID_CTLOUT_b 50000
#define UNITS_CTLPSI_A (-4.60)
#define UNITS_CTLPSI_B ((cdouble_t)(UNITSID_CTLOUT_b-UNITSID_CTLOUT_a)/1100.0 + UNITS_CTLPSI_A)
#define UNITS_CTLPSI_N 5
#define UNITS_CTLPSI_N_ENTRY 5

#define H_CTLPSI 6 //same HART pesentation as H_PSI

#endif //CONTROLDEF_H_
/* This line marks the end of the source */
