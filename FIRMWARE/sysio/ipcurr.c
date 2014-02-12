/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ipcurr.c
    \brief System-level data and services related to I/P current feedback read

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/ipcurr.c $
    $Date: 11/15/11 11:34a $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: ipcurr.c $
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
 * User: Arkkhasin    Date: 4/01/09    Time: 1:16a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * sysio_SetForcedOutput_OOS moved from sysio.c to ipcurr.c and became
 * ipcurr_SetForcedOutput_OOS.
 * sysio.c is now neatly dedicated to UI I/O only.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:09p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * SYSIO-level stuff extracted from where it was ad hoc.
 * Now eqipped with new temperature correction scheme
*/

#include "mnwrap.h"
#include "ipcurr.h"
#include "oswrap.h"
#include "devicemode.h"


static ipcurr_t ipcurr;

TEMPCOMP_C_ENTRY(ipcurr) =
{
    .AllowedValue =
    {
#if 0
        //Look at ESD - which makes more or less sense?
        [Xlow] = IP_STD_FROM_MA(-0.1),
        [Xhi] = IP_STD_FROM_MA(2.1), //LCX limit is perhaps different
#else
        //Look at AP - which makes more or less sense?
        [Xlow] = STD_FROM_MA(-0.1),
        [Xhi] = STD_FROM_MA(30.0),
#endif
    },
    .fcode = FAULT_IP_OUT_OF_RANGE,
    .ErrLimit = ERRLIMIT_DEFAULT,
};



/** \brief Required channel function
\param adval - A/D reading in raw counts.
NOTE: division of labor between ipcurr_Update and ipcurr_GetCompensatedIpCurrent
is opportunistic and can be changed as needed
*/
void ipcurr_Update(ADRaw_t adval)
{
    ipcurr = ipcurr_TempComp(adval);
}

/** \brief Required accessor function
\return Temperature-corrected feedback read of I/P  current.
NOTE: division of labor between ipcurr_Update and ipcurr_GetCompensatedIpCurrent
is opportunistic and can be changed as needed
*/
ipcurr_t ipcurr_GetCompensatedIpCurrent(void)
{
    return ipcurr;
}

/* This line marks the end of the source */
