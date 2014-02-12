/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_engunits.h
    \brief UI service functions: glue API of pushbuttons locks

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_locks.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_locks.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/04/09    Time: 7:22p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * First working implementation of UI pushbutton locks with password
 * override - "CLR ERR" only
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/09    Time: 10:34p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Intermediate check-in for button locks with password override
*/

#ifndef UI_LOCKS_H_
#define UI_LOCKS_H_

#include "uiglue_common.h"

extern const void *ui_GetLocalUIDataPtr(void);
extern const void *ui_GetLocalUILocks(void);
extern bool_t ui_isWriteProtected(const uistate_t *state);
extern const void *ui_isPasswordMatch(void);

extern const s16 ui_password_rv[Xends];

extern uivalvar_t *ui_GetCurrPassword(void);

typedef struct uipass_t
{
    s16 pass; //!< current entered value of the password
    u8 blmask; //!< button lock bitmap; guaranteed non-zero if not locked
    u8 match; //!< a {0,1} flag of password match
} uipass_t; //! a private type of password check

#endif //UI_LOCKS_H_
/* This line marks the end of the source */
