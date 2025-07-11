/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_sigsp.h
    \brief UI service functions prototypes

     CPU: Any

    OWNER: AK

    \ingroup UI
*/
#ifndef UI_SIGSP_H_
#define UI_SIGSP_H_

#include "uidef.h"

//Temporary data storage area
struct hiloSignal_t
{
    s16 nLowSignal;
    s16 nHighSignal;
    s16 lowupperlim, highlowerlim; //upper for low, lower for upper
};
typedef struct hiloSignal_t hiloSignal_t;
extern hiloSignal_t hiloSignal; //!< signal data for UI
#define UI_SIGNALDATAINDEX(member) (MN_OFFSETOF(hiloSignal_t, member)/sizeof(s16))
extern s16 *ui_getHiLoSignal(void);
//low/high signal limits adjustment
extern bool_t ui_AdjustSigLimits(const uistate_t *state);
extern uivalvar_t *ui_GetSignal(void);

extern bool_t ui_getHiLoSignalData(void);
extern bool_t ui_setHiLoSignalData(void);

#endif //UI_SIGSP_H_
