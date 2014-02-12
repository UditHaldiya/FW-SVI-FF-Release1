/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_findstops.h
    \brief UI service functions for "find stops" support (MNCB)

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_findstops.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_findstops.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/31/09    Time: 3:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted MNCB-style find stops support
*/
#ifndef UI_FINDSTOPS_H_
#define UI_FINDSTOPS_H_

extern bool_t ui_process_FindStops(const uistate_t *state);

//================ Manual find stops support =========================
extern bool_t ui_CommitStops(const uistate_t *state);
extern bool_t ui_SetHighStop(const uistate_t *state);
extern bool_t ui_SetLowStop(const uistate_t *state);

#endif //UI_FINDSTOPS_H_
/* This line marks the end of the source */
