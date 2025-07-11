/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_uisetup.c
    \brief UI glue layer functions to support UI configuration

    (Originally Extracted from uifunc.c ver. 138)

         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_uisetup.c $
    $Date: 1/06/12 3:16p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_uisetup.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:16p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:50a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * glue for UI's own setup functionality extracted here
*/

#include "mnwrap.h"
#include "uipublic.h"
#include "uidef.h"
#include "ui_uisetup.h"

/** \brief Increments the local language id modulo the number of languages in the pack.
Informs the configure module.
\return false (to enable the node exit)
*/
bool_t ui_LangIncrement(const uistate_t *state)
{
    UNUSED_OK(state);
    UIData_t uidata;
    (void)ui_GetLocalUIData(&uidata);

    ++uidata.LLanguageId;
    (void)ui_RepairSetUIData(&uidata); //always succeeds
    return false;
}

/* This line marks the end of the source */
