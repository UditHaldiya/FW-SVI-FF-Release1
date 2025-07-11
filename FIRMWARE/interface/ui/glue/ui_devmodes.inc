#MP If 0
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uidevmodes.inc
    \brief Macros for changing modes in MNCB

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_devmodes.inc $
    $Date: 1/20/12 12:07p $
    $Revision: 2 $
    $Author: Arkkhasin $
*/
/* $History: ui_devmodes.inc $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 12:07p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * TFS:8577 - old device mode adaptation
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/04/09   Time: 3:35p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Replaced pattern "0xFFU & ~" with "0xFFU ^" to make IAR 5.40 happy
 *
 * *****************  Version 9  *****************
 * User: Derek Li     Date: 6/25/09    Time: 3:21p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Modified Fsafe2Setup node text as "downarror SETUP"
 *
 * *****************  Version 8  *****************
 * User: Derek Li     Date: 6/10/09    Time: 5:26p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Added Fsafe_to_Normal, Setup_to_Normal and man_to_Normal
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 6/04/09    Time: 4:24p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Disabled clear faults and mode changes from UI in the presence of the
 * write-protect jumper
 *
 * *****************  Version 6  *****************
 * User: Derek Li     Date: 6/04/09    Time: 3:16p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Added Fsafe2Oper:failsafe mode to Normal mode
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:40p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Extracted more UI glue into separate supporting files
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 7:16p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Changed a text label to a better one
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/13/09    Time: 2:35a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Removed Manual Mode access from HART and UI for DLT if not controller
 * (i.e. for now, always)
 *
 * *****************  Version 2  *****************
 * User: Derek Li     Date: 3/12/09    Time: 2:38p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui
 * Add transitions from Normal mode to SETUP mode, and SETUP mode to
 * NORMAL mode
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/08/09    Time: 4:35p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui
 * Macro-ized mode change nodes, extracted in separate .inc files, with
 * new lock scheme
*/
#MP Endif

#MP Macro DevModes_En
    #MP    ;enabled in manual mode only
    #MP Expand StartEnEntry(#@OPMODE_MAN#)
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU, MODE_MANUAL, //The whole byte must match
        #MP Expand EnEntryNext(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#)
    #MP Expand EndEnEntry()
    #MP    ;enabled in oper mode only
    #MP Expand StartEnEntry(#@OPMODE_OPER#)
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU, MODE_OPERATE, //The whole byte must match
        #MP Expand EnEntryNext(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#)
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@OPMODE_OPERMAN#)
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU ^ (MODE_OPERATE|MODE_MANUAL), 0U, //The whole byte must match
        #MP Expand EnEntryNext(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#)
    #MP Expand EndEnEntry()
    #MP    ;----------- enabled in failsafe mode(s) only ----------------
    #MP Expand StartEnEntry(#@OPMODE_SETUP#)
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU, MODE_OOS, //The whole byte must match
#else
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU, MODE_SETUP, //The whole byte must match
#endif
        #MP Expand EnEntryNext(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#)
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@XMODE_SETUP#)
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU, MODE_OOS, //The whole byte must match
#else
        ui_GetXModeData, 0, 0xFFU, MODE_SETUP, //The whole byte must match
#endif
        #MP Expand EnEntryNext(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#)
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@OPMODE_FSAFE#) ;failsafe OR low power
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, submode), SUBMODE_OOS_FAILSAFE|SUBMODE_OOS_FATAL, SUBMODE_OOS_FAILSAFE, //low power is different
        #MP Expand EnEntryNext(#@OPMODE_OOS#)
#else
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU, MODE_FAILSAFE, //The whole byte must match
        #MP Expand EnEntryNext()
#endif
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#) ;not failsafe NEITHER low power
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, submode), SUBMODE_OOS_FAILSAFE|SUBMODE_OOS_LOWPOWER, 0U,
        #MP Expand EnEntryNext()
#else
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), MODE_FAILSAFE, 0U,
        #MP Expand EnEntryNext(#@OPMODE_NOT_LOWPWR#)
#endif
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@OPMODE_LOWPWR#)
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, submode), SUBMODE_OOS_LOWPOWER|SUBMODE_OOS_FAILSAFE|SUBMODE_OOS_FATAL, SUBMODE_OOS_LOWPOWER, //failsafe is different
        #MP Expand EnEntryNext(#@OPMODE_OOS#)
#else
        DATAFUNC(ui_GetDevSubmode), 0, SUBMODE_LOWPOWER, SUBMODE_LOWPOWER,
        #MP Expand EnEntryNext()
#endif
    #MP Expand EndEnEntry()

    #MP Expand StartEnEntry(#@OPMODE_OOS#)
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), OPMODE_MASK, MODE_OOS,
#else
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0, 0, //dummy
#endif
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()

    #MP Expand StartEnEntry(#@OPMODE_NOT_LOWPWR#)
#ifdef OLD_DEVMODE
        DATAFUNC(ui_GetDevSubmode), 0, 0, 0, //dummy
#else
        DATAFUNC(ui_GetDevSubmode), 0, SUBMODE_LOWPOWER, 0,
#endif
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
#MP ;~~~~~~~~ Late-addition enable nodes ~~~~~~~~~~~
#MP ;---- MANUAL or SETUP mode
    #MP Expand StartEnEntry(#@OPMODE_SETUP_OR_MANUAL#)
#ifdef OLD_DEVMODE
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), MODE_OPERATE & ~MODE_NOT_OOS, 0U,
#else
        ui_GetModeData, MN_OFFSETOF(ModeData_t, mode), 0xFFU ^ (MODE_MANUAL|MODE_SETUP), 0U,
#endif
        #MP Expand EnEntryNext(#@OPMODE_NOT_FSAFE_NOR_LOWPOWER#)
    #MP Expand EndEnEntry()
#MP ;----- FAILHIGH-related
    #MP Expand StartEnEntry(#@FAILHIGH_AND_SETUP_OR_MANUAL#)
        DATAFUNC(ui_IsFailedHigh), 0, 0x01, 0x01,
        #MP Expand EnEntryNext(#@OPMODE_SETUP_OR_MANUAL#)
    #MP Expand EndEnEntry()
#MP If FEATURE_POSITION_CONTROL == FEATURE_POSITION_CONTROL_INCLUDED
    #MP Expand StartEnEntry(#@NOFAILHIGH_AND_SETUP_OR_MANUAL#)
        DATAFUNC(ui_IsFailedHigh), 0, 0x01, 0x00,
        #MP Expand EnEntryNext(#@OPMODE_SETUP_OR_MANUAL#)
    #MP Expand EndEnEntry()
#MP Endif
#MP Endm

#MP Macro DevModesStretch ;not a submenu! inline stretch of modes
#MP Setstr Normal2Manual_autoname = #@Op2Man#
#MP Setstr Manual2Setup_autoname = #@man_toSetup#
#MP Setstr Normal2Setup_autoname = #@OptoSetup#
#MP Save uinode_autoname
    #MP Expand StartNode(nodetype_nav, #@setup_toMan#)
    { //-------- Setup --> Manual: no lock check
        #MP Expand NodetType()
        0,
        #MP TEXTLABEL(UP2MAN)
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
        ENOFFS(OPMODE_SETUP_ControllerOption),
#MP Endif
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
#MP     If FEATURE_CPU == FEATURE_CPU_STM32F10xxE
        .en = UINODE_DISABLED,
#MP     Else
        ENOFFS(OPMODE_SETUP),
#MP     Endif ;FEATURE_CPU == FEATURE_CPU_STM32F10xxE
#MP Endif ;FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
        NODE_LINK(Fsafe),
        NODE_LINK(up_toMan), //no button lock check
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand DeclareStartNode(#@UINODEID_INITFSAFE#)
    #MP Expand StartNode(nodetype_nav, #@Fsafe#)
    { //----------Fsafe indicator: no action
        #MP Expand NodetType()
        0, //no refresh: faults may be cleared outside UI, but failsafe stays put until a mode change
        #MP TEXTLABEL(FSAFE)
        ENOFFS(OPMODE_FSAFE),
        NODE_LINK(Lowpwr), //to include this node in the +- loop
        NODE_LINK(Fsafe), //nothing to do: stay here
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@Lowpwr#)
    { //----------Lowpwr (but not failsafe) indicator: no action
        #MP Expand NodetType()
        -TMOUT_TICKS_REFRESH, //refresh rate, in case we stay in setup or failsafe
        #MP TEXTLABEL(LOW_POWER)
        ENOFFS(OPMODE_LOWPWR),
        //NODE_LINK(toOper), //to include this node in the +- loop
        NODE_LINK(#mp%sUIGraphNodeNext), //to include this node in the +- loop
        NODE_LINK(Lowpwr),  //nothing to do: stay here
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP ;==============================================================================================
#MP If (FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT) || (FEATURE_CPU == FEATURE_CPU_STM32F10xxE)
        #MP ;Transition action from Setup to Normal
        #MP Expand StartNode(nodetype_xsel, #@Setup_toOper#)
    {
        {
            #MP Expand NodetType()
                0, //no timeout
                #MP TEXTLABEL(UP2OPER)
                ENOFFS(OPMODE_SETUP),
                .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(Setup_to_Normal),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, NULL
    },
    #MP Expand EndNode()
#MP Endif ;FEATURE_HARDWARE_PLATFORM_DLT
        #MP ;==============================================================================================

    #MP;// --------- Navigation to the ->OPER or ->MAN (Oper mode) ------------------
    #MP Expand StartNode(nodetype_xsel, #@toOper#)
    { //Normal to cycling menu: no lock check
        {
            #MP Expand NodetType()
            0, //no timeout
            #MP TEXTLABEL(OPER)
            ENOFFS(OPMODE_OPER),
            NODE_LINK(Op2Man),
            NODE_LINK(CyclingMenuStart),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        ui_Clr_Lin3, NULL
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, Normal2Manual_autoname)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(DOWN2MAN)
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
        ENOFFS(OPMODE_OPER_ControllerOption),
#MP Endif
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
#MP     If FEATURE_CPU == FEATURE_CPU_STM32F10xxE
        .en = UINODE_DISABLED,
#MP     Else
        ENOFFS(OPMODE_OPER),
#MP     Endif ;FEATURE_CPU FEATURE_CPU_STM32F10xxE
#MP Endif ;FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        #MP Setstr uinode_autoname = Normal2Manual_autoname LockCheckSuffix ;
        .select = NODE_LINK(#mp%suinode_autoname),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_xsel, "go_failsafe") ;force failsafe
    { //From non-Normal failhigh to failsafe
        {
            #MP Expand NodetType()
            -TMOUT_TICKS_REFRESH, //refresh rate, in case the fault(s) go away
            #MP TEXTLABEL(TO_FSAFE)
            ENOFFS(FAILHIGH_AND_SETUP_OR_MANUAL),
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(go_failsafe),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_setFailSafeMode
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, Manual2Setup_autoname)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(SETUP)
        ENOFFS(OPMODE_MAN),
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        #MP Setstr uinode_autoname = Manual2Setup_autoname LockCheckSuffix ;
        .select = NODE_LINK(#mp%suinode_autoname),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP ;=================================================================================
    #MP Expand StartNode(nodetype_nav, Normal2Setup_autoname)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(SETUP)
        ENOFFS(OPMODE_OPER),
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        #MP Setstr uinode_autoname = Normal2Setup_autoname LockCheckSuffix ;
        .select = NODE_LINK(#mp%suinode_autoname),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP ;=================================================================================
    #MP Expand StartNode(nodetype_xsel, #@man_toOper#)
    {
        {
            #MP Expand NodetType(#@UINODE_NO_STOP#)
            0, //no timeout
            #MP TEXTLABEL(UP2OPER)
            ENOFFS(OPMODE_MAN),
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(man_to_Normal), //so if mode is not Manual, will propagate to oper loop or fsafe
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, NULL //AK:TODO:Can it fail and have to be accounted for?
    },
    #MP Expand EndNode()
    #MP ;=================================================================================

    #MP Expand StartNode(nodetype_nav, #@man2Man#)
    { //we don't switch mode (already correct); just change the menu
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(MAN)
        ENOFFS(OPMODE_MAN),
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(CyclingMenuStart), //go to cycling menu (with Manual Mode personality)
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

#MP ;AK::
#MP If FEATURE_UI_TRANSITION_FSAFE2MANUAL == FEATURE_UI_TRANSITION_FSAFE2MANUAL_ENABLED
#MP Setstr Failsafe2Manual_autoname = #@Fsafe2Man#
    #MP Expand StartNode(nodetype_nav, Failsafe2Manual_autoname)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(UP2MAN)
#MP If FEATURE_CPU == FEATURE_CPU_STM32F10xxE
        .en = UINODE_DISABLED,
#MP Else
        ENOFFS(OPMODE_FSAFE),
#MP Endif ;FEATURE_CPU == FEATURE_CPU_STM32F10xxE
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        #MP Setstr uinode_autoname = Failsafe2Manual_autoname LockCheckSuffix ;
        .select = NODE_LINK(#mp%suinode_autoname), //button lock check
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endif ;If FEATURE_UI_TRANSITION_FSAFE2MANUAL == FEATURE_UI_TRANSITION_FSAFE2MANUAL_ENABLED
#MP If FEATURE_UI_TRANSITION_FSAFE2SETUP == FEATURE_UI_TRANSITION_FSAFE2SETUP_ENABLED
#MP Setstr Failsafe2Setup_autoname = #@Fsafe2Setup#
    #MP Expand StartNode(nodetype_nav, Failsafe2Setup_autoname)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP; TEXTLABEL(UP2SETUP)
        #MP TEXTLABEL(SETUP)
        ENOFFS(OPMODE_FSAFE),
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        #MP Setstr uinode_autoname = Failsafe2Setup_autoname LockCheckSuffix ;
        .select = NODE_LINK(#mp%suinode_autoname), //button lock check
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endif ;If FEATURE_UI_TRANSITION_FSAFE2SETUP == FEATURE_UI_TRANSITION_FSAFE2SETUP_ENABLED

#MP ;==============================================================================================
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
        #MP ;Transition action from Failsafe to Normal
        #MP Expand StartNode(nodetype_xsel, #@Fsafe2Oper#)
    {
        {
            #MP Expand NodetType()
                0, //no timeout
                #MP TEXTLABEL(UP2OPER)
                ENOFFS(OPMODE_FSAFE),
                .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            .select = NODE_LINK(Fsafe_to_Normal),
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, NULL
    },
    #MP Expand EndNode()
#MP Endif ;FEATURE_HARDWARE_PLATFORM_DLT
#MP ;==============================================================================================

#MP
    #MP Expand StartNode(nodetype_nav, #@Fsafe_reset#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(RESET)
        ENOFFS(OPMODE_FSAFE),
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(Fsafe_reset_do),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

#MP Restore uinode_autoname
#MP Endm

#MP Macro DevModesHelpers ;misc. supporting nodes for device mode change
#MP Save uinode_autoname
#MP ;Transition action from Setup to Manual
    #MP Expand StartNode(nodetype_xsel, #@up_toMan#)
    {
        {
            #MP Expand NodetType()
            .tmout = TMOUT_TICKS_DEFAULT,
            #MP TEXTLABEL(FAILED)
            .en = UINODE_ENABLED,
            .advance = NODE_LINK(setup_toMan), //return if enabled: this link is taken if ui_setManModeNaked() failed
            .select = NODE_LINK(man_toSetup), //job done
            .xfuncx = XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setManModeNaked, NULL
    },
    #MP Expand EndNode()

#MP ;Transition action from Normal to Manual
#MP If FEATURE_WPROTECT_MODE == FEATURE_WPROTECT_MODE_FALSE
    #MP ProtectedEntry[%sNormal2Manual_autoname, 0, LOCK_MANUAL]
#MP Else
    #MP ProtectedEntry[%sNormal2Manual_autoname, CHECK_WRITE_PROTECTED, LOCK_MANUAL]
#MP Endif
#MP Setstr uinode_autoname = Normal2Manual_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    { //The trick is that the destination node takes care of the right look whether the mode switch succeeds or fails
        {
            #MP Expand NodetType(#@UINODE_NO_STOP#)
            0, //no timeout
            #MP TEXTLABEL(DOWN2MAN) ;//don't care: the node will always be disabled
            UINODE_ENABLED, //to force execution of the entry function
            NODE_LINK(CyclingMenuStart), //actually, don't care
            NODE_LINK(CyclingMenuStart),
            XLATE_INDEX(xlate_SelectIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setManMode, NULL //ui_setManMode must always return non-0 to pass through a disabled node to CyclingMenuStart and maybe on
    },
    #MP Expand EndNode()

#MP ;Transition action from Manual to Setup
#MP If FEATURE_WPROTECT_MODE == FEATURE_WPROTECT_MODE_FALSE
    #MP ProtectedEntry[%sManual2Setup_autoname, 0, LOCK_SETUP]
#MP Else
    #MP ProtectedEntry[%sManual2Setup_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
#MP Endif
#MP Setstr uinode_autoname = Manual2Setup_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    { //In case ui_setOOSMode fails to set normal (setup) OOS mode
        {
            #MP Expand NodetType()
            0, //no timeout
            #MP TEXTLABEL(CONF)
            UINODE_ENABLED, //to force entry function
            NODE_LINK(#mp%sUIGraphNodeNext), //entry failure if enabled
            NODE_LINK(#mp%sSetupMenuFirstNode),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setOOSMode, NULL //ui_setOOSMode must return non-0 iff successful
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@setup_failure#)
    { //A failure indicator on failure to enter setup
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT,
        #MP TEXTLABEL(FAILED)
        UINODE_ENABLED,
        NODE_LINK(#mp%sManual2Setup_autoname),
        NODE_LINK(#mp%sManual2Setup_autoname),
        XLATE_INDEX(xlate_NextIfEnabled),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

#MP ;==============================================================================================
#MP ;Transition action from Normal to Setup
#MP If FEATURE_WPROTECT_MODE == FEATURE_WPROTECT_MODE_FALSE
    #MP ProtectedEntry[%sNormal2Setup_autoname, 0, LOCK_SETUP]
#MP Else
    #MP ProtectedEntry[%sNormal2Setup_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
#MP Endif
#MP Setstr uinode_autoname = Normal2Setup_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    {   //In case ui_setOOSMode fails to set normal (setup) OOS mode
        {
            #MP Expand NodetType()
            0, //no timeout
            #MP TEXTLABEL(UP2SETUP)
            UINODE_ENABLED, //to force entry function
            NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(#mp%sSetupMenuFirstNode),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setOOSMode, NULL //ui_setOOSMode must return non-0 iff successful
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@to_setup_failure#)
    {   //A failure indicator on failure to enter setup
        #MP Expand NodetType()
        TMOUT_TICKS_DEFAULT,
        #MP TEXTLABEL(FAILED)
        UINODE_ENABLED,
        NODE_LINK(#mp%sNormal2Setup_autoname),//after show "FAILURE", go to the node "Normal2Setup_autoname"
        NODE_LINK(#mp%sNormal2Setup_autoname),
        XLATE_INDEX(xlate_NextIfEnabled),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP ;==============================================================================================

#MP If Defined(Failsafe2Manual_autoname {STR})
#MP If FEATURE_WPROTECT_MODE == FEATURE_WPROTECT_MODE_FALSE
    #MP ProtectedEntry[%sFailsafe2Manual_autoname, 0, LOCK_MANUAL]
#MP Else
    #MP ProtectedEntry[%sFailsafe2Manual_autoname, CHECK_WRITE_PROTECTED, LOCK_MANUAL]
#MP Endif
#MP Setstr uinode_autoname = Failsafe2Manual_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //enable timeout default
            #MP TEXTLABEL(FSAFE)
            UINODE_ENABLED, //to force entry function
            NODE_LINK(#mp%sFailsafe2Manual_autoname), //return if enabled
            NODE_LINK(CyclingMenuStart), //to cycling menu if disabled
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setManModeNaked, NULL
    },
    #MP Expand EndNode()
#MP Endif ;Defined(Failsafe2Manual_autoname {STR})

#MP If Defined(Failsafe2Setup_autoname {STR})
#MP If FEATURE_WPROTECT_MODE == FEATURE_WPROTECT_MODE_FALSE
    #MP ProtectedEntry[%sFailsafe2Setup_autoname, 0, LOCK_SETUP]
#MP Else
    #MP ProtectedEntry[%sFailsafe2Setup_autoname, CHECK_WRITE_PROTECTED, LOCK_SETUP]
#MP Endif
#MP Setstr uinode_autoname = Failsafe2Setup_autoname TargetNodeSuffix
    #MP Expand StartNode(nodetype_xsel, uinode_autoname)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //enable timeout default
            #MP TEXTLABEL(FSAFE)
            UINODE_ENABLED, //to force entry function
            NODE_LINK(#mp%sFailsafe2Setup_autoname), //return if enabled
            NODE_LINK(#mp%sSetupMenuFirstNode), //to start menu if disabled
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setOOSMode, NULL
        //if ui_setOOSMode return false, this node enable, show "FAILSAFE", then go to advance node;
        //if ui_setOOSMode return true, this node disable, so go to select node;
    },
    #MP Expand EndNode()
#MP Endif ;Defined(Failsafe2Setup_autoname {STR})

#MP ;==============================================================================================
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT
    #MP Expand StartNode(nodetype_xsel, #@Fsafe_to_Normal#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //enable timeout default
            #MP TEXTLABEL(FSAFE)
            UINODE_ENABLED, //to force entry function
            //ENOFFS(OPMODE_FSAFE),
            .advance = NODE_LINK(Fsafe2Oper), //return if enabled
            .select = NODE_LINK(CyclingMenuStart), //to start menu if disabled
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setOperMode, NULL
        //if ui_setOperMode return false, this node enable, show "FAILSAFE", then go to advance node;
        //if ui_setOperMode return true, this node disable, so go to select node; if there is any fault, then go to FAILSAFE mode automatically
    },
    #MP Expand EndNode()
#MP Endif
#MP ;==============================================================================================

#MP ;==============================================================================================
#MP If (FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_DLT) || (FEATURE_CPU == FEATURE_CPU_STM32F10xxE)
        #MP ;Transition action from Setup to Normal
        #MP Expand StartNode(nodetype_xsel, #@Setup_to_Normal#)
    {
        {
            #MP Expand NodetType()
                TMOUT_TICKS_DEFAULT, //enable timeout default
                #MP TEXTLABEL(FAILED)
                //ENOFFS(OPMODE_SETUP),
                UINODE_ENABLED, //to force entry function
                .advance = NODE_LINK(Setup_toOper), //return if enabled
            .select = NODE_LINK(CyclingMenuStart), //to start menu if disabled
            //XLATE_INDEX(xlate_StdMenu),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setOperMode, NULL
        //if ui_setOperMode return false, this node enable, show "FAILURE", then go to advance node;
        //if ui_setOperMode return true, this node disable, so go to select node;
    },
    #MP Expand EndNode()
#MP Endif ;FEATURE_HARDWARE_PLATFORM_DLT
#MP ;==========================================================================================

#MP ;=================================================================================
    #MP Expand StartNode(nodetype_xsel, #@man_to_Normal#)
    {
        {
            #MP Expand NodetType()
            TMOUT_TICKS_DEFAULT, //enable timeout default
            #MP TEXTLABEL(FAILED)
            //ENOFFS(OPMODE_MAN),
            UINODE_ENABLED, //to force entry function
            .advance = NODE_LINK(man_toOper), //return if enabled
            .select = NODE_LINK(CyclingMenuStart), //to start menu if disabled
            //XLATE_INDEX(xlate_StdMenu),
            XLATE_INDEX(xlate_NextIfEnabled),
            .uid = UNITSID_NONE,
        },
        ui_setOperMode, NULL
        //if ui_setOperMode return false, this node enable, show "FAILURE", then go to advance node;
        //if ui_setOperMode return true, this node disable, so go to select node;
    },
    #MP Expand EndNode()
#MP ;=================================================================================

    #MP Expand StartNode(nodetype_xsel, #@Fsafe_reset_do#)
    {
        {
            #MP Expand NodetType()
            1, //smallest timeout
            #MP TEXTLABEL(WAIT)
            ENOFFS(OPMODE_FSAFE),
            NODE_LINK(Fsafe_reset_do),
            NODE_LINK(Fsafe_reset_do), //no * action other than exit function; stay here if you can :)
            XLATE_INDEX(xlate_SelectIfEnabled),
            .uid = UNITSID_NONE,
        },
        NULL, ui_reset //actually, ui_reset won't return, so the timeout is to give a chance to the display
    },
    #MP Expand EndNode()

#MP Restore uinode_autoname

#MP Endm