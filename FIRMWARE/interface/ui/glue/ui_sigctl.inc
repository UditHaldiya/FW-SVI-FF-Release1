#MP If 0
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uisignalgen.inc
    \brief Macros for output signal generator in DLT

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_sigctl.inc $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $
*/
/* $History: ui_sigctl.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 4  *****************
 * User: Derek Li     Date: 6/04/09    Time: 3:34p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Supported A02(Retransmit) signal generator
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/19/09    Time: 10:38p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Lint
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/17/09    Time: 12:28a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Preliminary check-in on the way to support 32-bit UI variables
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/15/09    Time: 2:45a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * UI for main output signal generator
*/
#MP Endif

#MP ;---------------------------------------------------------------------------
#MP Macro SignalGeneratorRoot ;not a submenu! inline stretch of modes
#MP Save uinode_autoname
#MP Setstr SignalGenerator_autoname "SignalGeneratorRoot"
#MP
    #MP Expand StartNode(nodetype_xsel, SignalGenerator_autoname)
    {
    	{
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(SIGNALGEN)
        ENOFFS(OPMODE_SETUP_OR_MANUAL),
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        #MP Setstr uinode_autoname = SignalGenerator_autoname LockCheckSuffix ;
        .select = NODE_LINK(#mp%suinode_autoname),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
        },
        NULL, NULL
    },
    #MP Expand EndNode()
#MP Restore uinode_autoname
#MP Endm
#MP ;---------------------------------------------------------------------------

#MP ;---------------------------------------------------------------------------
#MP Macro SignalGeneratorMenu ;Signal Generator menu
#MP Save uinode_autoname
    #MP ProtectedEntry[%sSignalGenerator_autoname, 0, LOCK_MANUAL]
#MP Setstr uinode_autoname = SignalGenerator_autoname TargetNodeSuffix

#MP ;------------------ SignalGeneratorMenu loop "+-"-navigable ----------------
	#MP Expand StartNode(nodetype_nav, uinode_autoname)
    {   //A proxy node to get to the submenu (since the 1st entry can be disabled)
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(SIGNALGEN)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
	#MP Expand EndNode()

	#MP AO_1_Gen_Menu1 ;A0_1 is for LOOP CURRENT; A0_2 is for Retransmit
	
	#MP Expand StartNode(nodetype_nav, #@AO_1_GEN#) ;A0_1 is for LOOP CURRENT; A0_2 is for Retransmit
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(AO_1_GEN)
        //.en = UINODE_ENABLED,
        .en = ENOFFS(RetransmitOptionEn),
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        .select = NODE_LINK(SigA01Gen),
        XLATE_INDEX(xlate_StdMenu),
        //XLATE_INDEX(xlate_SelectIfEnabled), //IMPORTANT:if this node is not enable, go the the "select" enable node
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

	#MP Expand StartNode(nodetype_nav, #@AO_2_GEN#) ;A0_1 is for LOOP CURRENT; A0_2 is for Retransmit
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(AO_2_GEN)
        .en = ENOFFS(RetransmitOptionEn),
        //.en = UINODE_ENABLED,
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        .select = NODE_LINK(SigA02Gen),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
	
	#MP Expand StartNode(nodetype_nav, #@SignalGenerator_end#)
    {
        #MP Expand NodetType()
        0, 
        #MP TEXTLABEL(GOBACK)
        .en = UINODE_ENABLED,
        .advance = NODE_LINK(AO_1_GEN), //wrap up to the real first node "AO_1_GEN"
        .select = NODE_LINK(#mp%sSignalGenerator_autoname), //back to top menu
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
	#MP Expand EndNode()
	
	#MP ;Include submenus here
    #MP AO_1_Gen_Menu
    #MP AO_2_Gen_Menu
    #MP AO_1_Gen_SubMenu1
#MP Restore uinode_autoname
#MP Endm
#MP ;---------------------------------------------------------------------------

#MP ;---------------------------------------------------------------------------
#MP Macro AO_1_Gen_Menu ;Signal A0_1 Generator submenu
    #MP Expand StartNode(nodetype_nav, #@SigA01Gen#)
    {
        #MP Expand NodetType()
        -1, //min refresh period
        #MP TEXTLABEL(SIG)
        .en = UINODE_ENABLED,
        #MP over_the_var = UIGraphNode+2
        .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
        .select = NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_SIGNAL_ENTRY,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xlimvar, #@var_signalgen1#)
    {   // Main Output Generator variable
        {
            {
                #MP Expand NodetType()
                .checksum_offset = 0, //no checksum
                .en = UINODE_ENABLED, //if we got here, stay here (can be thrown out e.g. by failsafe)
                .parent = NODELINK_NONE,
                .data_index = 0,
                .varfunc = VARFUNC(ui_GetSignal),
            },
            UI_VARLIM(&aotab[AO_CHANNEL_LOOP_CURRENT].aoFixed_range[Xlow]),
            UI_VARLIM(&aotab[AO_CHANNEL_LOOP_CURRENT].aoFixed_range[Xhi]),
        },
        ui_SetFixedSignalAO1
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, "UnfreezeAO1")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(UNFREEZE)
            .en = ENOFFS(AO1_FROZEN),
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_UnfreezeAO1
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "AO1_control_end")
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(GOBACK) ;//don't care: the node will always be disabled
        .en = UINODE_ENABLED,
        .advance = NODE_LINK(SigA01Gen), //wrap to the first node
        .select = NODE_LINK(#mp%sSignalGenerator_autoname), //back
        XLATE_INDEX(xlate_StdWiz),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
#MP ;---------------------------------------------------------------------------

#MP ;---------------------------------------------------------------------------
#MP Macro AO_1_Gen_Menu1 ;Signal A0_1 Generator submenu
    #MP Expand StartNode(nodetype_nav, #@SigA01Gen1_Proxy#)
    {   //A proxy node to get to the submenu (since the 1st entry can be disabled)
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(SIGNALGEN)
        .en = ENOFFS(RetransmitOptionDisable),
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        .select = NODE_LINK(SigA01Gen1),
        //XLATE_INDEX(xlate_StdMenu),
        XLATE_INDEX(xlate_SelectIfEnabled),//after time out, go to the select node: "SigA01Gen1"
        .uid = UNITSID_NONE,
    },
	#MP Expand EndNode()
#MP Endm
#MP ;---------------------------------------------------------------------------


#MP ;---------------------------------------------------------------------------
#MP Macro AO_1_Gen_SubMenu1 ;Signal A0_1 Generator submenu
    #MP Expand StartNode(nodetype_nav, #@SigA01Gen1#)
    {
        #MP Expand NodetType()
        -1, //min refresh period
        #MP TEXTLABEL(SIG)
        //.en = UINODE_ENABLED,
        .en = ENOFFS(RetransmitOptionDisable),
        #MP over_the_var = UIGraphNode+2
        .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
        //.advance = NODE_LINK(UnfreezeAO1_1),
        .select = NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_SIGNAL_ENTRY,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xlimvar, #@var_signalgen1_1#)
    {   // Main Output Generator variable
        {
            {
                #MP Expand NodetType()
                .checksum_offset = 0, //no checksum
                .en = UINODE_ENABLED, //if we got here, stay here (can be thrown out e.g. by failsafe)
                .parent = NODELINK_NONE,
                .data_index = 0,
                .varfunc = VARFUNC(ui_GetSignal),
            },
            UI_VARLIM(&aotab[AO_CHANNEL_LOOP_CURRENT].aoFixed_range[Xlow]),
            UI_VARLIM(&aotab[AO_CHANNEL_LOOP_CURRENT].aoFixed_range[Xhi]),
        },
        ui_SetFixedSignalAO1
    },
    #MP Expand EndNode()

    #MP Expand StartNode(nodetype_xsel, "UnfreezeAO1_1")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(UNFREEZE)
            .en = ENOFFS(AO1_FROZEN),
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_UnfreezeAO1
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "AO1_control_end_1")
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(GOBACK) ;//don't care: the node will always be disabled
        //.en = UINODE_ENABLED,
        .en = ENOFFS(RetransmitOptionDisable),
        .advance = NODE_LINK(SigA01Gen), //wrap to the first node
        .select = NODE_LINK(#mp%sSignalGenerator_autoname), //back
        XLATE_INDEX(xlate_StdWiz),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
#MP ;---------------------------------------------------------------------------

#MP ;---------------------------------------------------------------------------
#MP Macro AO_2_Gen_Menu ;Signal A0_2 Generator submenu
    #MP Expand StartNode(nodetype_nav, #@SigA02Gen#)
    {
        #MP Expand NodetType()
        -1, //min refresh period
        #MP TEXTLABEL(SIG)
        .en = UINODE_ENABLED,
        #MP over_the_var = UIGraphNode+2
        .advance = NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
        .select = NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_SIGNAL_ENTRY,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xlimvar, #@var_signalgen2#)
    { // Main Output Generator variable
        {
            {
                #MP Expand NodetType()
                .checksum_offset = 0, //no checksum
                .en = UINODE_ENABLED, //if we got here, stay here (can be thrown out e.g. by failsafe)
                .parent = NODELINK_NONE,
                .data_index = 0,
                .varfunc = VARFUNC(ui_GetSignalAO2),
            },
            UI_VARLIM(&aotab[AO_CHANNEL_POS_RETRANSMIT].aoFixed_range[Xlow]),
            UI_VARLIM(&aotab[AO_CHANNEL_POS_RETRANSMIT].aoFixed_range[Xhi]),
        },
        ui_SetFixedSignalAO2
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, "UnfreezeAO2")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(UNFREEZE)
            .en = ENOFFS(AO2_FROZEN), 
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_UnfreezeAO2
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, "AO2_control_end")
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(GOBACK) ;//don't care: the node will always be disabled
        .en = UINODE_ENABLED,
        .advance = NODE_LINK(SigA02Gen), //wrap to the first node
        .select = NODE_LINK(#mp%sSignalGenerator_autoname), //back
        XLATE_INDEX(xlate_StdWiz),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
#MP ;---------------------------------------------------------------------------