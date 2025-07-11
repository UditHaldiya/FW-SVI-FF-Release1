/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_common.h
    \brief UI glue layer functions to support common UI functionality

         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_common.h $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_common.h $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/09/09    Time: 11:25p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * If level measurement is not available, UI displays "!LEVEL" without
 * units or number in the cycling menu.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:47a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Commonly used UI glue hookups extracted here
*/

#ifndef UI_COMMON_H_
#define UI_COMMON_H_
extern const s16 *ui_GetMNCBVerNumber(void);

extern const void *ui_GetOptionConfig(void);

//Common processes
extern bool_t ui_process_SelfTune(const uistate_t *state);

//------------ generic process -----------------------
MN_DECLARE_API_FUNC(ui_getProcId, ui_GetProcPercentComplete, ui_ReadProcPercentComplete) //may or may not be used in a particular UI graph
extern uivalvar_t *ui_GetProcPercentComplete(void);
extern bool_t ui_ReadProcPercentComplete(const uistate_t *state);
extern const void *ui_getProcId(void);

extern bool_t ui_HavePosition(const uistate_t *state);
extern bool_t ui_DontHavePosition(const uistate_t *state);

#endif //UI_COMMON_H_
/* This line marks the end of the source */
