/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors/pressmon_pilot_hw.h $
    $Date: 12/21/10 2:06p $
    $Revision: 3 $
    $Author: Justin Shriver $

    CPU: Any

    \file Pressmon_pilot_hw.h
    \brief Hardware specific constants for SVI2AP

*/
/* $History: pressmon_pilot_hw.h $
 * 
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 12/21/10   Time: 2:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:4615
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 3:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:2489
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 3:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/Monitors
 * TFS:2489
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 12:30p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 12:12p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/Monitors
 * TFS:2259
 * TFS:2543
*/


//#ifndef IN_ActuatorOutputMonitors
//#error("Pressure subsystem file improperly included")
//#endif

#ifndef PRESSMON_PILOT_HW_H_
#define PRESSMON_PILOT_HW_H_

#include "pressmon_sub.h"
//Defines for isPilitPressureAtLimit
//These are the minimum pressures
//This is based on the fact that RELAY (AP-1) and AP-2 have minimum relay pressures (zeros)
//at around 7.5psi
#define PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY STD_FROM_PSI(6.0F)
//This is based on the pressure that is required to generate -100psi
#define PILOT_LOW_PRESS_LIMIT_DOUBLE_ACTING STD_FROM_PSI(4.0F)
//This is based on the pressure to reach maximum pressure
#define PILOT_HIGH_PRESS_LIMIT_DOUBLE_ACTING STD_FROM_PSI(30.0F)
#define PRESSURE_LIMIT_AP_1 80.0F
#define PRESSURE_LIMIT_AP_2 100.0F
#define GAIN_AP_1  6.821F
#define MAX_ZERO_AP_1  7.987F
#define GAIN_AP_2  7.134F
#define MAX_ZERO_AP_2  8.263F
#define PILOT_HIGH_PRESS_LIMIT_AP_1 STD_FROM_PSI(MAX_ZERO_AP_1 + (PRESSURE_LIMIT_AP_1/GAIN_AP_1))
#define PILOT_HIGH_PRESS_LIMIT_AP_2 STD_FROM_PSI(MAX_ZERO_AP_2 + (PRESSURE_LIMIT_AP_2/GAIN_AP_2))

#endif //END HEADER GUARD
