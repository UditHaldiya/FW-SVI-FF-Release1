#MP If 0
/**
Copyright 2012 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_ffinfo.inc
    \brief Macros for FF informationin MNCB

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: Ej

    $Archive: interface/ui/glue/ui_ffinfo.inc $
    $Date: 19/06/12 3:15p $
    $Revision: 1 $
    $Author: EricJ $
*/
/* $History: ui_ffinfo.inc $

 * *****************  Version 1  *****************
 * User: EricJ    Date: 19/06/12    Time: 12:32a
 * Created in FIRMWARE/interface/ui
*/
#MP Endif
#MP
#MP;//-------- ff_Info menu --------------------------------------------------
#MP
#MP;    // dummy node
#MP If 0 ;Comment
In the current UI implementation, there is a problem with descending into a
submenu with a SELECT action: if the first node is disabled, the UI engine
passes through it to whatever follows.
A workaround is to have an artificial node with timeout 1 which would be
always enabled and transition to the ADVANCE node right away.
#MP Endif ;Comment
#MP
#MP Macro FF_Blk_Info_Tags ;(blockname, block count of tag)
#MP ;--------------------- start of resource block -------------------
#MP Save uinode_autoname
    #MP Setstr uinode_autoname = "proxy_Info_"{%s#1#}
    #MP Expand StartNode(nodetype_nav, uinode_autoname)
    {
        #MP Expand NodetType()
        1, //smallest timeout
        #MP Setstr uinode_autoname = {%s#1#}"_INFO"
        #MP TEXTLABEL(%suinode_autoname)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext), //; advance
        #MP Setstr uinode_autoname = {%s#1#}"_info"
        NODE_LINK(#mp%suinode_autoname), //; select
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Setstr uinode_autoname = {%s#1#}"_info"
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP Setstr uinode_autoname = {%s#1#}"_INFO"
            #MP TEXTLABEL(%suinode_autoname)
            UINODE_ENABLED,
            #MP Setstr uinode_autoname = {%s#1#}"_Tag"
            .advance = NODE_LINK(#mp%suinode_autoname),
            #MP Setstr uinode_autoname = {%s#1#}"_info"
            .select = NODE_LINK(#mp%suinode_autoname),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        #MP Setstr uinode_autoname = "ui_ff_Info"{%s#1#}
        #mp%suinode_autoname, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Setstr uinode_autoname = {%s#1#}"_Tag"
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP Setstr uinode_autoname = {%s#1#}"_TAG"
            #MP TEXTLABEL(%suinode_autoname)
            UINODE_ENABLED,
            #MP Setstr uinode_autoname = {%s#1#}"_goback"
            .advance = NODE_LINK(#mp%suinode_autoname),
            #MP Setstr uinode_autoname = {%s#1#}"_Tag1"
            .select = NODE_LINK(#mp%suinode_autoname),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3, NULL
    },
    #MP Expand EndNode()
#MP
#MP For i=1, #2#
    #MP Setstr uinode_autoname = {%s#1#}"_Tag"
    #MP Expand StartNode(nodetype_xsel, uinode_autoname, i)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP Setstr uinode_autoname = {%s#1#}"_TAG"{%ui}
            #MP TEXTLABEL(%suinode_autoname)
            UINODE_ENABLED,
            #MP nextnode = i + 1
            #MP If nextnode > #2#
            #MP     nextnode = 1
            #MP Endif
            #MP Setstr uinode_autoname = {%s#1#}"_Tag"{%unextnode}
            .advance = NODE_LINK(#mp%suinode_autoname),
            #MP Setstr uinode_autoname = {%s#1#}"_Tag"
            .select = NODE_LINK(#mp%suinode_autoname),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        #MP Setstr uinode_autoname = "ui_ff_Tag"{%ui}{%s#1#}
        #mp%suinode_autoname, NULL
    },
    #MP Expand EndNode()
#MP Endfor
#MP
    #MP Setstr uinode_autoname = {%s#1#}"_goback"
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(GOBACK)
            UINODE_ENABLED,
            #MP Setstr uinode_autoname = {%s#1#}"_info"
            NODE_LINK(#mp%suinode_autoname),//wrap around
            #MP Setstr uinode_autoname = "ffInfo_"{%s#1#}
            NODE_LINK(#mp%suinode_autoname),//do it on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3, ui_Clr_Lin3
    },
    #MP Expand EndNode()
#MP ;------------ end of resource block --------------------------
#MP     Restore uinode_autoname
#MP Endm ; FF block info -------
#MP Macro subMenu_ffInfo
    #MP Expand StartNode(nodetype_nav, #@proxy_ffInfo#)
    {
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(FF_INFO)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(ff_Info),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
     #MP Expand StartNode(nodetype_nav, #@ffInfo_DevInfo#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_DEVINFO)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_devInfo),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, #@ffInfo_RB#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_RB)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_RB),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, #@ffInfo_TB#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_TB)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_TB),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, #@ffInfo_AO#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_AO)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_AO),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, #@ffInfo_PID#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_PID)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_PID),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, #@ffInfo_PID2#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_PID2)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_PID2),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, #@ffInfo_DO#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_DO)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_DO),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@ffInfo_DO2#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(FFINFO_DO2)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(proxy_Info_DO2),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_nav, "ffInfo_goback")
    { //Just display
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(GOBACK)
        UINODE_ENABLED,
        NODE_LINK(ffInfo_DevInfo), //wrap around
        NODE_LINK(ff_Info), //do it on *
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
#MP ;--------------------- start of Dev Info -------------------
    #MP Expand StartNode(nodetype_nav, #@proxy_devInfo#)
    {
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(FFINFO_DEVINFO)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_Addr#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_ADDR)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_Addr),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devAddr, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_ID#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_ID)
            UINODE_ENABLED,
            .advance = NODE_LINK(dev_Tag),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_ID1#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_ID1)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_ID),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devID1, NULL
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@dev_ID2#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_ID2)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_ID),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devID2, NULL
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@dev_ID3#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_ID3)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_ID),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devID3, NULL
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@dev_ID4#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_ID4)
            UINODE_ENABLED,
            .advance = NODE_LINK(dev_ID1),
            .select = NODE_LINK(dev_ID),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devID4, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_Tag#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_TAG)
            UINODE_ENABLED,
            .advance = NODE_LINK(devInfo_goback),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_Tag1#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_TAG1)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_Tag),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devTag1, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_Tag2#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_TAG2)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_Tag),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devTag2, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_Tag3#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_TAG3)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(dev_Tag),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devTag3, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@dev_Tag4#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(DEV_TAG4)
            UINODE_ENABLED,
            .advance = NODE_LINK(dev_Tag1),
            .select = NODE_LINK(dev_Tag),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_ff_devTag4, NULL
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, "devInfo_goback")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(GOBACK)
            UINODE_ENABLED,
            NODE_LINK(dev_Addr), //wrap around
            NODE_LINK(ffInfo_DevInfo), //do it on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3, ui_Clr_Lin3
    },
    #MP Expand EndNode()
#MP ;--------------------- End of Dev Info -------------------
#MP
#MP FF_Blk_Info_Tags(#@RB#, 4)
#MP FF_Blk_Info_Tags(#@TB#, 4)
#MP FF_Blk_Info_Tags(#@AO#, 4)
#MP FF_Blk_Info_Tags(#@PID#, 4)
#MP FF_Blk_Info_Tags(#@PID2#, 4)
#MP FF_Blk_Info_Tags(#@DO#, 4)
#MP FF_Blk_Info_Tags(#@DO2#, 4)
#MP
#MP Endm ; sub menu ff info ----
#MP
#MP
#MP;//-----------FF function, simulation active/deactive--------------
#MP Macro Simulation_root
    #MP Expand StartNode(nodetype_nav, #@Simulation_root#)
    {
        #MP Expand NodetType()
        -TMOUT_TICKS_REFRESH, //Refresh every half-second
        #MP TEXTLABEL(SIMULATION)
        UINODE_ENABLED,
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        .select = NODE_LINK(Simulation_Deactive),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

#MP Endm ;----------- MainMenu Simulation root-------------------------------
#MP Macro Simulation_sub
    #MP Expand StartNode(nodetype_xsel, #@Simulation_Active#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(SIMULATIONACT)
            UINODE_ENABLED,
            .advance = NODE_LINK(Simulation_Deactive),
            .select = NODE_LINK(Simulation_root),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_SimulationActive
    },
    #MP Expand EndNode()
#MP
    #MP Expand StartNode(nodetype_xsel, #@Simulation_Deactive#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //Refresh every half-second
            #MP TEXTLABEL(SIMULATIONDEACT)
            UINODE_ENABLED,
            .advance = NODE_LINK(Simulation_Active),
            .select = NODE_LINK(Simulation_root),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_SimulationDeactive
    },
    #MP Expand EndNode()
#MP
#MP Endm ;----------- MainMenu Simulation sub nodes-------------------------------
#MP
