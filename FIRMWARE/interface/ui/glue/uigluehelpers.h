/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uigluecommon.h
    \brief UI common helper service functions

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/uigluehelpers.h $
    $Date: 1/09/12 5:41p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: uigluehelpers.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 5:41p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Missing headers for prelim. UI wiring FBO TFS:8577
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 6/01/09    Time: 6:23p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/31/09    Time: 3:27p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Coomon helpers header
*/
#ifndef UI_GLUECOMMON_H_
#define UI_GLUECOMMON_H_

MN_DECLARE_API_FUNC(ui_process)
extern bool_t ui_process(ProcId_t proc_id);


#endif //UI_GLUECOMMON_H_
/* This line marks the end of the source */
