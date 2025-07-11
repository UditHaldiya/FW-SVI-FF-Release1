#MP Macro SigSpCharact_View
#MP
    #MP;    //-------- Characterization -------------
    #MP Expand StartNode(nodetype_nav, #@view_CharactLinear#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(LINEAR)
        ENOFFS(enCHARACT_LINEAR),
        NODE_LINK(view_CharactEq30),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_CharactEq30#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(EQUAL30)
        ENOFFS(enCHARACT_EQ30),
        NODE_LINK(view_CharactEq50),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_CharactEq50#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(EQUAL50)
        ENOFFS(enCHARACT_EQ50),
        NODE_LINK(view_CharactCAMFXEQ),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_CharactCAMFXEQ#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(CAMFXEQ)
        ENOFFS(enCHARACT_CAMFXEQ),
        NODE_LINK(view_CharactQUICK),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_CharactQUICK#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(QUICK)
        ENOFFS(enCHARACT_QUICK),
        NODE_LINK(view_CharactCUSTOM),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_nav, #@view_CharactCUSTOM#)
    {
        #MP Expand NodetType()
        0, //no timeout
        #MP TEXTLABEL(CUSTOM)
        ENOFFS(enCHARACT_CUSTOM_to_COST_FULL),
        NODE_LINK(#mp%sUIGraphNodeNext),
        NODE_LINK(man_viewData),
        XLATE_INDEX(xlate_StdMenu),
        .uid = UNITSID_NONE,
    },
    #MP Expand EndNode()
#MP Endm

#MP Macro SigSpCharact_En
#MP
    #MP     ;The next umpteen enable entries enable exactly one of the possible
    #MP     ;characterization options, some dependent on full/low cost options
    #MP ; ---------- for viewing, we'll need to retrieve congiguration data ---------
    #MP Expand StartEnEntry(#@enCHARACT_LINEAR#)
        DATAFUNC(ui_GetSpCharact), 0, CHARACT_MASK, CHARACT_LINEAR,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCHARACT_EQ30#)
        DATAFUNC(ui_GetSpCharact), 0, CHARACT_MASK, CHARACT_EQUAL_30,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCHARACT_EQ50#)
        DATAFUNC(ui_GetSpCharact), 0, CHARACT_MASK, CHARACT_EQUAL_50,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCHARACT_CAMFXEQ#)
        DATAFUNC(ui_GetSpCharact), 0, CHARACT_MASK, CHARACT_CAMFLEX_EQ30,
        #MP Expand EnEntryNext()
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCHARACT_QUICK#) ;//for viewing, regardless of cost option
        DATAFUNC(ui_GetSpCharact), 0, CHARACT_MASK, CHARACT_QUICK_OPEN,
        0 //chain terminated
    #MP Expand EndEnEntry()
    #MP Expand StartEnEntry(#@enCHARACT_CUSTOM_to_COST_FULL#)
        DATAFUNC(ui_GetSpCharact), 0, CHARACT_MASK, CHARACT_CUSTOM,
#if SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT == OPTION_ON
        #MP Expand EnEntryNext()
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT == OPTION_OFF
        #MP Expand EnEntryNext(#@enCOST_FULL#)
#endif
    #MP Expand EndEnEntry()
#MP Endm

#MP Macro SigSpCharact_Setup
    #MP;//------- Characterization --------------
    #MP Expand StartNode(nodetype_sel, #@charact_linear#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(LINEAR)
            ENOFFS(enCHARACT_LINEAR),
            NODE_LINK(charact_eq30), //next
            NODE_LINK(charact_eq30), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_EQUAL_30, //next value
        CHARACT_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@charact_eq30#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(EQUAL30)
            ENOFFS(enCHARACT_EQ30),
            NODE_LINK(charact_eq50), //next
            NODE_LINK(charact_eq50), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_EQUAL_50, //next value
        CHARACT_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@charact_eq50#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(EQUAL50)
            ENOFFS(enCHARACT_EQ50),
            NODE_LINK(charact_camfxeq), //next
            NODE_LINK(charact_camfxeq), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_CAMFLEX_EQ30, //next value
        CHARACT_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@charact_camfxeq#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(CAMFXEQ)
            ENOFFS(enCHARACT_CAMFXEQ),
            NODE_LINK(charact_quick_full), //next
            NODE_LINK(charact_quick_full), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_QUICK_OPEN, //next value
        CHARACT_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
    #MP;/* Two versions of QUICK: enabling and skipping CUSTOM
    #MP;   respectively
    #MP;*/
    #MP Expand StartNode(nodetype_sel, #@charact_quick_full#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(QUICK)
#if SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT == OPTION_ON
            ENOFFS(enCHARACT_QUICK),
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT == OPTION_OFF
            ENOFFS(enCHARACT_QUICK_to_COST_FULL),
#endif
            NODE_LINK(charact_quick_lowcost), //next
            NODE_LINK(charact_quick_lowcost), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_CUSTOM, //next value
        CHARACT_MASK, //mask
        0, //no checksum
    },
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@charact_quick_lowcost#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(QUICK)
#if SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT == OPTION_ON
            UINODE_DISABLED,
#endif
#if SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT == OPTION_OFF
            ENOFFS(enCHARACT_QUICK_to_COST_LOW),
#endif
            NODE_LINK(charact_custom), //next
            NODE_LINK(charact_custom), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_LINEAR, //next value
        CHARACT_MASK, //mask
        0 //no checksum
    },
    #MP;//      --- end two-headed QUICK item
    #MP Expand EndNode()
    #MP Expand StartNode(nodetype_sel, #@charact_custom#)
    {
        {
            #MP Expand NodetType()
            0,
            #MP TEXTLABEL(CUSTOM)
            ENOFFS(enCHARACT_CUSTOM_to_COST_FULL),
            NODE_LINK(#mp%sUIGraphNodeNext), //next
            NODE_LINK(charact_linear), //toggle
            XLATE_INDEX(xlate_StdMenu),
            .uid = UNITSID_NONE,
        },
        DATAFUNC(ui_GetSpCharact), 0,
        CHARACT_LINEAR, //next value
        CHARACT_MASK, //mask
        0 //no checksum
    },
    #MP Expand EndNode()
#MP Endm
