#MP If 0 ;Comment
/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_dispmode.inc
    \brief Macros to support UI device mode change

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_dispmode.inc $
    $Date: 1/06/12 3:15p $
    $Revision: 1 $
    $Author: Arkkhasin $

*/
/* $History: ui_dispmode.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:15p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 10:33p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * If 0 forgotten
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/07/09    Time: 10:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Compartmentalizing the UI glue layer
*/
#MP Endif

#MP Macro DispMode_Root
    #MP Expand StartNode(nodetype_nav, #@uictl#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(UICTL)
#ifdef NDEBUG
        UINODE_DISABLED,
#else
        UINODE_ENABLED,
#endif
        .advance = NODE_LINK(#mp%sMainMenuFirstNode),  //wrap around
#ifdef NDEBUG
        .select = NODE_LINK(#mp%sMainMenuFirstNode),  //doesn't matter
#else
        .select = NODE_LINK(uictl_do_0),
#endif
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm

#MP Macro DispMode_En
#ifndef NDEBUG
#MP For i=0,3
    #MP StartEnEntry({uJoin, "NUMVAL_", {%di}})
        (datafunc_t *)ui_getDispmode, 0, 7U, #mp%di,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
#MP Endfor
#endif //NDEBUG
#MP Endm


#MP Macro DispMode_Nodes
#ifndef NDEBUG
#MP ; --------- Display Line 3 helper menu -------------
  #MP For i=0,3
    #MP nexti=(i+1)%4
    #MP Expand StartNode(nodetype_sel, {uJoin, #@uictl_do_#, {%di}})
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(STAR2TOGGLE)
            ENOFFS(NUMVAL_#mp%di),
            NODE_LINK(uictl), //on +
            NODE_LINK(uictl_do_#mp%dnexti), //on *
            XLATE_INDEX(xlate_StdWiz),
            .uid = UNITSID_NONE,
        },
        (datafunc_t *)ui_getDispmode, 0,
        #mp%dnexti, //new value
        0x07U, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
  #MP Endfor
#endif
#MP Endm
