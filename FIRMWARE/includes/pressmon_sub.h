/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/pressmon_sub.h $
    $Date: 12/13/10 7:02p $
    $Revision: 2 $
    $Author: Justin Shriver $

    CPU: Any
    \file pressmon_sub.h
    \ingroup ActuatorOutputMonitors
    \brief Module Integration

*/

/** \addtogroup ActuatorOutputMonitors  Monitors for actuator output saturating
\htmlonly
<a href="http://mymasoneilan/en/Engineering/innovations/Team%20Documents1/Projects/SVI2AP/3.1.5/Requirements/Extended%20Signature%20--%202194/Req_ExtendedDiagnosticSignature.doc"> Requirements document </a><br>
<a href="N/A"> Design document </a><br>
<a href="TBD"> Unit Test Report [1]</a><br>
<a href="TBD"> Unit Test Report [2]</a><br>
\endhtmlonly
*/




/* $History: pressmon_sub.h $
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 12/13/10   Time: 7:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:3955 Removing custom guard
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 12:33p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 *
Brought in from 3.2.1
*/


#ifndef PRESSMON_SUB_H_
#define PRESSMON_SUB_H_

#include "pressuredef.h"
//Type for pressure limit
typedef enum {actmon_ACT_LIM_NONE, actmon_ACT_LIM_LOW, actmon_ACT_LIM_HIGH} limit_t;
//Type for pressure (Justin-specific)
typedef pres_t pressure_t;

#endif //END HEADER GUARD
