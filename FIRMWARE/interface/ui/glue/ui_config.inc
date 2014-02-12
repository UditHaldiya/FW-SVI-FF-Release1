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

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_config.inc $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $
*/
/* $History: ui_config.inc $
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
 * User: Arkkhasin    Date: 6/06/09    Time: 8:04a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Fixed tight shut-off link
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:53a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Language node macro-ized
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 1:53p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Temporary while rearranging the UI glue layer
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/08/09    Time: 4:37p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Macro-ized mode change nodes, extracted in separate .inc files, with
 * new lock scheme
 * PST setup menu extracted in a separate file
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/06/09    Time: 12:32a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Macro-ized Setup mode submenus, extracted in separate .inc files, with
 * new lock scheme
*/
#MP Endif
#MP
#MP Macro AirAction_En ;enable entries for ATO/ATC
    #MP     ;The next two enable entries enable exactly one of the ATO and ATC
    #MP Expand StartEnEntry(#@AIR_TO_OPEN_#)
        DATAFUNC(ui_GetAirAction), 0, 1U, 1U,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@AIR_TO_CLOSE_#)
        DATAFUNC(ui_GetAirAction), 0, 1U, 0U,
        0 //chain terminated
    #MP Expand EndEnEntry()
#MP Endm
#MP
#MP If FEATURE_UI_TIGHT_SHUTOFF == FEATURE_UI_TIGHT_SHUTOFF_SHOW
#MP Macro TightShutoffLo_En
    #MP     ;The next two enable one of the TS-ON/TS-OFF options
    #MP Expand StartEnEntry(#@TSO_LO_OFF_VAL#)
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xlow]), TIGHT_SHUTOFF_MASK, TIGHT_SHUTOFF_VALUE_OFF, //from configure.h
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@TSO_LO_ON_VAL#)
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xlow]), TIGHT_SHUTOFF_MASK, TIGHT_SHUTOFF_VALUE_ON, //from configure.h
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
#MP Endm
#MP ;-------------- enable node: tight shutoff hi -----------------------
#MP Macro TightShutoffHi_En
    #MP     ;The next two enable one of the TS-ON/TS-OFF options
    #MP Expand StartEnEntry(#@TSO_HI_OFF_VAL#)
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xhi]), TIGHT_SHUTOFF_MASK, TIGHT_SHUTOFF_VALUE_OFF, //from configure.h
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@TSO_HI_ON_VAL#)
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xhi]), TIGHT_SHUTOFF_MASK, TIGHT_SHUTOFF_VALUE_ON, //from configure.h
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
#MP Endm
#MP ;----------- enable node: manual tune ----------------------------------
#MP Macro Manual_Tune_En
#MP         ;
    #MP     ;The next umpteen enable entries enable exactly one of the possible
    #MP     ;manual tune options
    #MP ; ---------- for viewing, we'll need to retrieve congiguration data ---------
    #MP Expand StartEnEntry(#@enMANUALTUNE_AUTO#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_AUTOTUNE,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_5CAMFLEX#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_5CAMFLEX,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_6CAMFLEX#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_6CAMFLEX,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_10_12#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_10_20,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_10_24#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_10_24,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_23_12#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_23_12,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_9CAMFLEX#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_9CAMFLEX,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enMANUALTUNE_23_24#)
        DATAFUNC(ui_GetPidParamSet), 0, MANUALTUNE_MASK, CTLPARAMSET_23_24,
        0 //chain terminated
    #MP Expand EndEnEntry()
#MP Endm
#MP ; --------------sub nodes at View Data --------------------------
#MP Macro TightShutoffLo_View
    #MP;    //--------- tight shut-off view ---------------
    #MP Expand StartNode(nodetype_alt, #@view_TightLoON#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //refresh rate
            #MP TEXTLABEL(TSO_LO_ON)
            ENOFFS(TSO_LO_ON_VAL),
            NODE_LINK(view_TightLoOFF),
            NODE_LINK(man_viewData),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_POSITION,
        },
        NULL,
        NODE_LINK(var_tso1_lo)
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_TightLoOFF#)
    {
        #MP Expand NodetType()
        -TMOUT_TICKS_REFRESH, //refresh rate
        #MP TEXTLABEL(TSO_LO_OFF)
        ENOFFS(TSO_LO_OFF_VAL),
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
#MP ;------------------------------------------------------------------
#MP Macro TightShutoffHi_View
    #MP;    //--------- tight shut-off view ---------------
    #MP Expand StartNode(nodetype_alt, #@view_TightHiON#)
    {
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //refresh rate
            #MP TEXTLABEL(TSO_HI_ON)
            ENOFFS(TSO_HI_ON_VAL),
            NODE_LINK(view_TightHiOFF),
            NODE_LINK(man_viewData),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_POSITION,
        },
        NULL,
        NODE_LINK(var_tso1_hi)
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_TightHiOFF#)
    {
        #MP Expand NodetType()
        -TMOUT_TICKS_REFRESH, //refresh rate
        #MP TEXTLABEL(TSO_HI_OFF)
        ENOFFS(TSO_HI_OFF_VAL),
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
#MP ;----------sub node under settings ------------------------------
#MP Macro TightShutoffLo_Config
    #MP;//------ tight shut-off lo
    #MP Expand StartNode(nodetype_sel, #@tso_lo_on#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(TSO_LO_ON)
            ENOFFS(TSO_LO_ON_VAL),
            NODE_LINK(tso_lo_off), //next
            NODE_LINK(var_tso_lo), //will in effect toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_POSITION,
        },
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xlow]),
        TIGHT_SHUTOFF_VALUE_OFF, //next value
        TIGHT_SHUTOFF_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_var, #@var_tso_lo#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .data_index = Xlow, //location offset
        .parent = NODE_LINK(tso_lo_off),
        .varfunc = VARFUNC(ui_GetTightShutoff),
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_limvar, #@var_tso1_lo#)
    {
        {
            #MP Expand NodetType()
            .checksum_offset = 0, //no checksum
            .en = ENOFFS(TSO_LO_ON_VAL),
            .data_index = Xlow, //location offset
            .parent = NODE_LINK(tso_lo_on),
            .varfunc = VARFUNC(ui_GetTightShutoff),
        },
        UI_VARLIM(&tso_limits[0]),
        UI_VARLIM(&tso_limits[1])
    },
    #MP Expand EndNode()
    #MP;//Need a dummy navnode to prevent display of the var when it's off
    #MP Expand StartNode(nodetype_nav, #@dummy_tso_lo#)
    {
        #MP Expand NodetType()
        1, //instant timeout
        #MP TEXTLABEL(TSO_LO_OFF)
        UINODE_ENABLED,
        NODE_LINK(var_tso1_lo), //next
        NODE_LINK(var_tso1_lo), //on * go straight to var setting
        XLATE_INDEX(xlate_SelectIfEnabled),
        .uid = UNITSID_POSITION,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@tso_lo_off#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(TSO_LO_OFF)
            ENOFFS(TSO_LO_OFF_VAL),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(dummy_tso_lo), //on * go straight to var setting
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xlow]),
        TIGHT_SHUTOFF_VALUE_ON, //next value
        TIGHT_SHUTOFF_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
#MP Endm
#MP Macro TightShutoffHi_Config
    #MP;//------ tight shut-off hi
    #MP Expand StartNode(nodetype_sel, #@tso_hi_on#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(TSO_HI_ON)
            ENOFFS(TSO_HI_ON_VAL),
            NODE_LINK(tso_hi_off), //next
            NODE_LINK(var_tso_hi), //will in effect toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_POSITION,
        },
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xhi]),
        TIGHT_SHUTOFF_VALUE_OFF, //next value
        TIGHT_SHUTOFF_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_var, #@var_tso_hi#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .data_index = Xhi, //location offset
        .parent = NODE_LINK(tso_hi_off),
        .varfunc = VARFUNC(ui_GetTightShutoff),
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_limvar, #@var_tso1_hi#)
    {
        {
            #MP Expand NodetType()
            .checksum_offset = 0, //no checksum
            .en = ENOFFS(TSO_HI_ON_VAL),
                        .data_index = Xhi, //location offset
            .parent = NODE_LINK(tso_hi_on),
                        .varfunc = VARFUNC(ui_GetTightShutoff),
        },
        UI_VARLIM(&tco_limits[0]),
        UI_VARLIM(&tco_limits[1])
    },
    #MP Expand EndNode()
    #MP;//Need a dummy navnode to prevent display of the var when it's off
    #MP Expand StartNode(nodetype_nav, #@dummy_tso_hi#)
    {
        #MP Expand NodetType()
        1, //instant timeout
        #MP TEXTLABEL(TSO_HI_OFF)
        UINODE_ENABLED,
        NODE_LINK(var_tso1_hi), //next
        NODE_LINK(var_tso1_hi), //on * go straight to var setting
        XLATE_INDEX(xlate_SelectIfEnabled),
        .uid = UNITSID_POSITION,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@tso_hi_off#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(TSO_HI_OFF)
            ENOFFS(TSO_HI_OFF_VAL),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(dummy_tso_hi), //on * go straight to var setting
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetUntypedCtlLimits), MN_OFFSETOF(CtlLimits_t, EnableTightShutoff[Xhi]),
        TIGHT_SHUTOFF_VALUE_ON, //next value
        TIGHT_SHUTOFF_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
#MP Endm
#MP Endif ;FEATURE_UI_TIGHT_SHUTOFF == FEATURE_UI_TIGHT_SHUTOFF_SHOW
#MP
#MP Macro Config ;Root node: name comes from uinode_autoname
#MP Setstr Config_autoname = uinode_autoname
#MP Save uinode_autoname
    #MP StartNode(nodetype_xsel, Config_autoname)
    {
        {
            #MP Expand NodetType()
            .tmout = 0, //no timeout
            #MP TEXTLABEL(CONF)
            .en = ENOFFS(OPMODE_SETUP),
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            #MP Setstr uinode_autoname = Config_autoname LockCheckSuffix ;
            .select = NODE_LINK(#mp%suinode_autoname),
            .xfuncx = XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_getCalibMenuData
    },
    #MP EndNode
#MP Restore uinode_autoname
#MP Endm
#MP
#MP ;-------------- begin sub nodes, configuration setup loop "+-"-navigable --------------
#MP Macro Air_To_OpenClose
    #MP;//------- Air to Open/Close
    #MP Expand StartNode(nodetype_sel, #@airto_open#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(ATO)
            ENOFFS(AIR_TO_OPEN_),
            NODE_LINK(airto_close), //on +
            NODE_LINK(airto_close), //on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetAirAction), 0,
        0U, //new value (closed)
        1U, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@airto_close#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(ATC)
            ENOFFS(AIR_TO_CLOSE_),
            NODE_LINK(#mp%sUIGraphNodeNext), //on +
            NODE_LINK(airto_open), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetAirAction), 0,
        1U, //new value (open)
        1U, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
#MP Endm
#MP
#MP Macro Pressure_Units
    #MP;//-------- pressure units --------
    #MP Expand StartNode(nodetype_xsel, "Pressure_units")
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(UNITS_PREFIX)
            #MP; UINODE_ENABLED,
            ENOFFS(PRESS_ACTUATOR),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(Pressure_units), //toggle by wrapping to itself
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_PRESSURE,
        },
        NULL, ui_UnitsIncrement
    },
    #MP Expand EndNode()
#MP Endm
#MP ;------view manual tune ---------------------------------
#MP Macro Manual_Tune_View
#MP
    #MP;    //-------- manual tune -------------
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_Auto#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_AUTO)
        ENOFFS(enMANUALTUNE_AUTO),
        NODE_LINK(view_ManualTune_5Camflex),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_5Camflex#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_5CAM)
        ENOFFS(enMANUALTUNE_5CAMFLEX),
        NODE_LINK(view_ManualTune_6Camflex),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_6Camflex#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_6CAM)
        ENOFFS(enMANUALTUNE_6CAMFLEX),
        NODE_LINK(view_ManualTune_10_20),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_10_20#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_10_12)
        ENOFFS(enMANUALTUNE_10_12),
        NODE_LINK(view_ManualTune_10_24),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_10_24#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_10_24)
        ENOFFS(enMANUALTUNE_10_24),
        NODE_LINK(view_ManualTune_23_12),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_23_12#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_23_12)
        ENOFFS(enMANUALTUNE_23_12),
        NODE_LINK(view_ManualTune_9Camflex),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_9Camflex#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_9CAM)
        ENOFFS(enMANUALTUNE_9CAMFLEX),
        NODE_LINK(view_ManualTune_23_24),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_ManualTune_23_24#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MTUNE_23_24)
        ENOFFS(enMANUALTUNE_23_24),
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm
#MP Macro Manual_tune_slots
    #MP;//------- Manual tune --------------
    #MP Expand StartNode(nodetype_sel, #@manual_tune_auto#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_AUTO)
            .en = ENOFFS(enMANUALTUNE_AUTO),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_5CAMFLEX, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_5cam#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_5CAM)
            .en = ENOFFS(enMANUALTUNE_5CAMFLEX),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_6CAMFLEX, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_6cam#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_6CAM)
            .en = ENOFFS(enMANUALTUNE_6CAMFLEX),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_10_20, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_10_12#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_10_12)
            .en = ENOFFS(enMANUALTUNE_10_12),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_10_24, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_10_24#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_10_24)
            .en = ENOFFS(enMANUALTUNE_10_24),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_23_12, //next value
        MANUALTUNE_MASK, //mask
        0, //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_23_12#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_23_12)
            .en = ENOFFS(enMANUALTUNE_23_12),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_9CAMFLEX, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_9cam#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_9CAM)
            .en = ENOFFS(enMANUALTUNE_9CAMFLEX),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(#mp%sUIGraphNodeNext), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_23_24, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@manual_tune_23_24#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(MTUNE_23_24)
            .en = ENOFFS(enMANUALTUNE_23_24),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(manual_tune_auto), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetPidParamSet), 0,
        CTLPARAMSET_AUTOTUNE, //next value
        MANUALTUNE_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
#MP Endm
#MP ;-------------- end sub nodes, configuration setup loop "+-"-navigable --------------

#MP Macro ConfigMenu ;Submenu to set up calib params; requires that the root node be defined
#MP If Defined(Config_autoname {STR}) ;only then do we generate the nodes
    #MP ProtectedEntry[%sConfig_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
#MP Save uinode_autoname
#MP Setstr uinode_autoname = Config_autoname TargetNodeSuffix
    #MP ;-------------- configuration setup loop "+-"-navigable --------------
    #MP Expand StartNode(nodetype_nav, uinode_autoname) ;conf proxy
    { //A proxy node to get to the submenu (since the 1st entry can be disabled)
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(CONF)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

    #MP condex(Air_To_OpenClose)
#MP
#MP
#MP ;Stick Level custom units here for now. TODO: Do it right!
#MP If FEATURE_CUSTOM_UNITS == FEATURE_CUSTOM_UNITS_SUPPORTED
    #MP CustomUnitsMenuRoot[Level_units_root, UNITSID_POSITION]
#MP Endif ;FEATURE_CUSTOM_UNITS_SUPPORTED
#MP
#MP
#MP condex(SigSpCharact_Setup)
#MP
#MP If FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
        #MP condex(Pressure_Units)
#MP Endif ;FEATURE_PRESSURE_INPUTS_INCLUDED
#MP
#MP condex(TightShutoffLo_Config)
#MP
    #MP;//--------- language
    #MP condex(Language)

    #MP;//-------- back to configuration's top --------------
    #MP Expand StartNode(nodetype_nav, #@conf_done#)
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(UP2SETUP)
        UINODE_ENABLED,
        NODE_LINK(airto_open), //wrap
        NODE_LINK(try_toConf), //try to go up
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@try_toConf#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            UINODE_ENABLED,
            NODE_LINK(#mp%sConfig_autoname), //go up regardless but display FAILURE on data save failure
            NODE_LINK(#mp%sConfig_autoname),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setCalibMenuData, NULL // ui-setCalibMenuData() must return failure if fails
    },
    #MP Expand EndNode()
    #MP ;-------------- end configuration setup loop "+-"-navigable --------------
#MP     Restore uinode_autoname
#MP Endif ;Defined(Config_autoname {STR})
#MP Endm
#MP
#MP Macro FFBasSetupMenu ;Submenu to set up calib params; requires that the root node be defined
#MP If Defined(Config_autoname {STR}) ;only then do we generate the nodes
    #MP ProtectedEntry[%sConfig_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
#MP Save uinode_autoname
#MP Setstr uinode_autoname = Config_autoname TargetNodeSuffix
    #MP ;-------------- configuration setup loop "+-"-navigable --------------
    #MP Expand StartNode(nodetype_nav, uinode_autoname) ;conf proxy
    { //A proxy node to get to the submenu (since the 1st entry can be disabled)
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(CONF)
        UINODE_ENABLED,
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP ;------- air to open/close ---------------
    #MP condex(Air_To_OpenClose)
#MP ;---------open/stop adjustment ------------------------
#MP If FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
#MP     condex(Cal_Open_Stop)
#MP Endif ;FEATURE_UI_OPSTOP_ADJ == FEATURE_UI_OPSTOP_ADJ_SHOW
#MP ;-------- Characiterization selection -----------------
#MP     condex(SigSpCharact_Setup)
#MP
#MP;//--------- language ----------------------------------
#MP     condex(Language)
#MP;//-------- back to configuration's top --------------
    #MP Expand StartNode(nodetype_nav, #@conf_done#)
    {
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(UP2SETUP)
        UINODE_ENABLED,
        NODE_LINK(airto_open), //wrap
        NODE_LINK(try_toConf), //try to go up
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, #@try_toConf#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            UINODE_ENABLED,
            NODE_LINK(#mp%sConfig_autoname), //go up regardless but display FAILURE on data save failure
            NODE_LINK(#mp%sConfig_autoname),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setCalibMenuData, NULL // ui-setCalibMenuData() must return failure if fails
    },
    #MP Expand EndNode()
#MP ;-------------- end configuration setup loop "+-"-navigable --------------
#MP     Restore uinode_autoname
#MP Endif ;Defined(Config_autoname {STR})
#MP Endm

