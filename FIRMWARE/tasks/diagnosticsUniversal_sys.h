/**
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file diagnosticsUniversal.h
    \brief Interface from diagnosticsUniversal to system

    OWNER: LS
    CPU: Any

    $Revision: 1 $

*/



/*
    $History: diagnosticsUniversal_sys.h $
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 7/28/10    Time: 5:52p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:3543 -- Integrate 3.2.1 diagnostics into LCX.
 * TFS:3599 -- Remove unnecessary header guards
 * TFS:3656 -- Integration into new LCX structures
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 12:33p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 12:09p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 1/25/10    Time: 1:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 3:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2499
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 5:09p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2429
*/


#ifndef DIAGNOSTICSUNIVERSAL_SYS_H_
#define DIAGNOSTICSUNIVERSAL_SYS_H_

#include "errcodes.h"  //module

#include "devicemode.h"
#include "pressures.h"
#include "control.h"

#include "faultpublic.h"

//move to diagnosticsUniversal.h after development
extern procresult_t diag_Run_ExtActuatorSignatureOpen_Internal(void (*sample_func)(diag_t data[2]), taskid_t TaskContext,
                     pos_t StartPosition, pos_t EndPosition, u16 Speed, u8_least DiagDirection);


#endif //DIAGNOSTICSUNIVERSAL_SYS_H_

/* This line marks the end of the source */

