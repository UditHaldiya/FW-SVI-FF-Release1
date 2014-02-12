/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file dummyfaults.h
    \brief Faults technically required for build but not making user sense

    This file indicates afterthoughts and weaknesses. In the ideal world it
    should not exist

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP/dummyfaults.h $
    $Date: 12/05/11 4:48p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup Faultman
*/
/* $History: dummyfaults.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:48p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8347
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/16/11    Time: 6:33p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:6336: dummy faults necessary for the build moved away from the
 * fault matrix to dummyfaults.h
 *
*/

/* NOTE: This header intentionally omits standard guards */

#if 0
#define FAULT_STD_DIAGNOSTICS_FAILED ( (faultcode_t)12)
#define FAULT_EXT_DIAGNOSTICS_FAILED ( (faultcode_t)13)
#define FAULT_CURRENT_SENSOR_2ND ( (faultcode_t)37)
#define FAULT_PRESSURE1 ( (faultcode_t)38)
#define FAULT_PRESSURE2 ( (faultcode_t)39)
#define FAULT_PRESSURE3 ( (faultcode_t)40)
#define FAULT_WATCHDOG_TIMEOUT ( (faultcode_t)43)
#endif

/* This line marks the end of the source */
