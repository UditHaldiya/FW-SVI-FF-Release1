/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_locks.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App. Only the UI graph tables need to see any functions here.
    Buttons lock part.

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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_locks.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_locks.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 5/28/10    Time: 1:07a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Partial taming of headers toward modularization. No code change
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/04/09    Time: 7:22p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * First working implementation of UI pushbutton locks with password
 * override - "CLR ERR" only
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/09    Time: 10:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Intermediate check-in for button locks with password override
*/
#include "mnwrap.h"
#include "bitutils.h"
#include "numutils.h"
#include "uidef.h"
#include "ui_locks.h"
#include "wprotect.h"

const s16 ui_password_rv[Xends] =
{
    [Xlow] = INT16_MIN,
    [Xhi] = INT16_MAX,
};

static uipass_t uipass;

uivalvar_t *ui_GetCurrPassword(void)
{
    return &uipass.pass;
}

/** \brief A "node enable" function
\return A pointer to UI configuration
*/
const void *ui_GetLocalUIDataPtr(void)
{
    uipass.blmask = 0; //indicate the lock is effective
    return ui_GetLocalUIData(NULL);
}

/** A node function
    \brief checks write protect.
"false" disables the UI failure node, hence the somewhat counter-intuitive value inversion
    \return false if protected, true if write-enabled
*/
bool_t ui_isWriteProtected(const uistate_t *state)
{
    UNUSED_OK(state);
    return bios_ReadWriteProtectInput()?false /*enable */ :true /*disable*/; /*... the failure node */
}

/** \brief A node enable function: UI Password check
\return true  (to enable a failure node) iff: (locked and password mismatch or absent)
*/
const void *ui_isPasswordMatch(void)
{
    const UIData_t *p = ui_GetLocalUIData(NULL);
    uipass.match = bool2int(((uipass.pass == p->Password) && p->PasswordEn) || (uipass.blmask != 0));
    return &uipass.match;
}

/** \brief A node table-enable function for locks
\return a bitmap of locked access levels
*/
const void *ui_GetLocalUILocks(void)
{
    uipass.pass = 0; //initialize the password
    buttonlock_t bl = ui_GetLocalUIData(NULL)->ButtonLock;
    u8_least mask = 1U<<(u8)bl;
    mask = (mask<<1) - 1U;
    uipass.blmask = (u8)mask;
    return &uipass.blmask;
}


/* This line marks the end of the source */
