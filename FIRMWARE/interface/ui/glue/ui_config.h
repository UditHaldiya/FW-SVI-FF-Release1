/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_config.h
    \brief Macros to support UI CONFIG menu

    CPU: Any

    OWNER: AK

*/

#ifndef UI_CONFIG_H_
#define UI_CONFIG_H_

#include "uidef.h"
#include "ctllimits.h"
#include "poscharact.h"
#include "selftune.h"


extern const void *ui_GetAirAction(void);
extern const void *ui_GetSpCharact(void);
extern CtlLimits_t *ui_GetCtlLimits(void);
extern const void *ui_GetPidParamSet(void);

//get/set conf data
extern bool_t ui_GetSysCtlLimits(void);
extern bool_t ui_GetSysAirAction(void);
extern bool_t ui_GetSysSpCharact(void);
extern bool_t ui_GetSysPidParamSet(void);

extern bool_t ui_SaveSysCtlLimits(void);
extern bool_t ui_SaveSysAirAction(void);
extern bool_t ui_SaveSysSpCharact(void);
extern bool_t ui_SaveSysPidParamSet(void);
extern bool_t ui_SaveSysLanguage(void);

// invoked by ui enable list
MN_INLINE const void *ui_GetUntypedCtlLimits(void)
{
    return ui_GetCtlLimits();
}

//invoked by tight shut off hi/low
MN_INLINE uivalvar_t *ui_GetTightShutoff(void)
{
    return ui_GetCtlLimits()->TightShutoff;
}
#endif //UI_CONFIG_H_
