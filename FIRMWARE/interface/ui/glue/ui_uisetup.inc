#MP If 0
/*
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_uisetup.inc
    \brief Macros for UI setup

    This file cannot be processed by itself; it must be included in a
    .u file to generate the correct output.

    CPU: Any

    OWNER: Derek Li

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_uisetup.inc $
    $Date: 1/06/12 3:16p $
    $Revision: 1 $
    $Author: Arkkhasin $
*/
/* $History: ui_uisetup.inc $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 3:16p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue
 * First check-in of UI wiring adapted to UI module and LCX codebase
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/03/09    Time: 11:50a
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * glue for UI's own setup functionality extracted here
*/
#MP Endif

#MP;--------------------------Macro Language------------------------------------
#MP Macro Language ;Root node
#MP Save uinode_autoname
#MP Setstr Language_autoname = "Language"
	#MP Expand StartNode(nodetype_xsel, Language_autoname)
    {
        {
            #MP Expand NodetType()
            .tmout = 0,
            #MP TEXTLABEL(LANGNAME)
            UINODE_ENABLED,
            .advance = NODE_LINK(#mp%sUIGraphNodeNext),
            NODE_LINK(Language), //toggle by wrapping to itself
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        NULL, ui_LangIncrement
    },
    #MP Expand EndNode()

#MP Restore uinode_autoname
#MP Endm; End Macro Language
#MP;--------------------------End Macro Language--------------------------------

#MP;/* This line marks the end of the source */
