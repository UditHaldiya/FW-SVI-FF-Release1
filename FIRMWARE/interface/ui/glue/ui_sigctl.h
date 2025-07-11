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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_sigctl.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_sigctl.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 2  *****************
 * User: Derek Li     Date: 6/04/09    Time: 3:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Added some functions to support A02 signal(Retransmit)
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:29a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * UI glue for main output signal generator
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

#ifndef UI_SIGCTL_H_
#define UI_SIGCTL_H_

#include "uiglue_common.h"

extern const void *ui_CheckFrozenAO1(void); //"node enable" function
extern const void *ui_CheckFrozenAO2(void); //"node enable" function

extern bool_t ui_UnfreezeAO1(const uistate_t *state);
extern bool_t ui_UnfreezeAO2(const uistate_t *state);

extern uivalvar_t *ui_GetSignalAO2(void);

extern void ui_SetFixedSignalAO1(void);
extern void ui_SetFixedSignalAO2(void);

#endif //UI_SIGCTL_H_
/* This line marks the end of the source */
