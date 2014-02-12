/*
Copyright 2004-2010 by Dresser, Inc., an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file uistartnodes.h
    \brief Definitions of start UI node ids (valid arguments to ui_setNode())

    For LCD-based UI the counterpart of this file is
    Generated automatically by uistartnodes.u - Because it must correspond
    to navigation graph.
    For simple UI this is a collection of (arbitrary) tags which are still
    project-specific

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX/uistartnodes.h $
    $Date: 5/26/10 3:05p $
    $Revision: 3 $
    $Author: Anatoly Podpaly $

    \ingroup mnhartcmd
*/
/* $History: uistartnodes.h $
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 3:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Added requirement #
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 3:04p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Bug 3279 - added callback ids (Node IDs) to pass the info about
 * internal state of the process to the LUI.
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/15/10    Time: 12:29a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Not generated automatically for "simple" UI
*/

/** REQ 31-34 */

#define UINODEID_INITNODE 0
#define UINODEID_SETUPNODE 1
#define UINODEID_INITFSAFE 2
#define HART_OVERRIDE_INFO_NODE 3
#define UINODEID_CALSTOPS 4


#define UINODEID_STOPS_RUNNING          5
#define UINODEID_STOPS_RUNNING_LOW      6
#define UINODEID_STOPS_RUNNING_HIGH     7
#define UINODEID_STOPS_RUNNING_WARNING  8
#define UINODEID_RUNSTOPS_FAILED        9

#define UINODEID_TUNE1                  10
#define UINODEID_TUNE2                  11
#define UINODEID_TUNE3                  12
#define UINODEID_TUNE_OK                13
#define UINODEID_TUNE_FAILED            14
#define UINODEID_TUNE_PROTECTED         15
#define UINODEID_DIAG_EX                16
#define UINODEID_DIAG_STD               17
#define UINODEID_TUNE_WARNING           18

#define UINODEID_OPENSTOPADJ_OK         19
#define UINODEID_OPENSTOPADJ_FAILED     20
#define UINODEID_OPENSTOPADJ_WARNING    21
#define UINODEID_OPENSTOPADJ_NORMAL     22


#define UINODEID_BUMPLESS_XFER          23
#define UINODEID_BUMPLESS_XFER_FAILED   24
#define UINODEID_LOCKNODE               25
#define UINODEID_LOCKSETUP              26

/* This line marks the end of the source */
