/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_devmodes.h
    \brief API to support UI device mode change

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_devmodes.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

*/
/* $History: ui_devmodes.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted from uifunc.{c,h}
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 10:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Compartmentalizing the UI glue layer
*/
#ifndef UI_DEVMODES_H_
#define UI_DEVMODES_H_
//switch mode
extern bool_t ui_setManMode(const uistate_t *state);
extern bool_t ui_setManModeNaked(const uistate_t *state);
extern bool_t ui_setOperMode(const uistate_t *state);
extern bool_t ui_setOOSMode(const uistate_t *state);
extern bool_t ui_setFailSafeMode(const uistate_t *state);
extern const void *ui_GetModeData(void);
extern const void *ui_GetXModeData(void);
extern const void *ui_GetDevSubmode(void);
//reset
extern bool_t ui_reset(const uistate_t *state);

#endif //UI_DEVMODES_H_
