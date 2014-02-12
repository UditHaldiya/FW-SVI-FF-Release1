/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uistubs.c
    \brief Stubs for a project without UI

    CPU: Any (with standard C compiler)

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uistubs.c $
    $Date: 12/02/10 7:08p $
    $Revision: 1 $
    $Author: Arkkhasin $

    $History: uistubs.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/02/10   Time: 7:08p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface
 * TFS:4933 - building a blind AP
*/
#include "config.h"
#include "mnwrap.h"
#include "mncbtypes.h"
#include "sysio.h"

#include "uipublic.h"
#include "uiinput.h"

#ifndef NDEBUG
//NOTE: Should be in ui_io.c but this is a dummy
/** \brief Stays in a tight loop until a key is pressed
\param message - ignored
\param line - ignored
*/
void sysio_ShowCrash(const void *message, s32 line)
{
    UNUSED_OK(message);
    UNUSED_OK(line);
}
#endif

/** \brief "Callback" function from application to UI engine
  \parameter[in] node_id - info from the app to UI
*/
void ui_setNext(s16_least node_id)
{
    UNUSED_OK(node_id);
}

/** \brief "Callback" function from application to UI engine
  \parameter[in] node_id - info from the app to UI (passed to ui_setNext)
  \parameter[in] value - a number to show along with the node
*/

void ui_setNextExt(s16_least node_id, s16 value)
{
    UNUSED_OK(value);
    ui_setNext(node_id);
}

/** \brief Initializer of the MNCB LCD UI

  Initializes the UI state and finds the initial UI node to set as current.
  (And displays it, too.)
*/
void ui_init(void)
{
    // Handle  UI Main Data Structure
}

/** \brief Pushbutton handler of the MNCB  UI

  Handles pushbuttons and, internally, node timeout.
  Displays the current node if there is anything new.
*/
u32 ui_main()
{
    return 0U;
}
/* This line marks the end of the source */
