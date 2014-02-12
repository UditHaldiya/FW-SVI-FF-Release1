#MP If 0
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uilocks.inc
    \brief Macros for access lock management in MNCB

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    Pre-requisites: uinodes.inc included (it has all the generating macro definitions)

    This file illustrates how to structure a pluggable submenu with macros
    to reduce clutter in the top-level UI graph definition:

    A submenu is defined with two macros:
    - A macro defining a "root" node to plug in the upper-level menu, and
    - A macro (may be quite large) defining the whole submenu to which
        the root node descends.
    If a submenu and/or its root node may reasonably be reused and appear in
    different places in the UI graph, nodes definitions must avoid collisions
    of node names. A way to achieve this is to make node names based on a root
    node name passed as a parameter to the macros and/or to generate node names
    from location-unique artifacts. Both techniques are used in the two macros
    here.
    Of course, the menus so (or in any other way) defined provide both UI graph navigation
    functionality and user functionality only in concert with the necessary
    glue-layer functions. [If you need only to navigate, the UI graph handles
    it alone; if you need to manipulate data, glue functions are necessary. In
    some cases, there may already exist module API functions that might be reused
    directly without the need for a glue.]

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_locks.inc $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    IMPORTANT:
    Root node must be enabled so that it may be returned to (via a + link)
    ib case of unlock failure.

    IMPORTANT:
    Pay close attention to defining the "node enabled" field. It must be
    of the form UINODE_ENABLED, UINODE_DISABLED or ENOFFS(en) where en is
    a valid element of the Enable structure defined elsewhere.
*/
/* $History: ui_locks.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 11/17/09   Time: 10:46p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * A step in modularization of "enable" table entries and C headers:
 * macoized and moved to places where the graph nodes themselves are
 * defined.
 *
 * *****************  Version 5  *****************
 * User: Derek Li     Date: 3/23/09    Time: 4:11p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Uncomment LOCK_ADVSETUP
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/17/09    Time: 12:28a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Preliminary check-in on the way to support 32-bit UI variables
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/13/09    Time: 11:52a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * LOCK_ADVSETUP "enable" entry disabled until when it's needed
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/04/09    Time: 7:22p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * First working implementation of UI pushbutton locks with password
 * override - "CLR ERR" only
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/09    Time: 10:27p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Intermediate check-in for button locks with password override
*/
#MP Endif
#MP ;Naming Conventions
#MP Setstr LockCheckSuffix "_Check"
#MP Setstr TargetNodeSuffix "_Do"
#MP ;IOW, a node called foo must select node foo_Check to do the lock checking
#MP ;On failure, foo is arrived at with +-path (i.e. remains current if enabled).
#MP ;On success, foo_Do is arrived at with *-path (becomes current if enabled)
#MP CHECK_WRITE_PROTECTED = 1
#MP
#MP Macro ProtectedEntry ;[Root-node-name, CHECK_WRITE_PROTECTED-or-0, lock-level-name]
#MP Save uinode_autoname
#MP Save uinode_success
#MP Save uinode_failure
#MP Setstr uinode_success = {#1#} TargetNodeSuffix
#MP Setstr uinode_failure = "locked_" {%dUIGraphNode}
#MP Setstr uinode_autoname = {#1#} LockCheckSuffix
#MP If #2# == CHECK_WRITE_PROTECTED
    #MP StartNode(nodetype_xsel, uinode_autoname)
    {
        { //A proxy node to check write protect
            #MP NodetType
            .tmout = TMOUT_TICKS_DEFAULT, //enable timeout default
            #MP TEXTLABEL(PROTECTED)
            .en = UINODE_ENABLED,
            .advance = NODE_LINK(#mp%n#1#), //Advance to root node (locked)
            .select = NODE_LINK(#mp%sUIGraphNodeNext), //fallthrough if disabled
            .xfuncx = XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_isWriteProtected, NULL
    },
    #MP EndNode
    #MP Setstr uinode_autoname = "test_lock_" {%dUIGraphNode} ;make a unique node name
#MP Endif ;CHECK_WRITE_PROTECTED
    #MP StartNode(nodetype_nav, uinode_autoname)
    { //A node to prompt for password: enabled if locked and password enabled.
        #MP NodetType
        .tmout = TMOUT_TICKS_DEFAULT,
        #MP TEXTLABEL(PASSWORD) ;//request password
        .en = ENOFFS(#mp%n#3#),
        .advance = NODE_LINK(#mp%suinode_autoname), //stay here on + or -
        .select = NODE_LINK(#mp%sUIGraphNodeNext), //fallthrough to entering the password
        .xfuncx = XLATE_INDEX(xlate_SelectIfEnabled),
        .uid = UNITSID_DUMMY, //password displayed without scaling
    },
    #MP EndNode
    #MP Setstr uinode_autoname = "test_lock_" {%dUIGraphNode} ;make a unique node name
    #MP StartNode(nodetype_nav, uinode_autoname)
    { //A buffer node to prevent displaying password in the previous node
        #MP NodetType
        .tmout = 0, //don't care
        #MP TEXTLABEL(PASSWORD) ;//don't care
        .en = UINODE_DISABLED,
        .advance = NODE_LINK(#mp%sUIGraphNodeNext), //don't care
        .select = NODE_LINK(#mp%sUIGraphNodeNext), //fallthrough to entering the password
        .xfuncx = XLATE_INDEX(xlate_SelectIfEnabled), //don't care
        .uid = UNITSID_DUMMY, //password displayed without scaling
    },
    #MP EndNode
        #MP Setstr uinode_autoname = "test_lock_" {%dUIGraphNode} ;make a unique node name
        #MP StartNode(nodetype_limvar, uinode_autoname)
        {
            {
                #MP NodetType
                .checksum_offset = 0, //no checksum
                .en = ENOFFS(#mp%n#3#),
                .data_index = 0, //location offset
                .parent = NODE_LINK(#mp%sUIGraphNodeNext),
                .varfunc = VARFUNC(ui_GetCurrPassword), //password is there
            },
            UI_VARLIM(&ui_password_rv[Xlow]),
            UI_VARLIM(&ui_password_rv[Xhi])
        },
        #MP EndNode
    #MP StartNode(nodetype_nav, uinode_failure)
    { //A node to display failure to unlock (locked and wrong or no password)
        #MP NodetType
        .tmout = TMOUT_TICKS_DEFAULT, //enable timeout default
        #MP TEXTLABEL(BUTTONS_LOCKED)
        .en = ENOFFS(UIPasswordMismatch), //enable failure only if password exists
        .advance = NODE_LINK(#mp%n#1#), //Return to root node on failure (node enabled)
        .select = NODE_LINK(#mp%suinode_success), //go to success node if disabled
        .xfuncx = XLATE_INDEX(xlate_NextIfEnabled),
        .uid = UNITSID_NONE,
    },
    #MP EndNode


#MP Restore uinode_autoname
#MP Restore uinode_success
#MP Restore uinode_failure
#MP Endm

#MP If 0; Comment
A node lock level locks an area if it is >= effective (configured) UI lock level.
A handy way to test for it is to observe that
(1<<(UILockLevel+1))-1
has bits set for all affected lock levels and only for them.
So for a lock check node to be enabled, its 1<<NodeLockLevel mask carves out a set bit.
The price for it is that there must be a DATAFUNC returning the effective lock bitmap.
#MP Endif; Comment
#MP Macro Enable_UIButtonLockTest
    #MP StartEnEntry(#@LOCK_MANUAL#)
        DATAFUNC(ui_GetLocalUILocks), 0, 1U<<BUTTON_LOCK_MANUAL, 1U<<BUTTON_LOCK_MANUAL,
        #MP EnEntryNext["UIPassword"]
    #MP EndEnEntry
    #MP StartEnEntry(#@LOCK_SETUP#)
        DATAFUNC(ui_GetLocalUILocks), 0, 1U<<BUTTON_LOCK_SETUP, 1U<<BUTTON_LOCK_SETUP,
        #MP EnEntryNext["UIPassword"]
    #MP EndEnEntry

    #MP condex(UIAdvSetup_En)

    #MP StartEnEntry("LOCK_ANYWRITE")
        DATAFUNC(ui_GetLocalUILocks), 0, 1U<<BUTTON_LOCK_ANYWRITE, 1U<<BUTTON_LOCK_ANYWRITE,
        #MP EnEntryNext["UIPassword"]
    #MP EndEnEntry
    #MP StartEnEntry("UIPassword")
        DATAFUNC(ui_GetLocalUIDataPtr), MN_OFFSETOF(UIData_t, PasswordEn), 1, 1, //enabled if a password exists
        #MP EnEntryNext
    #MP EndEnEntry
    #MP StartEnEntry("UIPasswordMismatch")
        DATAFUNC(ui_isPasswordMatch), 0, 1, 0, //enabled if locked and a password doesn't exist or wrong
        #MP EnEntryNext
    #MP EndEnEntry
#MP Endm
