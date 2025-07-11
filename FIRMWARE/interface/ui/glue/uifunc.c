/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uifunc.c
    \brief UI node service functions implementing the glue layer between UI
    graph and the App. Only the UI graph tables need to see any functions here.

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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/uifunc.c $
    $Date: 1/30/12 2:14p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup UI
*/
/* (Optional) $History: uifunc.c $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:14p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Removed signal setpoint and loop signal data FBO TFS:8782
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 5:47p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Adaptation to the codebase FBO TFS:8577
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:16p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 140  *****************
 * User: Arkkhasin    Date: 1/04/10    Time: 11:32a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Fixed a bug in getting stop adjustment
 *
 * *****************  Version 139  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:58a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Cleaned up DLT incarnation; uifunc.{c,h} is ready for factoring out
 * (pending MNCB)
 *
 * *****************  Version 138  *****************
 * User: Derek Li     Date: 5/15/09    Time: 4:27p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed #   include "coupling_dlt.h"
 *
 * *****************  Version 137  *****************
 * User: Derek Li     Date: 5/15/09    Time: 2:52p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed ui_GetCouplingValue
 *
 * *****************  Version 136  *****************
 * User: Derek Li     Date: 5/12/09    Time: 12:19p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Updated function ui_GetPosition to return s32 value
 *
 * *****************  Version 135  *****************
 * User: Arkkhasin    Date: 5/11/09    Time: 5:52p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint (bugs) workarounds
 *
 * *****************  Version 134  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:40p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Extracted more UI glue into separate supporting files
 *
 * *****************  Version 133  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 6:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * ui_GetSignal() now correctly uses level_GetIntendedOutput()
 *
 * *****************  Version 132  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 2:21p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Header refactoring (for DLT lint)
 * Removed fpconvert_SaveData() - units are saved instantaneously
 *
 * *****************  Version 131  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 12:56a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Repaired ui_getPosition()
 *
 * *****************  Version 130  *****************
 * User: Derek Li     Date: 4/09/09    Time: 1:45p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * ui_GetPosition function calls vpos_GetFilteredEUposition
 *
 * *****************  Version 129  *****************
 * User: Arkkhasin    Date: 4/08/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Repaired AP and ESD builds
 *
 * *****************  Version 128  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 7:28p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Temporary kludges for MNCB setup menus
 *
 * *****************  Version 127  *****************
 * User: Arkkhasin    Date: 4/03/09    Time: 7:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint
 *
 * *****************  Version 126  *****************
 * User: Arkkhasin    Date: 3/31/09    Time: 3:28p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Extracted MNCB-style find stops support
 *
 * *****************  Version 125  *****************
 * User: Arkkhasin    Date: 3/30/09    Time: 10:03p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Tracking the process requestor is now in process.c
 *
 * *****************  Version 124  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 11:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * A few small glue layer functions extracted from uifunc.{c,h}
 *
 * *****************  Version 123  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 1:11a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed unused control.h
 *
 * *****************  Version 122  *****************
 * User: Arkkhasin    Date: 3/28/09    Time: 4:08p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Pressures-related UI node functions moved to ui_pressures.{c,h}
 *
 * *****************  Version 121  *****************
 * User: Arkkhasin    Date: 3/27/09    Time: 3:16a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Light linting
 *
 * *****************  Version 120  *****************
 * User: Arkkhasin    Date: 3/26/09    Time: 5:52p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * A small fix for TUNE node
 *
 * *****************  Version 119  *****************
 * User: Arkkhasin    Date: 3/26/09    Time: 12:38a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Better generic API for process support
 *
 * *****************  Version 118  *****************
 * User: Derek Li     Date: 3/06/09    Time: 12:02p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Move calibration and sg glue layer function to ui_calib_sg_dlt.c
 *
 * *****************  Version 117  *****************
 * User: Derek Li     Date: 2/13/09    Time: 2:08p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Calibration SG Menu glue layer fuctions support
 *
 * *****************  Version 116  *****************
 * User: Arkkhasin    Date: 2/06/09    Time: 12:23a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint (io.h unused)
 *
 * *****************  Version 115  *****************
 * User: Arkkhasin    Date: 2/01/09    Time: 10:46p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Intermediate check-in for button locks with password override
 *
 * *****************  Version 114  *****************
 * User: Arkkhasin    Date: 1/30/09    Time: 3:11p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Engineering units extracted from uifunc.c into glue layer folder
 *
 * *****************  Version 113  *****************
 * User: Sergey Kruss Date: 1/29/09    Time: 4:28p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * SG Functionality: Replaced Cmd 179 with 150,151,36,37 combination
 *
 * *****************  Version 111  *****************
 * User: Arkkhasin    Date: 1/27/09    Time: 12:06a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Custom units set and save UI nodes are disabled if the current unit is
 * built-in
 *
 * *****************  Version 110  *****************
 * User: Derek Li     Date: 1/26/09    Time: 3:22p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Get coupling value with internal presentation
 *
 * *****************  Version 109  *****************
 * User: Arkkhasin    Date: 1/25/09    Time: 1:58a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Fixed missing decimal point in custom range values by inventing
 * UNITSID_CUSTOMRANGE.
 * Added a space between "Units" and units name
 *
 * *****************  Version 108  *****************
 * User: Arkkhasin    Date: 1/24/09    Time: 3:18p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Preliminary support for units configuration from local UI - glue layer
 *
 * *****************  Version 107  *****************
 * User: Derek Li     Date: 1/15/09    Time: 10:26a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * modify ui_GetCouplingValue to get the percent interal presentation
 *
 * *****************  Version 106  *****************
 * User: Derek Li     Date: 1/12/09    Time: 4:49p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * modify ui_GetCouplingValue in order to covert the coupling value as
 * internal integer representations for ui output as uid UNITSID_PERCENT
 *
 * *****************  Version 105  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 7:43p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * fixed ui_setManModeNaked.
 * Added ui_GetModeData()
 *
 * *****************  Version 104  *****************
 * User: Arkkhasin    Date: 1/04/09    Time: 10:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * ui_UnitsIncrement() is temporarily broken until a real implementation
 * of UI custom units comes about
 *
 * *****************  Version 103  *****************
 * User: Arkkhasin    Date: 12/26/08   Time: 2:12a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed obsolete inactive UI_LANGUGAGE_NAMED_IN_ALL_LANGUAGES
 *
 * *****************  Version 102  *****************
 * User: Derek Li     Date: 12/18/08   Time: 4:07p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * add FEATURE DLT PLATFORM condition to make other project build
 * sucessfully
 *
 * *****************  Version 101  *****************
 * User: Derek Li     Date: 12/18/08   Time: 1:10p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * add coupling function
 *
 * *****************  Version 100  *****************
 * User: Arkkhasin    Date: 12/04/08   Time: 1:16p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Position calibration is now driven by a state machine ignorant of
 * position/level implementation details
 *
 * *****************  Version 99  *****************
 * User: Arkkhasin    Date: 8/07/08    Time: 10:28p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Enabled computed signal output in local UI for DLT
*/
#include "projectdef.h"
#include "mnwrap.h"
//#include "mnassert.h"

//data definitions
#include "bitutils.h"
#include "errcodes.h" //for configure.h
#include "faultpublic.h"
#include "process.h"
#include "devicemode.h"
#include "uipublic.h"
#include "uidef.h"
#include "uigluehelpers.h"
#include "uiglue_common.h"



#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
#   include "levelxmit.h"
#endif //FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
#include "configure.h"
//#include "ui_config.h"
#include "ui_calib.h"
#endif //FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB

#include "ctllimits.h"
#include "position.h"
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
#include "ao.h"
#endif
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
#   include "uifunc.h"
#endif
#include "ui_config.h"

/** A node data function.
\return a pointer to the position value from an internal location
NOTE: RED_FLAG: the internal location is NOT chosen prudently: a node change
request can overwirite it. It may cause momentary garbage value on the LCD.
*/
static uivalvar32_t var_misc_s32;
uivalvar32_t *ui_GetPosition(void)
{
    //var_misc_s32 = (pos_t)pos_GetPresentationPosition();//for engineering unit position
    var_misc_s32 = (uivalvar32_t)pos_GetPresentationPosition();//for engineering unit position
    return &var_misc_s32;
}

#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
static u8 cost_option; //! scratchpad location for dash- version
/** A "node enable" data function.
\return a pointer to the cost (dash-) version from an internal location
*/
const void *ui_getCostOption(void)
{
    cost_option = (cnfg_GetOptionConfigDashVersion()==LOW_COST_VERSION)?LOW_COST_VERSION:0U;
    return &cost_option;
}
#endif //FEATURE_HARDWARE_PLATFORM_MNCB

//--------------- long process support ---------------------------------
/** \brief A helper function to launch a process from pushbuttons
\param proc_id - the Id of a process to launch
\return true if OK, false if cannot launch
*/
bool_t ui_process(ProcId_t proc_id)
{
    bool_t ret = false;  //exit to whatever node to report busy
    //Check that a process is running...
    if(!process_CheckProcess())
    {
        /* If we set the process id successfully, return true to stay in the current node
           and let the process take over;
           otherwise, return false to go to failure node
        */

        if(process_SetProcessCommand(proc_id) == ERR_OK)
        {
            process_SetRequestorFlag(1);
            ret = true;
        }
    }
    return ret;
}


//=============== open stop adjustment ==============================
#if FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
static pos_t StopAdjustment[Xends]; //[Xlow]=ClosedStop, [Xhi]=OpenStop

/** A node data function.
\return a pointer to cached stop adjustment
*/
uivalvar_t *ui_getOpenStopAdjustment_var(void)
{
    return (uivalvar_t *)StopAdjustment;
}


static bool_t ui_getOpenStopAdjustment(void)
{
    (void)pos_GetStopAdjustment(StopAdjustment);
    return false;
}
/** A node function
    \brief saves cal data set in the menus (not in long processes)
    \return false on success (=failure to save, failure node enabled), true otherwise
*/
static bool_t ui_setOpenStopAdjustment(void)
{
    return (pos_SetStopAdjustment(StopAdjustment[Xlow], StopAdjustment[Xhi]) == ERR_OK);
}
#endif //FEATURE_UI_OPSTOP_ADJ

//A temporary kludge until MNCB UI redesign/refactoring
#if FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
//----------- get Calib menu data ----------
typedef struct calgetset_t
{
    const nodefunc_t *get;
    const nodefunc_t *set;
} calgetset_t;
static const calgetset_t ui_CalibMenuTable[] =
{
#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#if FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
    {ui_getHiLoSignalData,ui_setHiLoSignalData},
#endif //FEATURE_UI_SIGNAL_LIMITS
#endif //FEATURE_SIGNAL_SETPOINT
#if FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
    {ui_getOpenStopAdjustment,ui_setOpenStopAdjustment},
#endif //FEATURE_UI_OPSTOP_ADJ
    {ui_GetSysAirAction, ui_SaveSysAirAction}, // ATO/ATC
    {ui_GetSysSpCharact, ui_SaveSysSpCharact},
    {NULL,               ui_SaveSysLanguage},  // language
    {ui_GetSysCtlLimits, ui_SaveSysCtlLimits}, // tight shut off hi/low
#if FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    {ui_GetSysPidParamSet, ui_SaveSysPidParamSet}, // manual tune
#endif
    {NULL,NULL} // pressure nuit is saved by node itself
};

bool_t ui_getCalibMenuData(const uistate_t *state)
{
    UNUSED_OK(state);
    s8_least i;
    //below, 2 is (1+1), 1 for last index, 1 for last empty terminator
    for(i=(s8_least)NELEM(ui_CalibMenuTable)-2; i>=0; i--) //lint !e681 [MISRA Rule 14.1] Loop is not entered if empty table:OK
    {
        if (NULL != ui_CalibMenuTable[i].get)
        {
            (void)(ui_CalibMenuTable[i].get)();
        }
    }
    return false;
}

/** \brief saves all calib menu data and enables the failure node on failure.
\return false iff failed (by common convention)
*/
bool_t ui_setCalibMenuData(const uistate_t *state)
{
    UNUSED_OK(state);
    s8_least i;
    bool_t ret = true;

    //below, 2 is (1+1), 1 for last index, 1 for last empty terminator
    for(i=(s8_least)NELEM(ui_CalibMenuTable)-2; i>=0; i--) //lint !e681 [MISRA Rule 14.1] Loop is not entered if empty table:OK
    {
        if (NULL != ui_CalibMenuTable[i].set)
        {
            ret = ui_CalibMenuTable[i].set();
            if(!ret)
            {
                break;
            }
        }
    }
    return ret;
}
#endif //FEATURE_HARDWARE_PLATFORM_MNCB

/* This line marks the end of the source */
