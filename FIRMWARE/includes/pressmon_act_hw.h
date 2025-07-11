/**
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors/pressmon_act_hw.h $
    $Date: 12/21/10 2:04p $
    $Revision: 5 $
    $Author: Justin Shriver $

    CPU: Any

    \ingroup ActuatorOutputMonitors
    \brief Hardware specific constants for SVI2AP

*/
/* $History: pressmon_act_hw.h $
 * 
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 12/21/10   Time: 2:04p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:4615
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 2/02/10    Time: 3:12p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/Monitors
 * TFS:2489
 * TFS:2259
 * TFS:2543
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

#ifndef PRESSMON_ACT_HW_H_
#define PRESSMON_ACT_HW_H_
//#include "pressmon_sub.h"

//Defines for isActuatorPressureAtLimite
//This is the minimum actuator pressure
#define ACTUATOR_LOW_PRESS_LIMIT                STD_FROM_PSI(1.0F)
//This pair allows one to calculate how much error to expect as a function of target pressure
#define SUPPLY_FACTOR (STD_FROM_PSI(150.0F) / STD_FROM_PSI(3.0F))   //!< slope as a function of supply
#define ACTUATOR_TO_SUPPLY_PRESS_TOLERANCE(x)  (STD_FROM_PSI(2.0F) + ((x) / SUPPLY_FACTOR))
//This defines the maximum pressure differential we expect, regardless of supply pressure
#define ACTUATOR_DIFFERENTIAL_LIMIT             STD_FROM_PSI(80.0F)
//This is reserved for future use, it allows one to potentially compensate for
//the fact that there are more errors in double acting P1-P2 vs P1
#define DOUBLE_ACTING_ADJUSTMENT STD_FROM_PSI(0.0F)

#endif //END HEADER GUARD
