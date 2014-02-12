/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uifunc.h
    \brief UI service functions prototypes

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/uifunc.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: uifunc.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 74  *****************
 * User: Arkkhasin    Date: 11/16/09   Time: 3:20p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * pres_t SpringRange removed from scratchpad_t
 *
 * *****************  Version 73  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:58a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Cleaned up DLT incarnation; uifunc.{c,h} is ready for factoring out
 * (pending MNCB)
 *
 * *****************  Version 72  *****************
 * User: Derek Li     Date: 5/15/09    Time: 2:51p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed ui_GetCouplingValue
 *
 * *****************  Version 71  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:40p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Extracted more UI glue into separate supporting files
 *
 * *****************  Version 70  *****************
 * User: Arkkhasin    Date: 4/08/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Repaired AP and ESD builds
 *
 * *****************  Version 69  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 7:31p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Device modes stuff extracted to ui_devmodes.h
 * MNCB setup stuff extracted to ui_{calib,config}.h
 *
 * *****************  Version 68  *****************
 * User: Arkkhasin    Date: 4/03/09    Time: 7:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 3/31/09    Time: 3:28p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Extracted MNCB-style find stops support
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 3/30/09    Time: 10:03p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Tracking the process requestor is now in process.c
 *
 * *****************  Version 65  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 11:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * A few small glue layer functions extracted from uifunc.{c,h}
 *
 * *****************  Version 64  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:08p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Pressures-related UI node functions moved to ui_pressures.{c,h}
 *
 * *****************  Version 63  *****************
 * User: Arkkhasin    Date: 3/26/09    Time: 12:38a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Better generic API for process support
 *
 * *****************  Version 62  *****************
 * User: Derek Li     Date: 3/06/09    Time: 12:02p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Move calibration and sg glue layer function to ui_calib_sg_dlt.h
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:38a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * UI for main output signal generator
 *
 * *****************  Version 60  *****************
 * User: Derek Li     Date: 2/13/09    Time: 2:08p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Calibration SG Menu glue layer fuctions support
 *
 * *****************  Version 59  *****************
 * User: Arkkhasin    Date: 2/01/09    Time: 10:46p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Intermediate check-in for button locks with password override
 *
 * *****************  Version 58  *****************
 * User: Arkkhasin    Date: 1/30/09    Time: 3:11p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Engineering units extracted from uifunc.c into glue layer folder
 *
 * *****************  Version 57  *****************
 * User: Arkkhasin    Date: 1/27/09    Time: 12:06a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Custom units set and save UI nodes are disabled if the current unit is
 * built-in
 *
 * *****************  Version 56  *****************
 * User: Arkkhasin    Date: 1/24/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Preliminary support for units configuration from local UI - glue layer
 *
 * *****************  Version 55  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 7:44p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Added ui_GetModeData()
 *
 * *****************  Version 54  *****************
 * User: Arkkhasin    Date: 12/26/08   Time: 2:13a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed obsolete inactive UI_LANGUGAGE_NAMED_IN_ALL_LANGUAGES
 *
 * *****************  Version 53  *****************
 * User: Derek Li     Date: 12/18/08   Time: 4:08p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * add FEATURE DLT PLATFORM condition to make other projects build
 * sucessfully
 *
 * *****************  Version 52  *****************
 * User: Derek Li     Date: 12/18/08   Time: 1:10p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * add coupling function
 *
 * *****************  Version 51  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 1:28a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Changes to accommodate the new position input management
 *
 * *****************  Version 50  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:30p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Added get_scratchpad_var() to avoid function pointers casts
 *
 * *****************  Version 49  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 1:21p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Function pointer type casts eliminated
 * Adapted to new API
 *
 * *****************  Version 48  *****************
 * User: Arkkhasin    Date: 3/27/08    Time: 2:56p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * diag_GetPstData() now follows the common API pattern (can fill the
 * data)
 * pst_IsPSTAllowed() now takes a bitmap of allowed sources
 * Characterization (of signal setpoint) moved to charact_sig.{c,h}
 * ConfigurationData_t and corresponding instances and API removed
 * (nothing's left there)
 *
*/
#ifndef UIFUNC_H_
#define UIFUNC_H_

#include "ui_engunits.h"
#include "ui_locks.h"
#include "uiglue_common.h"
#include "ui_devmodes.h"

extern const void *ui_getCostOption(void);

#if FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
extern uivalvar_t *ui_getOpenStopAdjustment_var(void);
#endif

#endif //UIFUNC_H_
/* This line marks the end of the source */
