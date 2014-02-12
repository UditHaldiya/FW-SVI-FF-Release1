/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uiglue_common.h
    \brief UI service functions: glue API of commonly used things

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/uiglue_common.h $
    $Date: 1/09/12 5:41p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: uiglue_common.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 5:41p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Missing headers for prelim. UI wiring FBO TFS:8577
 *
 * *****************  Version 4  *****************
 * User: Derek Li     Date: 5/12/09    Time: 12:20p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Updated function ui_GetPosition to return s32 value
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/08/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Repaired AP and ESD builds
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:38a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * UI for main output signal generator
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/09    Time: 10:34p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Intermediate check-in for button locks with password override
*/

#ifndef UIGLUE_COMMON_H_
#define UIGLUE_COMMON_H_

extern s16 *ui_getMiscVar(void);
extern uivalvar32_t *ui_GetPosition(void);


#endif //UIGLUE_COMMON_H_
/* This line marks the end of the source */
