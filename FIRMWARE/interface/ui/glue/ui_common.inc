#MP If 0
/*
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_common.inc
    \brief Macros for UI common nodes

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_common.inc $
    $Date: 1/19/12 12:49p $
    $Revision: 2 $
    $Author: Arkkhasin $
*/
/* $History: ui_common.inc $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/19/12    Time: 12:49p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Adapted navigation graph to old device mode FBO TFS:8577
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:47a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Commonly used UI glue hookups extracted here
*/
#MP Endif

#MP Macro var_misc ;A definition of a required variable
    #MP Expand StartNode(nodetype_var, #@var_misc#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODELINK_NONE,
        .data_index = 0,
        .varfunc = VARFUNC(ui_getMiscVar)
    },
    #MP Expand EndNode()
#MP Endm

#MP ;This macro is mandatory (even though its implementation is not
#MP Macro banner ;what we brag about on reset
    #MP Expand DeclareStartNode(#@UINODEID_BANNERNODE#)
    #MP Expand StartNode(nodetype_nav, #@banner#)
    {
        #MP Expand NodetType()
        1, //smallest timeout - should move as soon as init is complete
        #MP TEXTLABEL(%sPROJ_PREFIX%n_BANNER)
        .en = UINODE_ENABLED, //1st node must be enabled
        #MP over_the_var = UIGraphNode+2
        .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
        .select = NODE_LINK(#mp%sMainMenuFirstNode),
        //.advance = NODE_LINK(#mp%sUIGraphNodeNext),
        //.select = NODE_LINK(#mp%sUIGraphNodeNext),
        //.select = NODE_LINK(banner_version),
        .xfuncx = XLATE_INDEX(xlate_NextIfEnabled),
        .uid = UNITSID_DUMMY
    },
    #MP Expand EndNode()
#MP If 1
    #MP Expand StartNode(nodetype_var, "banner_version")
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext), //fall through
        .data_index = 0,
        .varfunc = VARFUNC((varfunc_t*)ui_GetMNCBVerNumber)
    },
    #MP Expand EndNode()
#MP Endif
#MP Endm

#MP ;a companion macro to view version in "view data" menu
#MP Macro view_Version   ;(return node on "*" as name)
    #MP Expand StartNode(nodetype_alt, "view_Version")
    {
        {
            #MP Expand NodetType()
            0, //refresh
            //#MP TEXTLABEL(%sPROJ_PREFIX%n_BANNER)
                        #MP TEXTLABEL(FIRM_REV)
            UINODE_ENABLED,
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(#mp%n#1#), //on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_DUMMY,
        },
        NULL,
        NODE_LINK(banner_version)
    },
    #MP Expand EndNode()
#MP Endm

#MP;/* This line marks the end of the source */
