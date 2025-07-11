#MP If 0
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uicustomunits.inc
    \brief Macros for configuration of engineering units LCD UI nodes for MNCB

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

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_engunits.inc $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

    IMPORTANT:
    Pay close attention to defining the "node enabled" field. It must be
    of the form UINODE_ENABLED, UINODE_DISABLED or ENOFFS(en) where en is
    a valid element of the Enable structure defined elsewhere.
*/
/* $History: ui_engunits.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 2/11/10    Time: 1:54a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Light linting
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 11/17/09   Time: 10:46p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * A step in modularization of "enable" table entries and C headers:
 * macoized and moved to places where the graph nodes themselves are
 * defined.
 *
 * *****************  Version 8  *****************
 * User: Derek Li     Date: 6/25/09    Time: 1:13p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Modified "BACK" label to "CANCEL"
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 4/29/09    Time: 11:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Made custom units 32-bit with the internal range +-99999
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 3/19/09    Time: 10:38p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/27/09    Time: 6:40p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Wrong URV max value pointer fixed
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 1/27/09    Time: 12:04a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Custom units set and save UI nodes are disabled if the current unit is
 * built-in
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/26/09    Time: 1:38p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Fixed the toggling of units
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/25/09    Time: 1:58a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Fixed missing decimal point in custom range values by inventing
 * UNITSID_CUSTOMRANGE.
 * Added a space between "Units" and units name
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/24/09    Time: 2:57p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * First shot at custom units configuration using UI:
 * Pluggable UI graph nodes and corresponding glue functions
*/
#MP Endif
#MP
#MP Macro CustomUnitsMenuRoot ;[Root-node-name, units-id]
    #MP StartNode(nodetype_xsel, {#1#})
    {
        {
            #MP NodetType
            .tmout = 0,
            #MP TEXTLABEL(LEVEL_UNITS_SETUP)
            .en = UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext), //next
            .select = NODE_LINK(__begin_submenu__#mp%n#1#), //descend to submenu
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = #mp%n#2#, //Common custom units code will now what units will be configured
        },
        NULL, ui_GetCustomUnits
    },
    #MP EndNode
#MP Endm

#MP Macro CustomUnitsMenu ;[Root-node-name, units-id]
    //Begin custom units menu for root node #mp%n#1#
    #MP Save MenuStartNode
    #MP Setstr MenuStartNode = "__begin_submenu__" {#1#}
    #MP StartNode(nodetype_xsel, MenuStartNode)
    {
        {
            #MP NodetType
            .tmout = 0,
            #MP TEXTLABEL(UNITS_SELECT)
            .en = UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext), //next
            .select = NODE_LINK(#mp%sMenuStartNode), //toggle by wrapping to itself
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = #mp%n#2#,
        },
        NULL, ui_UnitsIncrement
    },
    #MP EndNode
    #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
    #MP StartNode(nodetype_nav, uinode_autoname)
    {
        #MP NodetType
        .tmout = 0, //no timeout
        #MP TEXTLABEL(DECIMALS)
        .en = ENOFFS(CUSTOMUNITS),
        #MP over_the_var = UIGraphNode+2
        .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
        .select = NODE_LINK(#mp%sUIGraphNodeNext),
        .xfuncx = XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_DUMMY, //display literally
    },
    #MP EndNode
        #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
        #MP StartNode(nodetype_limvar, uinode_autoname)
        {
            {
                #MP NodetType
                .checksum_offset = 0, //no checksum
                .en = UINODE_ENABLED,
                .data_index = 0, //location offset
                .parent = NODELINK_NONE,
                .varfunc = VARFUNC(ui_getMiscVar),
            },
            UI_VARLIM(&custunits_decimals[0]),
            UI_VARLIM(&custunits_decimals[1])
        },
        #MP EndNode
    #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
    #MP StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP NodetType
            .tmout = 0, //no timeout
            #MP TEXTLABEL(LRV)
            .en = ENOFFS(CUSTOMUNITS),
            #MP over_the_var = UIGraphNode+2
            .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_CUSTOMRANGE,
        },
        ui_FixLRVrange, NULL,
    },
    #MP EndNode
        #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
        #MP StartNode(nodetype_limvar, uinode_autoname)
        {
            {
                #MP NodetType("UINODE_V32_FLAG")
                .checksum_offset = 0, //no checksum
                .en = UINODE_ENABLED,
                .data_index = MN_OFFSETOF(Cun_t, rv[Xlow])/sizeof(uivalvar_t), //location offset
                .parent = NODELINK_NONE,
                .varfunc = VARFUNC32(ui_GetCurrentCun),
            },
            UI_VARLIM32(&custunits_rv[0]),
            UI_VARLIM32(&custunits_middle)
        },
        #MP EndNode
    #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
    #MP StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP NodetType
            .tmout = 0, //no timeout
            #MP TEXTLABEL(URV)
            .en = ENOFFS(CUSTOMUNITS),
            #MP over_the_var = UIGraphNode+2
            .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_CUSTOMRANGE,
        },
        ui_FixURVrange, NULL,
    },
    #MP EndNode
        #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
        #MP StartNode(nodetype_limvar, uinode_autoname)
        {
            {
                #MP NodetType("UINODE_V32_FLAG")
                .checksum_offset = 0, //no checksum
                .en = UINODE_ENABLED,
                .data_index = MN_OFFSETOF(Cun_t, rv[Xhi])/sizeof(uivalvar32_t), //location offset
                .parent = NODELINK_NONE,
                .varfunc = VARFUNC32(ui_GetCurrentCun),
            },
            UI_VARLIM32(&custunits_middle),
            UI_VARLIM32(&custunits_rv[1])
        },
        #MP EndNode
    #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
    #MP StartNode(nodetype_nav, uinode_autoname)
    {
        #MP NodetType
        .tmout = 0,
        #MP TEXTLABEL(SAVE)
        .en = ENOFFS(CUSTOMUNITS),
        #MP over_the_failed = UIGraphNode+2
        .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_failed),
        .select = NODE_LINK(#mp%sUIGraphNodeNext),
        .xfuncx = XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP EndNode
    #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
    #MP StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP NodetType
            .tmout = TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            .en = UINODE_ENABLED,
            #MP before_the_failed = UIGraphNode-1
            .advance = NODE_LINK(#mp%sUIGraphNode_%dbefore_the_failed), //on failure
            .select = NODE_LINK(#mp%n#1#), //up on success
            .xfuncx = XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_UnitsSave, NULL
    },
    #MP EndNode
    #MP Setstr uinode_autoname = "CustomUnitsMenu" {%dUIGraphNode}
    #MP StartNode(nodetype_nav, uinode_autoname)
    {
        #MP NodetType
        .tmout = 0,
        #MP TEXTLABEL(CANCEL)
        .en = UINODE_ENABLED,
        .advance = NODE_LINK(#mp%sMenuStartNode), //wraparound
        .select = NODE_LINK(#mp%n#1#), //up
        .xfuncx = XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP EndNode
    #MP Restore MenuStartNode
    //End custom units menu for root node #mp%n#1#
#MP Endm

#MP If FEATURE_CUSTOM_UNITS == FEATURE_CUSTOM_UNITS_SUPPORTED
#MP Macro EngUnits_En ;checks if enabled
    #MP StartEnEntry("CUSTOMUNITS")
        DATAFUNC(ui_CheckCustomUnitsSelected), 0, 1, 1,
        #MP Expand EnEntryNext()
    #MP EndEnEntry()
#MP Endm
#MP Endif ;FEATURE_CUSTOM_UNITS
#MP ;/* This line marks the end of the source */
