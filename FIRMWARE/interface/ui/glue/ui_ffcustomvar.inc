#MP If 0
/**
Copyright 2012 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_ffcustomvar.inc
    \brief Macros for FF custom variables informationin MNCB

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: Ej

    $Archive: interface/ui/glue/ui_ffcustomvar.inc $
    $Date: 19/06/12 3:15p $
    $Revision: 1 $
    $Author: EricJ $
*/
/* $History: ui_ffcustomvar.inc $

 * *****************  Version 1  *****************
 * User: EricJ    Date: 19/06/12    Time: 12:32a
 * Created in FIRMWARE/interface/ui
*/
#MP Endif
#MP
#MP ;////////////////////////////////////////////////////////
#MP ;----------- enable node: manual tune ----------------------------------
#MP Macro CycleMenu_FfVar_En
#MP         ;
    #MP     ;The next umpteen enable entries enable exactly 2 of the possible
    #MP     ;FF variable options
    #MP ; ---------- for viewing, we'll need to retrieve congiguration data ---------
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_WORKINGSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_WORKINGSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_WORKINGPOS#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_WORKINGPOS,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_PIDSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PIDSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_PIDPV#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PIDPV,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_PIDOUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PIDOUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_PID2SP#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PID2SP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_PID2PV#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PID2PV,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_PID2OUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PID2OUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_AIOUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AIOUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_AI2OUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AI2OUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_AI3OUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AI3OUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_AOSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AOSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_DOSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_DOSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_DO2SP#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_DO2SP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_ISIN1#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN1,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_ISIN2#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN2,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_ISIN3#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN3,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR1_ISIN4#)
        DATAFUNC(ui_CycleMenuGetFfCustom1), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN4,
        0 //chain terminated
    #MP Expand EndEnEntry()
#MP ;--------- custom 2 ----------------------------------------------
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_WORKINGSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_WORKINGSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_WORKINGPOS#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_WORKINGPOS,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_PIDSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PIDSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_PIDPV#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PIDPV,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_PIDOUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PIDOUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_PID2SP#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PID2SP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_PID2PV#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PID2PV,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_PID2OUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_PID2OUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_AIOUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AIOUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_AI2OUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AI2OUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_AI3OUT#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AI3OUT,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_AOSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_AOSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_DOSP#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_DOSP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_DO2SP#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_DO2SP,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_ISIN1#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN1,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_ISIN2#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN2,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_ISIN3#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN3,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCYCLEMENUFFVAR2_ISIN4#)
        DATAFUNC(ui_CycleMenuGetFfCustom2), 0, CYCLEMENUFFVAR_MASK, CYCLEMENUFFVAR_ISIN4,
        0 //chain terminated
    #MP Expand EndEnEntry()
#MP Endm
#MP ;------------------------------------------------------------------
#MP Macro CycleMenu_FfVar
#MP;//------------ Working SP descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@working_setpoint1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(WORKINGSP)
            ENOFFS(enCYCLEMENUFFVAR1_WORKINGSP),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(working_setpoint2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_working_setpoint)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@working_setpoint2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(WORKINGSP)
        ENOFFS(enCYCLEMENUFFVAR2_WORKINGSP),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_working_setpoint#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetWorkingSP,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(working_setpoint1, CyclingMenuStart)
#MP;//------------ Working Pos descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@working_position1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(WORKINGPOS)
            ENOFFS(enCYCLEMENUFFVAR1_WORKINGPOS),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(working_position2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_working_positon)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@working_position2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(WORKINGPOS)
        ENOFFS(enCYCLEMENUFFVAR2_WORKINGPOS),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_working_positon#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetWorkingPos,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(working_position1, CyclingMenuStart)
#MP;//------------ Working PID SP descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_pid_sp1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFPIDSP)
            ENOFFS(enCYCLEMENUFFVAR1_PIDSP),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_pid_sp2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_pid_sp)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_pid_sp2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFPIDSP)
        ENOFFS(enCYCLEMENUFFVAR2_PIDSP),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_pid_sp#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetPidSp,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_pid_sp1, CyclingMenuStart)
#MP;//------------ Working PID PV descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_pid_pv1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFPIDPV)
            ENOFFS(enCYCLEMENUFFVAR1_PIDPV),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_pid_pv2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_pid_pv)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_pid_pv2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFPIDPV)
        ENOFFS(enCYCLEMENUFFVAR2_PIDPV),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_pid_pv#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetPidPv,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_pid_sp1, CyclingMenuStart)
#MP;//------------ Working PID OUT descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_pid_out1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFPIDOUT)
            ENOFFS(enCYCLEMENUFFVAR1_PIDOUT),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_pid_out2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_pid_out)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_pid_out2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFPIDOUT)
        ENOFFS(enCYCLEMENUFFVAR2_PIDOUT),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_pid_out#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetPidOut,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_pid_out1, CyclingMenuStart)
#MP;//------------ Working PID2 SP descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_pid2_sp1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFPID2SP)
            ENOFFS(enCYCLEMENUFFVAR1_PID2SP),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_pid2_sp2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_pid2_sp)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_pid2_sp2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFPID2SP)
        ENOFFS(enCYCLEMENUFFVAR2_PID2SP),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_pid2_sp#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetPid2Sp,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_pid2_sp1, CyclingMenuStart)
#MP;//------------ Working PID2 PV descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_pid2_pv1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFPID2PV)
            ENOFFS(enCYCLEMENUFFVAR1_PID2PV),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_pid2_pv2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_pid2_pv)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_pid2_pv2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFPID2PV)
        ENOFFS(enCYCLEMENUFFVAR2_PID2PV),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_pid2_pv#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetPid2Pv,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_pid2_sp1, CyclingMenuStart)
#MP;//------------ Working PID2 OUT descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_pid2_out1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFPID2OUT)
            ENOFFS(enCYCLEMENUFFVAR1_PID2OUT),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_pid2_out2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_pid2_out)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_pid2_out2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFPID2OUT)
        ENOFFS(enCYCLEMENUFFVAR2_PID2OUT),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_pid2_out#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetPid2Out,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_pid2_out1, CyclingMenuStart)
#MP;//------------ Working AI1 OUT descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_ai1_out1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFAI1OUT)
            ENOFFS(enCYCLEMENUFFVAR1_AIOUT),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_ai1_out2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_ai1_out)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_ai1_out2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFAI1OUT)
        ENOFFS(enCYCLEMENUFFVAR2_AIOUT),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_ai1_out#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetAi1Out,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_ai1_out1, CyclingMenuStart)
#MP;//------------ Working AI2 OUT descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_ai2_out1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFAI2OUT)
            ENOFFS(enCYCLEMENUFFVAR1_AI2OUT),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_ai2_out2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(strvar_ff_ai2_out)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_ai2_out2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFAI2OUT)
        ENOFFS(enCYCLEMENUFFVAR2_AI2OUT),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@strvar_ff_ai2_out#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetAi2Out,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_ai2_out1, CyclingMenuStart)
#MP;//------------ Working AI3 OUT descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_ai3_out1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFAI3OUT)
            ENOFFS(enCYCLEMENUFFVAR1_AI3OUT),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_ai3_out2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_ai3_out)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_ai3_out2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFAI3OUT)
        ENOFFS(enCYCLEMENUFFVAR2_AI3OUT),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_ai3_out#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetAi3Out,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_ai3_out1, CyclingMenuStart)
#MP;//------------ AO SP descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_ao_sp1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFAOSP)
            ENOFFS(enCYCLEMENUFFVAR1_AOSP),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_ao_sp2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_ao_sp)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_ao_sp2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFAOSP)
        ENOFFS(enCYCLEMENUFFVAR2_AOSP),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_ao_sp#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetAoSp,
    },
#MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_ao_sp1, CyclingMenuStart)
#MP;//------------ Working DO SP descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_do_sp1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFDOSP)
            ENOFFS(enCYCLEMENUFFVAR1_DOSP),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_do_sp2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_DUMMY
        },
        NULL,
        NODE_LINK(var_ff_do_sp)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_do_sp2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFDOSP)
        ENOFFS(enCYCLEMENUFFVAR2_DOSP),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_DUMMY
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_var, #@var_ff_do_sp#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .data_index = 0,
        .varfunc = VARFUNC(ui_ffGetDoSp)
    },
#MP Expand EndNode()
#MP;//------------  DO2 SP descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_do2_sp1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFDO2SP)
            ENOFFS(enCYCLEMENUFFVAR1_DO2SP),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_do2_sp2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_DUMMY
        },
        NULL,
        NODE_LINK(var_ff_do2_sp)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_do2_sp2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFDO2SP)
        ENOFFS(enCYCLEMENUFFVAR2_DO2SP),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_DUMMY
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_var, #@var_ff_do2_sp#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .data_index = 0,
        .varfunc = VARFUNC(ui_ffGetDo2Sp)
    },
#MP Expand EndNode()
#MP;//------------  IS in1 descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_is_in1_1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFISIN1)
            ENOFFS(enCYCLEMENUFFVAR1_ISIN1),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_is_in1_2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_is_in1)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_is_in1_2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFISIN1)
        ENOFFS(enCYCLEMENUFFVAR2_ISIN1),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_is_in1#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetIsIn1,
    },
#MP Expand EndNode()
#MP;//------------  IS in2 descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_is_in2_1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFISIN2)
            ENOFFS(enCYCLEMENUFFVAR1_ISIN2),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_is_in2_2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_is_in2)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_is_in2_2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFISIN2)
        ENOFFS(enCYCLEMENUFFVAR2_ISIN2),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_is_in2#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetIsIn2,
    },
#MP Expand EndNode()
#MP;//------------  IS in3 descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_is_in3_1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFISIN3)
            ENOFFS(enCYCLEMENUFFVAR1_ISIN3),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_is_in3_2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_is_in3)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_is_in3_2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFISIN3)
        ENOFFS(enCYCLEMENUFFVAR2_ISIN3),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_is_in3#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetIsIn3,
    },
#MP Expand EndNode()
#MP;//------------  IS in2 descriptor ------------------
    #MP Expand StartNode(nodetype_alt, #@ff_is_in4_1#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //timeout
            #MP TEXTLABEL(FFISIN4)
            ENOFFS(enCYCLEMENUFFVAR1_ISIN4),
            NODE_LINK(#mp%sMainMenuFirstNode),
            NODE_LINK(ff_is_in4_2),
            XLATE_INDEX(xlate_OperMenu1),
           .uid = UNITSID_POSITION
        },
        NULL,
        NODE_LINK(var_ff_is_in4)
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_nav, #@ff_is_in4_2#)
    {
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT, //timeout
        #MP TEXTLABEL(FFISIN4)
        ENOFFS(enCYCLEMENUFFVAR2_ISIN4),
        NODE_LINK(#mp%sMainMenuFirstNode),
        NODE_LINK(#mp%sUIGraphNodeNext),
        XLATE_INDEX(xlate_OperMenu1),
        .uid = UNITSID_POSITION
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_strvar, #@var_ff_is_in4#)
    {
        #MP Expand NodetType()
        .checksum_offset = 0, //no checksum
        .en = UINODE_DISABLED,
        .parent = NODE_LINK(#mp%sUIGraphNodeNext),
        .strvarf = ui_ffGetIsIn4,
    },
#MP Expand EndNode()
#MP Expand StartNode(nodetype_xsel, "customer_var_ff_goback")
    {
        {
            #MP Expand NodetType()
            1,
            #MP TEXTLABEL(BLANK)
            UINODE_ENABLED,
            NODE_LINK(CyclingMenuStart), //wrap around
            NODE_LINK(CyclingMenuStart), //do it on *
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3Silent, NULL
    },
    #MP Expand EndNode()
#MP Expand CycleMenu_TrippedOrFailed(ff_do_sp1, CyclingMenuStart)
#MP Endm ;----------- CycleMenu_FfVar -------------------------------

