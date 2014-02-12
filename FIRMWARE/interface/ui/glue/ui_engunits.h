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
    \brief UI service functions: glue API of engineering units settings

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_engunits.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* (Optional) $History: ui_engunits.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/29/09    Time: 11:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Made custom units 32-bit with the internal range +-99999
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/30/09    Time: 3:09p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Engineering units extracted from uifunc.c into glue layer folder
*/

#ifndef UI_ENGUNITS_H_
#define UI_ENGUNITS_H_
#include "unitsids.h" //To see if NUM_UNITSIDS_CUSTOMCONV is defined, i.e., if custom units are supported

extern bool_t ui_UnitsIncrement(const uistate_t *state);
#if NUM_UNITSIDS_CUSTOMCONV > 0
//---------- Custom eng. units -------------------
extern bool_t ui_GetCustomUnits(const uistate_t *state);
extern bool_t ui_UnitsSave(const uistate_t *state);
extern bool_t ui_FixURVrange(const uistate_t *state);
extern bool_t ui_FixLRVrange(const uistate_t *state);
extern const void *ui_CheckCustomUnitsSelected(void);
extern uivalvar32_t *ui_GetCurrentCun(void);
extern const uivalvar_t custunits_decimals[Xends];
extern const uivalvar32_t custunits_rv[Xends];
extern uivalvar32_t custunits_middle;
extern UnitsId_t ui_UnitsId;
#endif

#endif //UI_ENGUNITS_H_
/* This line marks the end of the source */
