//Goals for module
//1. Module should be configurable based on available sensors and system features
//2. Module should not require code that cannot be used in a project
//
//Goals for this file
//1. Store all types that should be shared across the module
//2. Provide place for doxygen module description
//3. This file should not change based on integration requirements
//4. Parts of a module
//
//Derived designe requirements
//This file contains only definitions that are required for any actuator monitoring
//A pressmon_int.h file contains all definitions that are integration specific
//It may require that certain C files are not included in the project
/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/pressmon.h $
    $Date: 12/21/10 1:59p $
    $Revision: 8 $
    $Author: Justin Shriver $

    CPU: Any

    \ingroup ActuatorOutputMonitors
    \brief Module Header

*/

/** \addtogroup ActuatorOutputMonitors  Monitors for actuator output saturating
\brief This module provides monitors for actuator output saturation
\htmlonly
<a href="http://mymasoneilan/en/Engineering/innovations/Team%20Documents1/Projects/SVI2AP/3.1.5/Requirements/Extended%20Signature%20--%202194/Req_ExtendedDiagnosticSignature.doc"> Requirements document </a><br>
<a href="N/A"> Design document </a><br>
<a href="TBD"> Unit Test Report [1]</a><br>
<a href="TBD"> Unit Test Report [2]</a><br>
\endhtmlonly
*/



/* $History: pressmon.h $
 * 
 * *****************  Version 8  *****************
 * User: Justin Shriver Date: 12/21/10   Time: 1:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4615
 *
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 1/27/10    Time: 7:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 7:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 3:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 3:51p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 3:14p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
*/

#ifndef PRESSMON_H_
#define PRESSMON_H_

#include "mnwrap.h" //for basic types
//#define IN_ActuatorOutputMonitors
//#define IN_ActuatorOutputMonitors
#include "pressmon_sub.h"
#include "pressmon_act.h" //actuator based pressure monitoring
#include "pressmon_pilot.h" //pilot based pressure monitoring
//#undef IN_ActuatorOutputMonitors
#endif //END HEADER GUARD
