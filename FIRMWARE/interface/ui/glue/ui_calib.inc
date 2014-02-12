#MP If 0
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uipst.inc
    \brief Macros for PST configuration in MNCB

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_calib.inc $
    $Date: 1/30/12 2:16p $
    $Revision: 2 $
    $Author: Arkkhasin $
*/
/* $History: ui_calib.inc $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:16p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * Removed signal setpoint and loop signal data FBO TFS:8782
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/17/09   Time: 10:46p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * A step in modularization of "enable" table entries and C headers:
 * macoized and moved to places where the graph nodes themselves are
 * defined.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/06/09    Time: 12:32a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Macro-ized Setup mode submenus, extracted in separate .inc files, with
 * new lock scheme
*/
#MP Endif
#MP
#MP Macro Calib ;Root node
#MP Setstr Calib_autoname = "Calib"
    #MP;//calib entry retrieves/saves data on the fly to ensure consistency with long processes
    #MP;//which retieve/store data autonomously.
    #MP StartNode(nodetype_xsel, Calib_autoname)
    {
        {
            #MP Expand NodetType()
            .tmout = 0, //no timeout
            #MP TEXTLABEL(CAL)
            .en = ENOFFS(OPMODE_SETUP),
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            #MP Setstr uinode_autoname = Calib_autoname LockCheckSuffix ;
            .select = NODE_LINK(#mp%suinode_autoname),
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_getCalibMenuData
    },
    #MP EndNode
#MP Endm
#MP
#MP Macro Method ;Root node
#MP Setstr Method_autoname = "Method"
    #MP;//Method entry retrieves/saves data on the fly to ensure consistency with long processes
    #MP;//which retieve/store data autonomously.
    #MP StartNode(nodetype_xsel, Method_autoname)
    {
        {
            #MP Expand NodetType()
            .tmout = 0, //no timeout
            #MP TEXTLABEL(METHOD)
            .en = ENOFFS(OPMODE_SETUP),
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            #MP Setstr uinode_autoname = Method_autoname LockCheckSuffix ;
            .select = NODE_LINK(#mp%suinode_autoname),
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, NULL
    },
    #MP EndNode
#MP Endm
#MP ;--------------- begin, macros for sub nodes ----------------------------
#MP Macro Cal_Open_Stop_View
    #MP;    //--------- open/stop adjustment view ---------------
    #MP Expand StartNode(nodetype_alt, #@view_cal_openstop_adj#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //refresh rate
            #MP TEXTLABEL(STOP_OP)
            UINODE_ENABLED,
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(man_viewData),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_POSITION,
        },
        NULL,
        NODE_LINK(var_cal_open_stop)
    },
    #MP Expand EndNode()
#MP Endm
#MP ;---------------------------------
#MP Macro Cal_Open_Stop
    #MP Expand StartNode(nodetype_nav, #@cal_open_stop#)
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(STOP_OP)
        UINODE_ENABLED,
        #MP over_the_var = UIGraphNode+2
        NODE_LINK(#mp%sUIGraphNode_%dover_the_var),
        NODE_LINK(var_cal_open_stop), //do it on *
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_POSITION,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_limvar, #@var_cal_open_stop#)
    {
        {
            #MP Expand NodetType()
            .checksum_offset = 0, //no checksum
            .en = UINODE_ENABLED,
            .data_index = Xhi, //location offset
            .parent = NODELINK_NONE,
            .varfunc = VARFUNC(ui_getOpenStopAdjustment_var),
        },
        UI_VARLIM(&OpenStopAdj_range[0]),
        UI_VARLIM(&OpenStopAdj_range[1])
    },
    #MP Expand EndNode()
#MP Endm
#MP ;--------------------------------------
#MP Macro Cal_Tune
    #MP Expand StartNode(nodetype_xsel, #@cal_tune#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(TUNE)
            ENOFFS(enMANUALTUNE_AUTO),
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(cal_tune),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_process_SelfTune
    },
    #MP Expand EndNode()
#MP Endm
#MP
#MP ;--------------------------------------
#MP Macro Cal_Signal_Hi_Lo
    #MP Expand StartNode(nodetype_xsel, #@cal_sig_lo#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(SIG_LO)
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_ON
            UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_OFF
            ENOFFS(COST_FULL),
#endif
            NODE_LINK(cal_sig_hi),
            NODE_LINK(var_cal_sig_lo), //do it on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_SIGNAL_ENTRY,
        },
        ui_AdjustSigLimits, NULL //adjust overlapping limits on entry
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_limvar, #@var_cal_sig_lo#)
    {
        {
            #MP Expand NodetType()
            .checksum_offset = 0, //no checksum
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_ON
            .en = UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_OFF
            .en = ENOFFS(COST_FULL),
#endif
            .parent = NODELINK_NONE,
            .data_index = UI_SIGNALDATAINDEX(nLowSignal), //location offset
            .varfunc = VARFUNC(ui_getHiLoSignal),
        },
        UI_VARLIM(&SignalSP_range[0]),
        UI_VARLIM(&hiloSignal.lowupperlim)
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@cal_sig_hi#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(SIG_HI)
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_ON
            UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_OFF
            ENOFFS(COST_FULL),
#endif
            NODE_LINK(cal_done),
            NODE_LINK(var_cal_sig_hi), //do it on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_SIGNAL_ENTRY,
        },
        ui_AdjustSigLimits, NULL //adjust overlapping limits on entry
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_limvar, #@var_cal_sig_hi#)
    {
        {
            #MP Expand NodetType()
            .checksum_offset = 0, //no checksum
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_ON
            .en = UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_OFF
            .en = ENOFFS(COST_FULL),
#endif
            .parent = NODELINK_NONE,
            .data_index = UI_SIGNALDATAINDEX(nHighSignal), //location offset
            .varfunc = VARFUNC(ui_getHiLoSignal),
        },
        UI_VARLIM(&hiloSignal.highlowerlim),
        UI_VARLIM(&SignalSP_range[1])
    },
    #MP Expand EndNode()
#MP Endm
#MP ;---------------------
#MP ;---------------end, macros for sub nodes --------------------------------
#MP
#MP Macro CalibMenu ;Submenu to set up calib params; requires that the root node be defined
#MP If Defined(Calib_autoname {STR}) ;only then do we generate the nodes
    #MP ProtectedEntry[%sCalib_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
    #MP ;-------------- calibration setup loop "+-"-navigable --------------
    #MP Expand DeclareStartNode(#@UINODEID_CALSTOPS#)
#MP Save uinode_autoname
#MP Setstr uinode_autoname = Calib_autoname TargetNodeSuffix
    #MP CalStops ;node name supplied in uinode_autoname
#MP
#MP If FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
#MP     condex(Cal_Open_Stop)
#MP Endif ;FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
#MP
#MP If FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
#MP     condex(Cal_Tune)
#MP Endif ;FEATURE_POSITION_CONTROL
#MP
#MP If FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#MP If FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
#MP     condex(Cal_Signal_Hi_Lo)
#MP Endif ;FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
#MP Endif ;FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#MP
    #MP;//-------- back to calibration's top --------------
    #MP Expand StartNode(nodetype_nav, #@cal_done#)
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(UP2SETUP)
        UINODE_ENABLED,
        NODE_LINK(#mp%suinode_autoname), //wrap
        NODE_LINK(try_toCalib), //try to go up
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
        #MP Expand StartNode(nodetype_xsel, #@try_toCalib#)
        {
            {
                #MP Expand NodetType()
                TMOUT_TICKS_DEFAULT,
                #MP TEXTLABEL(FAILED)
                UINODE_ENABLED,
                NODE_LINK(#mp%sCalib_autoname), //go up regardless but display FAILURE on data save failure
                NODE_LINK(#mp%sCalib_autoname),
                XLATE_INDEX(xlate_NextIfEnabled),
                .uid = UNITSID_NONE,
            },
            ui_setCalibMenuData, NULL // ui-setCalibMenuData() must return failure if fails
        },
        #MP Expand EndNode()
        #MP CalStopsMenu ;include it here
#MP     Restore uinode_autoname
#MP Endif ;Defined(Calib_autoname {STR})
#MP Endm
#MP ;-----------------------------------------------
#MP Macro FFAdvSetupMenu ;Submenu to set up calib params; requires that the root node be defined
#MP If Defined(Calib_autoname {STR}) ;only then do we generate the nodes
    #MP ProtectedEntry[%sCalib_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
    #MP ;-------------- calibration setup loop "+-"-navigable --------------
#MP Save uinode_autoname
#MP Setstr uinode_autoname = Calib_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_nav, uinode_autoname) ;calib proxy
    { //A proxy node to get to the submenu (since the 1st entry can be disabled)
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(CAL)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP If FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
        #MP condex(Pressure_Units)
#MP Endif ;FEATURE_PRESSURE_INPUTS_INCLUDED
#MP
#MP ;Stick Level custom units here for now. TODO: Do it right!
#MP If FEATURE_CUSTOM_UNITS == FEATURE_CUSTOM_UNITS_SUPPORTED
    #MP CustomUnitsMenuRoot[Level_units_root, UNITSID_POSITION]
#MP Endif ;FEATURE_CUSTOM_UNITS_SUPPORTED
#MP ;-------- tight shutoff lo on/off ----------------------------
#MP condex(TightShutoffLo_Config)
#MP ;-------- tight shutoff hi on/off ----------------------------
#MP condex(TightShutoffHi_Config)
#MP
#MP If FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#MP If FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
#MP     condex(Cal_Signal_Hi_Lo)
#MP Endif ;FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
#MP Endif ;FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#MP
#MP If FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
#MP     condex(Manual_tune_slots) ;include it here
#MP Endif ;FEATURE_POSITION_CONTROL
#MP;//-------- back to calibration's top --------------
    #MP Expand StartNode(nodetype_nav, #@cal_done#)
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(UP2SETUP)
        UINODE_ENABLED,
        NODE_LINK(Pressure_units), //wrap
        NODE_LINK(try_toCalib), //try to go up
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@try_toCalib#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            UINODE_ENABLED,
            NODE_LINK(#mp%sCalib_autoname), //go up regardless but display FAILURE on data save failure
            NODE_LINK(#mp%sCalib_autoname),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setCalibMenuData, NULL // ui-setCalibMenuData() must return failure if fails
    },
    #MP Expand EndNode()
#MP Restore uinode_autoname
#MP ;--------------------------------------------------------
#MP Endif ;Defined(Calib_autoname {STR})
#MP Endm
#MP
#MP ;-----------------------------------------------
#MP Macro MethodMenu ;Submenu to set up method params; requires that the root node be defined
#MP If Defined(Method_autoname {STR}) ;only then do we generate the nodes
    #MP ProtectedEntry[%sMethod_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
    #MP ;-------------- calibration setup loop "+-"-navigable --------------
#MP Save uinode_autoname
#MP Setstr uinode_autoname = Method_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_xsel, uinode_autoname) ;method proxy
    { //A proxy node to get to the submenu (since the 1st entry can be disabled)
        {
            #MP Expand NodetType()
            1, //smallest timeout
            #MP TEXTLABEL(METHOD)
            UINODE_ENABLED,
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(#mp%sUIGraphNodeNext),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_getCalibMenuData, NULL
    },
    #MP Expand EndNode()
#MP ;--------find stop ------------------------
    #MP Expand DeclareStartNode(#@UINODEID_CALSTOPS#)
#MP Setstr uinode_autoname = "Calib_Stops"
    #MP CalStops ;node name supplied in uinode_autoname
#MP ;-------- AutoTune ------------------------------------
#MP If FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
#MP     condex(Cal_Tune)
#MP Endif ;FEATURE_POSITION_CONTROL
#MP;//-------- back to calibration's top --------------
    #MP Expand StartNode(nodetype_nav, #@method_done#)
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(UP2SETUP)
        UINODE_ENABLED,
        NODE_LINK(#mp%suinode_autoname), //wrap
        NODE_LINK(#mp%sMethod_autoname),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP ;------------------------------------------------
    #MP CalStopsMenu ;include it here
#MP Restore uinode_autoname
#MP Endif ;Defined(Method_autoname {STR})
#MP Endm
#MP ;-------------------------------------------------------------------
#MP ;
#MP If FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#MP If FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
#MP Macro ViewSignalCal
#MP
    #MP;    //---------- view low signal cal ---------------
    #MP Expand StartNode(nodetype_alt, #@view_calLowSig#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //refresh rate
            #MP TEXTLABEL(SIG_LO)
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_ON
            UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_OFF
            ENOFFS(COST_FULL),
#endif
            NODE_LINK(view_calHighSig),
            NODE_LINK(man_viewData),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_SIGNAL_ENTRY,
        },
        ui_getCalibMenuData,
        NODE_LINK(var_cal_sig_lo)
    },
    #MP Expand EndNode()
#MP;    //---------- view high signal cal ---------------
    #MP Expand StartNode(nodetype_alt, #@view_calHighSig#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //refresh rate
            #MP TEXTLABEL(SIG_HI)
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_ON
            UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE == OPTION_OFF
            ENOFFS(COST_FULL),
#endif
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(man_viewData),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_SIGNAL_ENTRY,
        },
        ui_getCalibMenuData,
        NODE_LINK(var_cal_sig_hi)
    },
    #MP Expand EndNode()
#MP Endm
#MP Endif ;FEATURE_UI_SIGNAL_LIMITS == FEATURE_UI_SIGNAL_LIMITS_SHOW
#MP Endif ;FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
