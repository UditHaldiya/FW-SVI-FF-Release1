/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file inpv.c
    \brief System-level data and services related to PV read

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/inpv.c $
    $Date: 11/15/11 11:34a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: inpv.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 11:34a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 2:08p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Light linting
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:09p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * SYSIO-level stuff extracted from where it was ad hoc.
 * Now eqipped with new temperature correction scheme
*/

#include "mnwrap.h"
#include "inpv.h"
#include "configure.h"

TEMPCOMP_C_ENTRY(inpv) =
{
    .AllowedValue =
    {
        [Xlow] = STD_FROM_MA(-0.1),
        [Xhi] = STD_FROM_MA(30.0),
    },
    .fcode = FAULT_CURRENT_SENSOR_2ND,
    .ErrLimit = ERRLIMIT_DEFAULT,
};

static sig_t  m_PV;           //temp compensated PV, self-healing at long A/D cycle rate

/** \brief Required channel function
\param adval - A/D reading in raw counts.
NOTE: division of labor between inpv_Update and inpv_GetPV
is opportunistic and can be changed as needed
*/
void inpv_Update(ADRaw_t adval)
{
    m_PV = (sig_t)inpv_TempComp(adval); //cast OK because of limits in TEMPCOMP_C_ENTRY(inpv)
}

/** \brief Required accessor function
\return Temperature-corrected feedback read of I/P  current.
NOTE: division of labor between inpv_Update and inpv_GetPV
is opportunistic and can be changed as needed
*/
sig_t inpv_GetPV(void)
{
    sig_t ret;
    if(cnfg_GetOptionConfigFlag(PV_INPUT))  //see if we even have PV
    {
        ret = m_PV;
    }
    else
    {
        ret = 0; //dummy
    }
    return ret;
}

/* This line marks the end of the source */
