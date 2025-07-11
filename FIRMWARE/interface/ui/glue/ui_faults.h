/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_faults.h
    \brief UI service functions: glue API of faults interface

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_faults.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_faults.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 5:20p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted from uifunc.{c,h}
*/

#ifndef UI_FAULTS_H_
#define UI_FAULTS_H_

#include "uiglue_common.h"

MN_DECLARE_API_FUNC(ui_IsFailedHigh) //may or may not be used in a project
extern const void *ui_IsFailedHigh(void);
extern const void *ui_GetFaultCodeData(void);
extern bool_t ui_clearFaults(const uistate_t *state);

#endif //UI_FAULTS_H_
/* This line marks the end of the source */
