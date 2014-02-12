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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/ipcurr.h $
    $Date: 12/09/11 1:50p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: ipcurr.h $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 1:50p
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
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/01/09    Time: 1:16a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * sysio_SetForcedOutput_OOS moved from sysio.c to ipcurr.c and became
 * ipcurr_SetForcedOutput_OOS.
 * sysio.c is now neatly dedicated to UI I/O only.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:09p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * SYSIO-level stuff extracted from where it was ad hoc.
 * Now eqipped with new temperature correction scheme
*/
#ifndef IPCURR_H_
#define IPCURR_H_

#include "signaldef.h"
#include "tempcomp.h"

/* Feedback read of I/P current */
typedef tcorr_t ipcurr_t;

extern void ipcurr_Update(ADRaw_t adval);
extern ipcurr_t ipcurr_GetCompensatedIpCurrent(void);

TEMPCOMP_H_ENTRY(ipcurr);

#endif //IPCURR_H_
/* This line marks the end of the source */
