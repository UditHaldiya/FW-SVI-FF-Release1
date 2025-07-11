/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/pressmon_pilot.h $
    $Date: 12/21/10 2:01p $
    $Revision: 4 $
    $Author: Justin Shriver $

    CPU: Any

    \file pressmon_pilot.h
    \brief System header

*/

/* $History: pressmon_pilot.h $
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 12/21/10   Time: 2:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4615
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 1/27/10    Time: 7:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 4:00p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
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

//#ifndef IN_ActuatorOutputMonitors
//#error("Pressure subsystem file improperly included")
//#endif


#ifndef PRESSMON_PILOT_H_
#define PRESSMON_PILOT_H_

#include "pressmon_sub.h"

bool_t actmon_isPilotPressureAtLimit(limit_t* pilot_limit);

#endif //END HEADER GUARD
