/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_findstops.c
    \brief UI service functions for "find stops" support (MNCB)

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_findstops.c $
    $Date: 1/09/12 3:14p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_findstops.c $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 3:14p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Adapted to this codebase FBO TFS:8577
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 12:57a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Repaired headers
 *
 * *****************  Version 2  *****************
 * User: Sergey Kruss Date: 4/14/09    Time: 2:30p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Addressed Lint messages
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/31/09    Time: 3:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted MNCB-style find stops support
*/
#include "mnwrap.h"
#include "uipublic.h"
#include "uidef.h"
#include "position.h"
#include "process.h"
#include "uigluehelpers.h"
#include "ui_findstops.h"


//================ Automatic find stops support =========================
/**  A node function
\brief launch "run stops" from pushbuttons
\return true if OK, false if cannot launch
*/
bool_t ui_process_FindStops(const uistate_t *state)
{
    UNUSED_OK(state);
    return ui_process(PROC_FIND_STOPS);
}


//================ Manual find stops support =========================

bool_t ui_CommitStops(const uistate_t *state)
{
    UNUSED_OK(state);

    ErrorCode_t err;

    err = poscal_CommitStops(true);
    return err == ERR_OK;
}

bool_t ui_SetHighStop(const uistate_t *state)
{
    UNUSED_OK(state);
    ErrorCode_t err = poscal_AcceptStop();
    return err == ERR_OK;
}

bool_t ui_SetLowStop(const uistate_t *state)
{
    UNUSED_OK(state);
    ErrorCode_t err = poscal_AcceptStop();
    if(err == ERR_OK)
    {
        err = poscal_CommitStops(false);
    }
    return err == ERR_OK;
}
/* This line marks the end of the source */
