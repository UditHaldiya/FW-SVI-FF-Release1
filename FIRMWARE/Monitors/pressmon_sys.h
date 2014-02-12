/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors/pressmon_sys.h $
    $Date: 5/16/11 6:34p $
    $Revision: 8 $
    $Author: Arkkhasin $

    CPU: Any

    \ingroup Actuator Monitor -- actmon
    \brief Performs pressure monitoring for out of control detection.

*/
/* $History: pressmon_sys.h $
 * 
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 5/16/11    Time: 6:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:6336: dummy faults necessary for the build moved away from the
 * fault matrix to dummyfaults.h
 * 
 *
 * *****************  Version 7  *****************
 * User: Justin Shriver Date: 12/21/10   Time: 2:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:4615
 *
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 3:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:2489
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 3:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/Monitors
 * TFS:2489
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 1/27/10    Time: 7:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/Monitors
 * TFS:2259
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 3:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/Monitors
 * TFS:2259
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 3:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/Monitors
 * TFS:2304
 * TFS:2259
*/


//#ifndef IN_ActuatorOutputMonitors
//#error("Pressure subsystem file improperly included")
//#endif

#ifndef PRESSMON_SYS_H_
#define PRESSMON_SYS_H_

//#include "errcodes.h"
#include "cycle.h" //for cycle_GetRawPressureData
#include "configure.h" //for cnfg_GetComputedConfigurationData()
#include "faultpublic.h"
#include "pressmon_sub.h"
#include "dummyfaults.h"




bool_t isSingleActing(void);
bool_t isLowCost(void);




#endif //END HEADER GUARD
