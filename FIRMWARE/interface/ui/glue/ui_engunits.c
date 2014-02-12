/*
Copyright 2004-2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_engunits.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App in what concerns built-in engineering units.
    Only the UI graph tables need to see any functions here.

    If a function is used as an entry function of an (extended) navigation node,
    the node is still disabled if it is en-table-enabled but the entry function
    returns non-zero. (IMPORTANT: the function *is not* called if the node is
    table-disabled, so the function's side-effects, if any, are not computed.)

    If a function is used as an exit function of an (extended) navigation node,
    it is called every time the node is about to be left to follow the SELECT link
    (usually associated with a * but defined by the node's input translation
    function); the node cannot be left if the function returns non-0.

    NOTE: There is a design pattern where a table-enabled node with an entry
    function serves as a transient UI state, e.g., to show data save error or
    (future?) operator error and such. E.g., with xlate_NextIfEnabled() input
    translation function, such a node falls through if disabled or follows the
    advance link if enabled. If a node is designed to be always disabled, it can
    be achieved by a node function always returning a non-zero (and thus called
    for its side effects). This can do clever things. :)

    This file also contains misc. service functions.

         CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_engunits.c $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* $History: ui_engunits.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:34p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Fixed a problem with proscribed eng. units
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/29/09    Time: 11:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Made custom units 32-bit with the internal range +-99999
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/28/09    Time: 1:52p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Accommodated a check for proscribed units
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/30/09    Time: 3:09p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Engineering units extracted from uifunc.c into glue layer folder
*/
#include "mnwrap.h"
#include "bitutils.h"
//#include "mnassert.h"
#include "errcodes.h" //for configure.h
#include "uidef.h"
#include "fpconvert.h"
#include "interface.h"
#include "customunits.h"
#include "ui_engunits.h"

/** \brief A changer of engineering units modulo number of units for the id in the UI node
\param state - a pointer to the UI state
\return falways false (success)
*/
bool_t ui_UnitsIncrement(const uistate_t *state)
{
    UNUSED_OK(state);
    UnitsId_t uid = state->uid;
    u8_least units_index = fpconvert_GetUnitsIndex(uid);
    ErrorCode_t err;
    u8_least uix = units_index + 1;
    do
    {
        err = fpconvert_SetUnitsByIndex(uid, uix);
        switch(err)
        {
            case ERR_OK:
                break;
            case ERR_NOT_SUPPORTED:
                ++uix;
                break;
            case ERR_INVALID_PARAMETER:
                uix = 0; //wraparound
                break;
            case ERR_I2C_BUS_STUCK:
            case ERR_I2C_NO_DEVICE:
                //This is a special case
                //lint -fallthrough
            default:
                //restore previous value
                (void)fpconvert_SetUnitsByIndex(uid, units_index); //restore
                err = ERR_OK;
                break;
        }

    } while((err != ERR_OK) && (uix != units_index));
    return false;
}

#if NUM_UNITSIDS_CUSTOMCONV > 0
UnitsId_t ui_UnitsId;

static UnitsCust_t unitsPresentation;

static Cun_t uicun; //local UI buffer
const uivalvar_t custunits_decimals[Xends] = {CUN_DECIMALS_MIN, CUN_DECIMALS_MAX};
const uivalvar32_t custunits_rv[Xends] = {CUNITS_A1_MIN, CUNITS_B1_MAX};
uivalvar32_t custunits_middle;

/** \brief Node-Enable function for several custom units nodes
\return a pointer to "custom" flag
*/
const void *ui_CheckCustomUnitsSelected(void)
{
    UnitsId_t uid = ui_UnitsId;
    u8_least units_index = fpconvert_GetUnitsIndex(uid);
    ErrorCode_t err = fpconvert_GetUnitsByIndex(uid, units_index, &unitsPresentation);
    MN_DBG_ASSERT(err == ERR_OK); //retrieved units index must be valid
    return &unitsPresentation.iscustom;
}

/** \brief Entry function to a custom units LRV node
\return false iff success
*/
bool_t ui_FixLRVrange(const uistate_t *state)
{
    UNUSED_OK(state);
    custunits_middle = uicun.rv[Xhi] - CUNITS_MIN_RANGE;
    ErrorCode_t err = fpconvert_SetUnitsByIndex(UNITSID_CUSTOMRANGE, (u8_least)(u16)var_misc - UNITS_DECIMALDIG1_N);
    MN_DBG_ASSERT((err == ERR_OK) || (err == ERR_NOT_SUPPORTED));
    return false;
}

/** \brief Entry function to a custom units URV node
\return false iff success
*/
bool_t ui_FixURVrange(const uistate_t *state)
{
    UNUSED_OK(state);
    custunits_middle = uicun.rv[Xlow] + CUNITS_MIN_RANGE;
    ErrorCode_t err = fpconvert_SetUnitsByIndex(UNITSID_CUSTOMRANGE, (u8_least)(u16)var_misc - UNITS_DECIMALDIG1_N);
    MN_DBG_ASSERT((err == ERR_OK) || (err == ERR_NOT_SUPPORTED));
    return false;
}

/** \brief Exit function from units configuration menu to the menu root node
\return false (enable FAILURE display node) on failure, true on success
*/
bool_t ui_UnitsSave(const uistate_t *state)
{
    UNUSED_OK(state);
    //Only cutom presentation params must be saved: units are saved on the fly
    UnitsId_t uid = ui_UnitsId;
    uicun.index = fpconvert_GetUnitsIndex(uid);
    uicun.decimals = (u8)var_misc; //Legit cast - the number is between 1 and 3
    ErrorCode_t err = fpconvert_SetCustomUnitsConf(uid, &uicun);
    return (err == ERR_OK);
}

/** \brief Exit function from units configuration menu to the menu root node
\return false (enable) on success
*/
bool_t ui_GetCustomUnits(const uistate_t *state)
{
    UnitsId_t uid = ui_getNavnode(state, state->node)->uid;
    ui_UnitsId = uid;
    if(fpconvert_GetCustomUnitsConf(uid, &uicun) == ERR_OK)
    {
        var_misc = uicun.decimals;
    }
    /* Explanation: if the uid has custom units, we populate a private copy.
    Otherwise, nothing happens but we don't care: we'll never use it
    */

    return false;
}

uivalvar32_t *ui_GetCurrentCun(void)
{
    return &uicun.rv[Xlow];
}
#endif //NUM_UNITSIDS_CUSTOMCONV
/* This line marks the end of the source */
