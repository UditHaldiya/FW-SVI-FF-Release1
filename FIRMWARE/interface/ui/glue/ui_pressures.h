/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_pressures.h
    \brief UI service functions prototypes for pressures support

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_pressures.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_pressures.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/16/09   Time: 2:43p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Made aware of pressures.h
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:16p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Pressures-related UI node functions moved to ui_pressures.{c,h}
*/
#ifndef UI_PREESURES_H_
#define UI_PREESURES_H_
#include "pressures.h"
#include "pneumatics.h"

extern pres_t *ui_getMainPressure(void);
extern pres_t *ui_getSupplyPressure(void);
extern const void *ui_getPneuParams(void);

#endif //UI_PREESURES_H_
/* This line marks the end of the source */
