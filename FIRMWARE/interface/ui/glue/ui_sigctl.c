/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_sigctl.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App. Only the UI graph tables need to see any functions here.
    Buttons lock part.


         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_sigctl.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_sigctl.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 2  *****************
 * User: Derek Li     Date: 6/04/09    Time: 3:32p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Added some functions to support A02 signal(Retransmit)
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:29a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * UI glue for main output signal generator
*/
#include "mnwrap.h"
//#include "bitutils.h"
//#include "mnassert.h"
//#include "errcodes.h"
#include "uidef.h"
#include "ui_sigctl.h"
#include "ao.h"

static u8 ui_FrozenAO1;
static u8 ui_FrozenAO2;
static uivalvar_t SignalAO2;

uivalvar_t *ui_GetSignalAO2(void)
{
    SignalAO2 = ao_GetAOValue(AO_CHANNEL_POS_RETRANSMIT);
    return &SignalAO2;
}

/** \brief A "node enable" function
\return A pointer to UI configuration
*/
const void *ui_CheckFrozenAO1(void)
{
    ui_FrozenAO1 = bool2int(ao_IsFixedAO(AO_CHANNEL_LOOP_CURRENT));
    return &ui_FrozenAO1;
}

const void *ui_CheckFrozenAO2(void)
{
    ui_FrozenAO2 = bool2int(ao_IsFixedAO(AO_CHANNEL_POS_RETRANSMIT));
    return &ui_FrozenAO2;
}

/** A node function
    \brief unfreezes AO1.
    \return false
*/
bool_t ui_UnfreezeAO1(const uistate_t *state)
{
    UNUSED_OK(state);
    ao_Freeze(AO_CHANNEL_LOOP_CURRENT, false);
    return false;
}

bool_t ui_UnfreezeAO2(const uistate_t *state)
{
    UNUSED_OK(state);
    ao_Freeze(AO_CHANNEL_POS_RETRANSMIT, false);
    return false;
}

/** \brief A var action function
*/
void ui_SetFixedSignalAO1(void)
{
    (void)ao_SetAOValueFixed(AO_CHANNEL_LOOP_CURRENT, *ui_getMiscVar()); //where the local var is held. AK:TODO: make a private place
}

void ui_SetFixedSignalAO2(void)
{
    (void)ao_SetAOValueFixed(AO_CHANNEL_POS_RETRANSMIT, SignalAO2); //where the local var is held. AK:TODO: make a private place
}
/* This line marks the end of the source */
