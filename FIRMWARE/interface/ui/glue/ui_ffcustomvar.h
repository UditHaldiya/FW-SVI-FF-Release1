/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_ffcustomvar.h
    \brief interface to support custom FF variables

    CPU: Any

    OWNER: ej

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_ffcustomvar.h $
    $Date: 1/09/12 3:15p $
    $Revision: 1 $
    $Author: ericj $

*/
/* $History: ui_ffcustomvar.h $
 * *****************  Version 1  *****************
 * User: eric j    Date: 1/07/12    Time: 10:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Compartmentalizing the UI glue layer
*/
#ifndef UI_FFCUSTOMVAR_H_
#define UI_FFCUSTOMVAR_H_

#define FFSTR_VARSTATUS_BAD         "BAD      "
#define FFSTR_VARSTATUS_UNCERTAIN   "UNCERTAIN"
#define FFSTR_VARSTATUS_GOOD        "GOOD     "
#define FFSTR_VARSTATUS_GOODCAS     "GOOD CAS "

/* for cycle menu custom 1 and custom 2*/
extern const void *ui_CycleMenuGetFfCustom1(void);
extern const void *ui_CycleMenuGetFfCustom2(void);

extern u8 ui_ffGetWorkingSP(u8 *str, u8 maxlen);
extern u8 ui_ffGetWorkingPos(u8 *str, u8 maxlen);
extern u8 ui_ffGetPidSp(u8 *str, u8 maxlen);
extern u8 ui_ffGetPidPv(u8 *str, u8 maxlen);
extern u8 ui_ffGetPidOut(u8 *str, u8 maxlen);
extern u8 ui_ffGetPid2Sp(u8 *str, u8 maxlen);
extern u8 ui_ffGetPid2Pv(u8 *str, u8 maxlen);
extern u8 ui_ffGetPid2Out(u8 *str, u8 maxlen);
extern u8 ui_ffGetAi1Out(u8 *str, u8 maxlen);
extern u8 ui_ffGetAi2Out(u8 *str, u8 maxlen);
extern u8 ui_ffGetAi3Out(u8 *str, u8 maxlen);
extern u8 ui_ffGetAoSp(u8 *str, u8 maxlen);
extern s16 *ui_ffGetDoSp(void);
extern s16 *ui_ffGetDo2Sp(void);
extern u8 ui_ffGetIsIn1(u8 *str, u8 maxlen);
extern u8 ui_ffGetIsIn2(u8 *str, u8 maxlen);
extern u8 ui_ffGetIsIn3(u8 *str, u8 maxlen);
extern u8 ui_ffGetIsIn4(u8 *str, u8 maxlen);

#endif //UI_FFCUSTOMVAR_H_
