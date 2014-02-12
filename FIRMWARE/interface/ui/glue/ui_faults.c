/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_faults.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App. Only the UI graph tables need to see any functions here.
    Faults interface part.

    The glue functions here are designed to work in concert with the macro
ProtectedEntry (see uilocks.inc) to implement a state machine as follows:
Password prompt (timed out in a standard timeout interval) and password entry
are both allowed if:
- the "area" is locked, i.e. 3rd param of ProtectedEntry is >= UI lock level, and
- UI password is enabled.
The "area" is considered unlocked if
- it was not locked, or
- UI password is enabled and the entered password matches UI password

Heavy inter-dependency between node functions is uncharacteristic of the UI design
where the complexity is typically hidden in the UI graph itself, rather than in
node functions.
The reason for this dependency is that ProtectedEntry is a macro which is expected
to appear multiple times, so the size of a subgraph outweighs the size of the
supporting code.

         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_faults.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_faults.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:08p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted from uifunc.{c,h}
 * Eliminated FAULT_LOW_POWER from DLT
*/

#include "mnwrap.h"
#include "bitutils.h"
#include "uidef.h"
#include "faultpublic.h"
#include "ui_faults.h"

//---------- faults with properties ----------
const void *ui_IsFailedHigh(void)
{
    static u8 ret;
    ret = error_IsAnyFaultWithAttributes(FATTRIB_FAILHIGH)?0x01:0x00;
    return &ret;
}

const void *ui_GetFaultCodeData(void)
{
    return error_GetFaultCodeData(NULL);
}

/** A node entry function
    \brief clears faults
    \return always true to disable the node
*/
bool_t ui_clearFaults(const uistate_t *state)
{
    UNUSED_OK(state);
    error_ClearFaults(false); //Leave history codes alone
    return true;
}


/* This line marks the end of the source */
