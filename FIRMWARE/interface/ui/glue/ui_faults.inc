#MP If 0
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_latch.inc
    \brief Macros to support UI faults interface functionality

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_faults.inc $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

*/
/* $History: ui_faults.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:45a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * For now, cost model applies only to MNCB
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:08p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Extracted from uifunc.{c,h}
 * Eliminated FAULT_LOW_POWER from DLT
*/
#MP Endif

#MP Macro Faults_En
    #MP For faultword_index = 1, FAULT_CODE_BYTES_REQUIRED_PROJ
      #MP Expand StartEnEntry(#@NOFAULT#, faultword_index)
        DATAFUNC(ui_GetFaultCodeData), (MN_OFFSETOF(FaultCodeData_t, fcode) + #mp%ufaultword_index) - 1, 0xff, 0x00,
       #MP If faultword_index==FAULT_CODE_BYTES_REQUIRED_PROJ
        #MP Expand EnEntryNext()
       #MP Else
        #MP Expand EnEntryNext(#@NOFAULT#, (faultword_index+1))
       #MP Endif
      #MP Expand EndEnEntry()
    #MP Endfor
    //Individual faults
  #MP Expand LoopStart(fault_count,  0, (NUM_DIFFERENT_FAULTS_PROJ-1) )
    #MP ;AK:TODO: Need an entry only if a fault is actually defined!!! Save time and memory!!!
      #MP Expand StartEnEntry(#@FAULT#, fault_count)
        DATAFUNC(ui_GetFaultCodeData), MN_OFFSETOF(FaultCodeData_t, fcode) + FWORD(#mp%uFaultEnum_%ufault_count), FBITPOS(#mp%uFaultEnum_%ufault_count), FBITPOS(#mp%uFaultEnum_%ufault_count),
        #MP Expand EnEntryNext()
      #MP Expand EndEnEntry()
  #MP Expand LoopEnd()
#MP Endm
        #MP
        #MP
#MP Macro ViewClearFaults
    #MP Expand StartNode(nodetype_nav, #@man_viewFaults#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(VIEWFLT)
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
#if SWOPTION_UI_LOWCOST_FAULTS_SHOW == OPTION_ON
        UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_FAULTS_SHOW == OPTION_OFF
        ENOFFS(COST_FULL),
#endif
#MP Else
        UINODE_ENABLED,
#MP Endif ;FEATURE_HARDWARE_PLATFORM
        NODE_LINK(man_clrFaults),
        NODE_LINK(man_viewFaults_transition),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP If 0 ;Comment
In the current UI implementation, there is a problem with descending into a
submenu with a SELECT action: if the first node is disabled, the UI engine
passes through it to whatever follows.
A workaround is to have an artificial node with timeout 1 which would be
always enabled and transition to the ADVANCE node right away.
#MP Endif ;Comment
    #MP Expand StartNode(nodetype_nav, #@man_viewFaults_transition#)
    {
        #MP Expand NodetType()
        1, //smallest timeout
        #MP TEXTLABEL(VIEWFLT)
        UINODE_ENABLED,
        NODE_LINK(nofault_node),
        NODE_LINK(man_viewFaults),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

    #MP Expand StartNode(nodetype_nav, #@man_clrFaults#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(CLRFLT)
#MP If FEATURE_HARDWARE_PLATFORM == FEATURE_HARDWARE_PLATFORM_MNCB
#if SWOPTION_UI_LOWCOST_FAULTS_CLEAR == OPTION_ON
        UINODE_ENABLED,
#endif
#if SWOPTION_UI_LOWCOST_FAULTS_CLEAR == OPTION_OFF
        ENOFFS(COST_FULL),
#endif
#MP Else
        UINODE_ENABLED,
#MP Endif ;FEATURE_HARDWARE_PLATFORM
        .advance = NODE_LINK(#mp%sUIGraphNodeNext),
        //AK:Here! .select = NODE_LINK(man_clrFaults), //no * action other than exit function; stay here
        .select = NODE_LINK(man_clrFaults#mp%sLockCheckSuffix),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()

#MP Endm
