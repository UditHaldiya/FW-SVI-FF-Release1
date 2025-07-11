#MP If 0
/**
Copyright 2008 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_finstops_noctl.inc
    \brief Configuration of LCD UI nodes for MNCB : "Find stops" with active control

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    Pre-requisites: uinodes.inc included (it has all the macro definitions)

    CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $

    IMPORTANT:
    Pay close attention to defining the "node enabled" field. It must be
    of the form UINODE_ENABLED, UINODE_DISABLED or ENOFFS(en) where en is
    a valid element of the Enable structure defined elsewhere.
*/
/* (Optional) $History: ui_findstops.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 7:17p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Unlatch node made conditional
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/06/09    Time: 12:29a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Macro-ized Setup mode submenus, extracted in separate .inc files, with
 * new lock scheme
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/24/08    Time: 3:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Passive and active "find stops" support extracted from uinodedef.inc
*/
#MP Endif

#MP Macro CalStops ;Root node - name supplied in uinode_autoname
#MP Setstr CalStops_autoname = uinode_autoname
    #MP Expand StartNode(nodetype_nav, uinode_autoname) ;cal stops
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(STOPS)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(cal_stops_submenu), //go to submenu on *
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm

#MP Macro CalStopsMenu
    #MP Expand StartNode(nodetype_xsel, "cal_stops_submenu")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(AUTO_STOPS)
            UINODE_ENABLED,
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(cal_stops_submenu), //stay here on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_process_FindStops
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "cal_stops_manual")
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(MAN_STOPS)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(cal_stops_manual_low), //on *
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "cal_stops_goback")
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(GOBACK)
        UINODE_ENABLED,
        NODE_LINK(cal_stops_submenu), //wrap
        NODE_LINK(#mp%sCalStops_autoname), //on *
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
#MP         ;----------------- Manual Find Stops support -------
    #MP Expand StartNode(nodetype_xsel, "cal_stops_manual_low")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(STOP_LOW)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual_goback),
            NODE_LINK(cal_stops_manual_low_accept),
            XLATE_INDEX(xlate_StdWiz),
            .uid = UNITSID_NONE,
        },
        NULL, ui_IPout_low
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_alt, "cal_stops_manual_low_accept")
    {
        {
            #MP Expand NodetType()
            -1, //refresh every time
            #MP TEXTLABEL(ACCEPT_LOW)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual_goback),
            NODE_LINK(cal_stops_manual_low_failed), //on *
            XLATE_INDEX(xlate_StdWiz),
            .uid = UNITSID_POSITION,
        },
        NULL,
        NODE_LINK(var_start_pos)
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@cal_stops_manual_low_failed#) ;pass through if OK
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual), //go up regardless but display FAILURE on data save failure
            NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_SetLowStop, NULL // ui_SetLowStop() must return failure flag if fails
    },
    #MP Expand EndNode()
    #MP condex(UnLatchAtStop)
    #MP Expand StartNode(nodetype_xsel, "cal_stops_manual_high")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(STOP_HIGH)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual_goback),
            NODE_LINK(cal_stops_manual_high_accept),
            XLATE_INDEX(xlate_StdWiz),
            .uid = UNITSID_NONE,
        },
        NULL, ui_IPout_high
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_alt, "cal_stops_manual_high_accept")
    {
        {
            #MP Expand NodetType()
            -1, //refresh every time
            #MP TEXTLABEL(ACCEPT_HIGH)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual_goback),
            NODE_LINK(cal_stops_manual_high_failed), //on *
            XLATE_INDEX(xlate_StdWiz),
            .uid = UNITSID_POSITION,
        },
        NULL,
        NODE_LINK(var_start_pos)
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@cal_stops_manual_high_failed#) ;pass through if OK
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual), //go up regardless but display FAILURE on data save failure
            NODE_LINK(cal_stops_manual_commit),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_SetHighStop, NULL // ui_SetHighStop() must return failure flag if fails
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "cal_stops_manual_commit")
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(COMMIT)
        UINODE_ENABLED,
        NODE_LINK(cal_stops_manual_goback),
        NODE_LINK(cal_stops_manual_commit_failed),
        XLATE_INDEX(xlate_StdWiz),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@cal_stops_manual_commit_failed#) ;pass through if OK
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            UINODE_ENABLED,
            NODE_LINK(cal_stops_manual), //go up regardless but display FAILURE on data save failure
            NODE_LINK(cal_stops_manual),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_CommitStops, NULL // ui_SetHighStop() must return failure flag if fails
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "cal_stops_manual_goback")
    { //Just display
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(GOBACK)
        UINODE_ENABLED,
        NODE_LINK(cal_stops_manual_low), //wrap around
        NODE_LINK(cal_stops_manual), //up
        XLATE_INDEX(xlate_StdWiz),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
