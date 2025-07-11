/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_calib.h
    \brief API to support UI MNCB calib menu(s)

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_calib.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

*/
/* $History: ui_calib.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 10:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Compartmentalizing the UI glue layer
*/
#ifndef UI_CALIB_H_
#define UI_CALIB_H_

//get/set calib data
extern bool_t ui_getCalibMenuData(const uistate_t *state);
extern bool_t ui_setCalibMenuData(const uistate_t *state);

#endif //UI_CALIB_H_
