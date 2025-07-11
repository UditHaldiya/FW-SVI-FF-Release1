/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_sp.c
    \brief UI support functions for manual setpoint

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_sp.h $
    $Date: 1/06/12 3:16p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_sp.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:16p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 11:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * A few small glue layer functions extracted from uifunc.{c,h}
*/

#ifndef UI_SP_H_
#define UI_SP_H_
//get/set setpoint
extern s16 *ui_getManSetpoint(void);
extern void ui_setManSetpoint(void);

extern bool_t ui_IPout_low(const uistate_t *state);
extern bool_t ui_IPout_high(const uistate_t *state);

extern s16 *ui_GetSetpoint(void);

#endif //UI_SP_H_

/* This line marks the end of the source */
