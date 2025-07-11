/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_pressures.c
    \brief UI node service functions to support pressures-related nodes
         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_pressures.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_pressures.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:16p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Pressures-related UI node functions moved to ui_pressures.{c,h}
*/
#include "mnwrap.h"
#include "pressures.h"
#include "ui_pressures.h"
#include "pneumatics.h"

static pres_t main_pressure;
/**  A var node data function
\brief Get pressure for display
\return pointer to a scratchpad location now holding pressure
*/
pres_t *ui_getMainPressure(void)
{
    main_pressure = pres_GetMainPressure(); //new Larry's interface
    return &main_pressure;
}

/**  A var node data function
\brief Get supply pressure for display
\return pointer to a scratchpad location now holding pressure
*/
pres_t *ui_getSupplyPressure(void)
{
    const BoardPressure_t *p;

    p = pres_GetPressureData();
    main_pressure = p->Pressures[PRESSURE_SUPPLY_INDEX];
    return &main_pressure;
}


const void *ui_getPneuParams(void)
{
    return pneu_GetParams(NULL);
}
/* This line marks the end of the source */
